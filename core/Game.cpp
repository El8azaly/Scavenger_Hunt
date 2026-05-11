#include "core/Game.h"

#include "entities/GameObject.h"
#include "entities/Entity.h"
#include "entities/Player.h"
#include "entities/InteractiveObject.h"
#include "entities/Container.h"
#include "entities/CollectibleItem.h"
#include "entities/HiddenArea.h"
#include "entities/StaticPlatform.h"

#include "systems/InventorySystem.h"
#include "systems/QuestSystem.h"
#include "systems/ScoreManager.h"

#include "data/LevelLoader.h"

#include "ui/HUD.h"
#include "core/Constants.h"
#include <QPainter>
#include <QDebug>
#include <algorithm>
#include <QMap>
#include "ui/sprite/SkyBackground.h"
#include "data/levels/Level0.h"
#include "data/levels/Level1.h"
#include <QSettings>

#include "data/levels/Level3.h"
#include "entities/Cannon.h"
#include "entities/Cannonball.h"
#include "entities/CaptainStar.h"
#include "entities/FierceTooth.h"
#include "entities/QuizNpc.h"

class SkyBackground;

Game::Game(QObject* parent)
    : QObject(parent) {
    m_stateManager  = std::make_unique<GameStateManager>(this);
    m_input = std::make_unique<InputHandler>(this);
    m_camera = std::make_unique<Camera>();
    m_collision = std::make_unique<CollisionEngine>();
    m_inventory = std::make_unique<InventorySystem>(this);
    m_quest = std::make_unique<QuestSystem>(m_inventory.get(), this);
    m_score = std::make_unique<ScoreManager>(this);
    m_interactPopup = std::make_unique<SlicedSprite>("interact_popup");
    m_captainStarPopup = std::make_unique<SlicedSprite>("captain_star_dialog_1x1");
    m_captainStarErrorPopup = std::make_unique<SlicedSprite>("captain_star_error_dialog_1x1");

    connect(m_stateManager.get(), &GameStateManager::stateChanged, this, &Game::stateChanged);
    connect(m_stateManager.get(), &GameStateManager::stateChanged, this, &Game::stateChanged);
    connect(m_quest.get(), &QuestSystem::allTargetsFound, this, [this]() {
        m_allTargetsFound = true;
        m_showStarError = false;
    });
    connect(m_score.get(), &ScoreManager::timeUp, this, [this]() {
        m_stateManager->setState(GameState::GAME_OVER);
    });
    connect(m_score.get(), &ScoreManager::scoreChanged, this, &Game::scoreChanged);

    m_skyBg = new SkyBackground();
    m_animationTimer = new QTimer(this);
    connect(m_animationTimer, &QTimer::timeout, this, [this]{ m_skyBg->update(16); });
    m_animationTimer->start(16);
}

Game::~Game() {
    clearLevel();
}

void Game::startNewGame(int levelNumber) {
    m_currentLevel = levelNumber;
    clearLevel();
    m_allTargetsFound = false;
    m_showStarError = false;
    m_input->reset();
    m_score->reset();
    m_inventory->clear();
    m_isDying = false;

    LevelData data = LevelLoader::load(m_currentLevel);
    m_score->startTimer(data.timeLimitSec);

    m_quest->setTargets(data.targetIds);

    spawnEntities(data);
    m_stateManager->setState(GameState::PLAYING);
}
void Game::startNextLevel() {
    startNewGame(m_currentLevel + 1);
}
void Game::restartLevel() { startNewGame(m_currentLevel); }

void Game::pauseGame() {
    if (m_stateManager->isPlaying()) {
        m_score->stopTimer();
        m_stateManager->setState(GameState::PAUSED);
    }
}

void Game::resumeGame() {
    if (m_stateManager->isPaused()) {
        m_score->startTimer(m_score->timeLeft());
        m_stateManager->setState(GameState::PLAYING);
    }
}

void Game::exitToMenu() {
    m_score->stopTimer();
    clearLevel();
    m_stateManager->setState(GameState::MAIN_MENU);
}

void Game::onPuzzleSolved(const QString& objectId) {
    InteractiveObject* obj = findInteractableById(objectId);
    if (!obj) return;
    obj->unlock();
    InteractionResult result = obj->interact();
    processInteractionResult(result, obj);
    m_score->addScore(Constants::SCORE_PUZZLE_SOLVED);
}

void Game::onPuzzleFailed(const QString& objectId) { Q_UNUSED(objectId) }

GameState Game::currentState() const { return m_stateManager->current(); }
InventorySystem *Game::inventory() const { return m_inventory.get(); }
QuestSystem *Game::quest() const { return m_quest.get(); }
ScoreManager *Game::score() const { return m_score.get(); }

QString Game::getWorldPosString(const QPoint& screenPos) const {
    if (!m_camera) return "0, 0";
    float worldX = screenPos.x() + m_camera->offsetX();
    float worldY = screenPos.y() + m_camera->offsetY();
    float scale = Constants::UI_SCALE;
    float yOffset = 0.0f;
    if (m_currentLevelObj) {
        yOffset = m_currentLevelObj->verticalOffset();
    }
    int levelX = static_cast<int>(worldX / scale);
    int levelY = static_cast<int>((worldY - yOffset) / scale);
    return QString("%1, %2").arg(levelX).arg(levelY);
}

void Game::update(int deltaTimeMs) {
    if (!m_stateManager->isPlaying()) return;

    if (m_doorPhase != DoorPhase::None) {

        for (GameObject* obj : m_entities) {
            if (!obj || !obj->isActive()) continue;
            if (Door* door = dynamic_cast<Door*>(obj)) {
                door->notifyDelta(deltaTimeMs);
                door->update();
            }
        }

        if (m_doorPhase == DoorPhase::WaitingForOpenAnim) {

            if (m_activeDoor && m_activeDoor->isOpenAnimDone()) {

                m_doorPhase       = DoorPhase::FadingOut;
                m_doorFadeElapsed = 0;
                m_doorFadeOpacity = 1.0f;
            }
        }
        else if (m_doorPhase == DoorPhase::FadingOut) {
            m_doorFadeElapsed += deltaTimeMs;
            float t = qMin(1.0f, (float)m_doorFadeElapsed / (float)m_doorFadeTimeMs);
            m_doorFadeOpacity = 1.0f - t;

            if (t >= 1.0f) {

                m_doorFadeOpacity = 0.0f;
                if (m_activeDoor && m_player) {
                    QPointF dest = m_activeDoor->teleportDestination();
                    m_player->setPosition(dest.x(), dest.y());
                    m_camera->snapTo(m_player->centreX(), m_player->centreY());
                    m_activeDoor->onFadeOutComplete();
                }
                m_doorPhase       = DoorPhase::FadingIn;
                m_doorFadeElapsed = 0;
            }
        }
        else if (m_doorPhase == DoorPhase::FadingIn) {
            m_doorFadeElapsed += deltaTimeMs;
            float t = qMin(1.0f, (float)m_doorFadeElapsed / (float)m_doorFadeTimeMs);
            m_doorFadeOpacity = t;

            if (t >= 1.0f) {
                m_doorFadeOpacity = 1.0f;

                if (m_activeDoor)
                    m_activeDoor->onFadeInComplete();

                m_activeDoor = nullptr;
                m_doorPhase  = DoorPhase::None;
            }
        }

        if (m_player) m_player->updateAnimation();
        advanceCamera();
        m_input->endFrame();
        return;
    }

    processInput();
    updateEntities();
    runCollision();

    if (m_player) m_player->updateAnimation();
    if (m_currentLevelObj) m_currentLevelObj->update(deltaTimeMs);

    advanceCamera();
    removeInactiveEntities();

    bool showPopup = (m_nearestInteractable != nullptr && m_nearestInteractable->isInteractable());
    if (showPopup) {
        m_popupOpacity += 0.25f;
        if (m_popupOpacity > 1.0f) m_popupOpacity = 1.0f;
        m_lastPopupX = m_nearestInteractable->centreX();
        m_lastPopupY = m_nearestInteractable->y();
        m_lastPopupWasCaptainStar = (dynamic_cast<CaptainStar*>(m_nearestInteractable) != nullptr);
        m_lastPopupWasDoor        = (dynamic_cast<Door*>(m_nearestInteractable) != nullptr);
        m_lastPopupWasQuizNpc     = (dynamic_cast<QuizNpc*>(m_nearestInteractable) != nullptr);
    } else {
        m_popupOpacity -= 0.25f;
        if (m_popupOpacity < 0.0f) m_popupOpacity = 0.0f;
    }
    m_input->endFrame();

    if (m_player && m_player->getHealth() <= 0) {
        if (m_stateManager->current() != GameState::GAME_OVER && !m_isDying) {
            m_isDying = true;
            QTimer::singleShot(1500, this, [this]() {
                m_stateManager->setState(GameState::GAME_OVER);
            });
        }
    }
}
void Game::processInput() {
    if (!m_player) return;
    float currentSpeed = m_player->isInTrap() ? (Constants::PLAYER_SPEED * 0.5f) : Constants::PLAYER_SPEED;
    float velX = 0;
    if (m_input->isHeld(GameAction::MOVE_LEFT))  velX = -currentSpeed;
    if (m_input->isHeld(GameAction::MOVE_RIGHT)) velX =  currentSpeed;
    m_player->setVelocityX(velX);

    if (m_input->isHeld(GameAction::JUMP) && m_player->isOnGround() && m_player->getJumpCooldown() <= 0) {
        m_player->jump(Constants::JUMP_VELOCITY);
        m_player->setJumpCooldown(24);
    }
    if (m_input->wasJustPressed(GameAction::PAUSE)) {
        pauseGame();
    }
}

void Game::handleMousePress(int qtMouseButton) {
    if (m_stateManager->isPlaying() && m_player && qtMouseButton == Qt::LeftButton) {
        if (m_player->attack()) {
            m_camera->addShake(2, 8.0f);
        }
    }
}

void Game::updateEntities() {
    bool playerJustHit = (m_player && m_player->getAttackTimer() == 23);
    QVector<GameObject*> newEntities;

    for (GameObject* obj : m_entities) {
        if (!obj || !obj->isActive()) continue;

        if (Door* door = dynamic_cast<Door*>(obj)) {
            door->notifyDelta(16);
        }

        obj->update();

        if (Enemy* enemy = dynamic_cast<Enemy*>(obj)) {
            if (CollectibleItem* dropped = enemy->takeDroppedItem()) {
                newEntities.append(dropped);
            }
            if (auto* cannon = dynamic_cast<Cannon*>(enemy)) {
                QVector<Cannonball*> balls = cannon->takeNewProjectiles();
                for (Cannonball* b : balls) {
                    newEntities.append(b);
                }
            }
        }

        if (playerJustHit) {
            if (AttackableEntity* attackable = dynamic_cast<AttackableEntity*>(obj)) {
                float dist = std::abs(m_player->centreX() - attackable->centreX());
                float verticalDist = std::abs(m_player->y() - attackable->y());

                bool facingEnemy = (m_player->isFacingRight() && attackable->x() > m_player->x()) ||
                                   (!m_player->isFacingRight() && attackable->x() < m_player->x());

                bool isVeryClose = dist < 40.0f;
                if ((dist < 105.0f && facingEnemy || isVeryClose) && verticalDist < 60.0f) {
                    attackable->takeDamage(15);
                }
            }
        }
    }

    for (GameObject* newObj : newEntities) {
        m_entities.append(newObj);
    }
}

void Game::runCollision() {
    for (GameObject* obj : m_entities) {
        if (Cannonball* ball = dynamic_cast<Cannonball*>(obj)) {
            if (!ball->hasExploded()) {

                QRectF nextXBox = ball->boundingBox().translated(ball->getVelX(), 0);
                auto solidHitsX = m_collision->checkSolid(nextXBox, m_entities);
                if (!solidHitsX.isEmpty()) {
                    ball->setVelX(-ball->getVelX() * 0.8f);
                }

                QRectF nextYBox = ball->boundingBox().translated(0, ball->getVelY());
                auto solidHitsY = m_collision->checkSolid(nextYBox, m_entities);

                if (!solidHitsY.isEmpty()) {
                    ball->onGroundContact();

                    if (std::abs(ball->getVelY()) < 2.5f) {
                        ball->setVelY(0.0f);

                        ball->setVelX(ball->getVelX() * 0.90f);
                    } else {
                        ball->setVelY(-ball->getVelY() * 0.5f);
                        ball->setVelX(ball->getVelX() * 0.95f);
                    }
                }
            }
        }
        if (Entity* entity = dynamic_cast<Entity*>(obj)) {
            auto solidHits = m_collision->checkSolid(entity->boundingBox(), m_entities);
            for (const CollisionResult& cr : solidHits) {
                resolveSolidCollision(entity, cr);
            }
        }
        else if (CollectibleItem* item = dynamic_cast<CollectibleItem*>(obj)) {
            auto solidHits = m_collision->checkSolid(item->boundingBox(), m_entities);
            for (const CollisionResult& cr : solidHits) {
                item->handleSolidCollision(cr);
            }
        }
    }

    if (!m_player) return;

    bool currentlyInTrap = false;
    if (m_currentLevelObj) {
        QRectF pBox = m_player->boundingBox();
        for (const QRectF& trapRect : m_currentLevelObj->getTrapRects()) {
            if (pBox.intersects(trapRect)) {
                currentlyInTrap = true;
                break;
            }
        }
    }
    m_player->setInTrap(currentlyInTrap);
    if (m_currentLevelObj && m_player->y() > m_currentLevelObj->worldHeight()) {
        m_player->takeDamage(999);
    }
    auto nearby = m_collision->checkProximity(m_player->boundingBox(), m_entities, Constants::INTERACT_RANGE);
    resolveProximity(nearby);
}

void Game::resolveSolidCollision(Entity* subject, const CollisionResult& cr) {
    if (!cr.object || !subject) return;
    QRectF solid = cr.object->boundingBox();

    if (cr.fromTop) {
        subject->setPosition(subject->x(), solid.top() - subject->height());
        subject->land();
    }
    else if (cr.fromBottom) {
        subject->setPosition(subject->x(), solid.bottom());
        subject->stopVertical();
    }
    else if (cr.fromLeft) {
        subject->setPosition(solid.left() - subject->width(), subject->y());
        subject->stopHorizontal();
    }
    else if (cr.fromRight) {
        subject->setPosition(solid.right(), subject->y());
        subject->stopHorizontal();
    }
}
void Game::resolveProximity(const QVector<GameObject*>& nearby) {
    m_nearestInteractable = nullptr;
    float minDistanceSq = 1e9f;

    for (GameObject* obj : nearby) {
        if (!obj) continue;

        if (obj->isCollectible()) {
            auto* item = static_cast<CollectibleItem*>(obj);
            if (!item->isCollected() && m_player->boundingBox().intersects(obj->boundingBox())) {
                m_inventory->addItem(item->item());
                m_score->addScore(Constants::SCORE_ITEM_FOUND);
                emit itemCollected(item->item().id);

                if (item->item().id == "sword") {
                    m_player->equipSword();
                }

                item->markCollected();
            }
            continue;
        }

        if (obj->isInteractable()) {

            float dx = m_player->centreX() - obj->centreX();
            float dy = m_player->centreY() - obj->centreY();
            float distSq = (dx * dx) + (dy * dy);

            if (distSq < minDistanceSq) {
                minDistanceSq = distSq;
                m_nearestInteractable = static_cast<InteractiveObject*>(obj);
            }
        }
    }

    if (m_nearestInteractable && m_input->wasJustPressed(GameAction::INTERACT)) {
        triggerInteraction(m_nearestInteractable);
    }
}

void Game::advanceCamera() {
    if (m_player) m_camera->follow(m_player->centreX(), m_player->centreY());
    m_camera->updateShake();
}

void Game::removeInactiveEntities() {
    auto it = std::remove_if(m_entities.begin(), m_entities.end(), [](GameObject* obj) {
        if (obj && !obj->isActive()) { delete obj; return true; }
        return false;
    });
    m_entities.erase(it, m_entities.end());
}

void Game::draw(QPainter& painter) {
    if (m_stateManager->isInMenu()) return;
    m_skyBg->draw(painter, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT, m_camera->offsetX());
    if (m_currentLevelObj) {
        m_currentLevelObj->drawBackLayer(painter, *m_camera);
        m_currentLevelObj->drawLevelLayer(painter, *m_camera);
        m_currentLevelObj->drawMiddleLayer(painter, *m_camera);
        if (getCurrentLevel() == 3) m_currentLevelObj->drawFrontLayer(painter, *m_camera);
        if (Level3* lvl3 = dynamic_cast<Level3*>(m_currentLevelObj.get()))
            lvl3->drawWindowLayer(painter, *m_camera);
    }
    QRectF view = m_camera->viewport();
    float camX = m_camera->offsetX();
    float camY = m_camera->offsetY();
    for (GameObject* obj : m_entities) {
        if (!obj || !obj->isActive()) continue;
        if (!view.intersects(obj->boundingBox())) continue;
        if (obj == m_player) continue;
        if (dynamic_cast<Enemy*>(obj)) continue;
        bool isCollisionBox = obj->id().startsWith("level_collision_") ||
                              obj->id() == "floor" ||
                              obj->id() == "plat1";

        if (isCollisionBox) {
            if (Constants::DRAW_COLLISION_BOXES)
                obj->draw(painter, camX, camY);
            continue;
        }

        obj->draw(painter, camX, camY);
    }

    for (GameObject* obj : m_entities) {
        Enemy* enemy = dynamic_cast<Enemy*>(obj);
        if (!enemy) continue;
        if (!enemy->isActive()) continue;
        if (!view.intersects(enemy->boundingBox())) continue;
        enemy->draw(painter, camX, camY);
        if (CollectibleItem* beltItem = enemy->getBeltItem()) {
            beltItem->draw(painter, camX, camY);
        }
        enemy->drawDialog(painter, camX, camY);
    }

    if (m_player && m_player->isActive() && view.intersects(m_player->boundingBox())) {
        if (m_doorFadeOpacity < 1.0f) {
            painter.save();
            painter.setOpacity(m_doorFadeOpacity);
            m_player->draw(painter, camX, camY);
            painter.restore();
        } else {
            m_player->draw(painter, camX, camY);
        }
    }

    if (m_currentLevelObj && getCurrentLevel() != 3) {
        m_currentLevelObj->drawFrontLayer(painter, *m_camera);
    }
    if (m_popupOpacity > 0.0f) {
        painter.save();
        painter.setOpacity(m_popupOpacity);

        if (m_lastPopupWasCaptainStar) {
            int targetW = 160 * Constants::UI_SCALE;
            int targetH = 64 * Constants::UI_SCALE;
            float sx = m_camera->toScreenX(m_lastPopupX) - (targetW / 2.0f) + 210;
            float sy = m_camera->toScreenY(m_lastPopupY) - targetH - 5;
            if (m_showStarError)
                m_captainStarErrorPopup->draw(painter, sx, sy, targetW, targetH);
            else
                m_captainStarPopup->draw(painter, sx, sy, targetW, targetH);

        } else if (m_lastPopupWasDoor) {

            int targetW = 70 * Constants::UI_SCALE;
            int targetH = 13 * Constants::UI_SCALE;
            float sx = m_camera->toScreenX(m_lastPopupX) - (targetW / 2.0f) + 5;
            float sy = m_camera->toScreenY(m_lastPopupY) - targetH - 5;
            m_interactPopup->draw(painter, sx, sy, targetW, targetH);

        } else if (m_lastPopupWasQuizNpc) {
            int targetW = 70 * Constants::UI_SCALE;
            int targetH = 13 * Constants::UI_SCALE;
            float sx = m_camera->toScreenX(m_lastPopupX) - (targetW / 2.0f) + 75;
            float sy = m_camera->toScreenY(m_lastPopupY) - targetH - 5;
            m_interactPopup->draw(painter, sx, sy, targetW, targetH);

        } else {
            int targetW = 70 * Constants::UI_SCALE;
            int targetH = 13 * Constants::UI_SCALE;
            float sx = m_camera->toScreenX(m_lastPopupX) - (targetW / 2.0f) + 70;
            float sy = m_camera->toScreenY(m_lastPopupY) + m_popupYOffset + 110;
            m_interactPopup->draw(painter, sx, sy, targetW, targetH);
        }

        painter.restore();
    }

    HUD::draw(painter, m_score.get(), m_inventory.get(), m_quest.get(), m_player, this);
}

void Game::handleKeyPress(int qtKey) {
    if ((m_stateManager->isPlaying() && m_player)
        && (qtKey == Qt::Key_X || qtKey == Qt::Key_F)) {
        if (m_player->attack()) m_camera->addShake(2, 8.0f);
    }
    m_input->keyPressEvent(qtKey);
}

void Game::handleKeyRelease(int qtKey) { m_input->keyReleaseEvent(qtKey); }
void Game::spawnEntities(const LevelData& data) {
    float scale = 1.0f;
    float yOffset = 0.0f;

    m_currentLevelObj = LevelLoader::createLevelInstance(m_currentLevel, this);

    if (m_currentLevelObj) {
        m_currentLevelObj->init();
        m_camera->setWorldBounds(m_currentLevelObj->worldWidth(), m_currentLevelObj->worldHeight());
        scale = Constants::UI_SCALE;
        yOffset = m_currentLevelObj->verticalOffset();

        int rectIndex = 0;
        for (const QRectF& rect : m_currentLevelObj->getCollisionRects()) {
            auto* p = new StaticPlatform(rect.x(), rect.y(), rect.width(), rect.height());
            p->setId(QString("level_collision_%1").arg(rectIndex++));
            m_entities.append(p);
        }
    } else {
        m_camera->setWorldBounds(data.worldWidth, data.worldHeight);
    }

    if (!m_player) {
        float pStartX = data.playerStartX * scale;
        float pStartY = data.playerStartY * scale + yOffset;

        m_player = new Player(pStartX, pStartY, m_input.get());
        m_entities.append(m_player);
        m_camera->snapTo(m_player->centreX(), m_player->centreY());
    }

    QMap<QString, Item> itemLib;
    for (const Item& item : data.itemLibrary)
        itemLib[item.id] = item;

    QVector<QRectF> environmentRects;
    if (m_currentLevelObj) {
        environmentRects = m_currentLevelObj->getCollisionRects();
    }

    for (const EntitySpawnData& e : data.entities) {
        if (e.type == "platform") {
            environmentRects.append(QRectF(e.x * scale, e.y * scale + yOffset, e.w * scale, e.h * scale));
        }
    }

    for (const EntitySpawnData& e : data.entities) {
        GameObject* obj = nullptr;
        float eX = e.x * scale;
        float eY = e.y * scale + yOffset;
        float eW = e.w * scale;
        float eH = e.h * scale;

        if (e.type == "platform") {
            obj = new StaticPlatform(eX, eY, eW, eH);
        }
        else if (e.type == "enemy") {
            QString enemyType = e.properties.value("enemyType").toString();
            if (enemyType == "fiercetooth") {
                auto* enemy = new FierceTooth(eX, eY, m_player);

                enemy->setEnvironment(environmentRects);
                QString heldId = e.properties.value("heldItemId").toString();
                if (!heldId.isEmpty()) {
                    Item heldItem = itemLib.value(heldId, Item::null());
                    if (!heldItem.isNull()) {
                        enemy->setHeldItem(heldItem);
                    }
                }

                obj = enemy;
            }
            else if (enemyType == "cannon") {
                float maxSpeed = e.properties.value("maxSpeed", 10.0f).toFloat();
                float shotRange = e.properties.value("shotRange", 1000.0f).toFloat();
                auto* cannon = new Cannon(eX, eY, m_player, maxSpeed, shotRange);
                bool hasTarget = e.properties.value("hasTarget", false).toBool();
                if (hasTarget) {
                    float tx = e.properties.value("targetX", 0.0f).toFloat();
                    float ty = e.properties.value("targetY", 0.0f).toFloat();
                    cannon->setTrajectoryTarget(tx * scale, ty * scale + yOffset);
                }

                obj = cannon;
            }
        }
        else if (e.type == "collectible") {
            QString itemId = e.properties.value("itemId").toString();
            Item item = itemLib.value(itemId, Item::null());
            if (!item.isNull()) obj = new CollectibleItem(eX, eY, item);
        }
        else if (e.type == "container") {
            QString cType = e.properties.value("containerType", "chest").toString();
            auto* c = new Container(eX, eY, eW, eH, cType);

            bool locked = e.properties.value("locked", false).toBool();
            if (locked) {
                QString pType = e.properties.value("puzzleType").toString();
                QStringList pData = e.properties.value("puzzleData").toStringList();
                c->setLocked(pType, pData);
            }

            QStringList contentIds = e.properties.value("contents").toStringList();
            for (const QString& id : contentIds) {
                Item item = itemLib.value(id, Item::null());
                if (!item.isNull()) c->addItem(item);
            }
            obj = c;
        }
        else if (e.type == "npc_star") {
            obj = new CaptainStar(eX, eY);
        }
        else if (e.type == "quiz_npc") {
            auto* quizNpc = new QuizNpc(eX, eY);

            const QString rewardItemId = e.properties.value("rewardItemId").toString();
            Item rewardItem = itemLib.value(rewardItemId, Item::null());
            if (!rewardItem.isNull()) {
                quizNpc->addRewardItem(rewardItem);
            }

            const QString quizType = e.properties.value("quizType", "quiz").toString();
            const QStringList quizData = e.properties.value("quizData").toStringList();
            quizNpc->setLocked(quizType, quizData);
            obj = quizNpc;
        }
        else if (e.type == "hidden_area") {
            int triggers = e.properties.value("requiredTriggers", Constants::DEFAULT_TRIGGER_COUNT).toInt();
            auto* ha = new HiddenArea(eX, eY, eW, eH, triggers);
            QStringList hiddenIds = e.properties.value("itemIds").toStringList();
            for (const QString& id : hiddenIds) {
                Item item = itemLib.value(id, Item::null());
                if (!item.isNull()) ha->addHiddenItem(item);
            }
            obj = ha;
        }
        else if (e.type == "door") {
            int fadeMs = e.properties.value("fadeTimeMs", 500).toInt();
            auto* door = new Door(eX, eY, eW, eH, fadeMs);
            door->setGroupId(e.properties.value("groupId").toString());
            obj = door;
        }

        if (obj) {
            obj->setId(e.id);
            if (e.type == "player_start") continue;
            m_entities.append(obj);
        }
    }
    QMap<QString, QVector<Door*>> doorGroups;
    for (GameObject* obj : m_entities) {
        if (Door* d = dynamic_cast<Door*>(obj)) {
            doorGroups[d->groupId()].append(d);
        }
    }
    for (auto& group : doorGroups) {
        if (group.size() == 2) {
            group[0]->setPartner(group[1]);
            group[1]->setPartner(group[0]);
        }
    }
}
void Game::clearLevel() {
    for (GameObject* obj : m_entities) delete obj;
    m_entities.clear();
    m_player = nullptr;
    m_nearestInteractable = nullptr;
    m_currentLevelObj.reset();
}

void Game::spawnItemInWorld(const Item& item, float x, float y) {
    auto* collectible = new CollectibleItem(x, y, item);
    m_entities.append(collectible);
}

void Game::triggerInteraction(InteractiveObject* obj) {
    if (!obj) return;

    if (Door* door = dynamic_cast<Door*>(obj)) {
        if (!door->isInteractable()) return;
        door->interact();
        m_activeDoor      = door;
        m_doorPhase       = DoorPhase::WaitingForOpenAnim;
        m_doorFadeTimeMs  = door->fadeTimeMs();
        m_doorFadeElapsed = 0;
        m_doorFadeOpacity = 1.0f;
        return;
    }

    if (auto* captain = dynamic_cast<CaptainStar*>(obj)) {
        captain->interact();

        if (m_allTargetsFound) {
            m_showStarError = false;
            QTimer::singleShot(1000, this, [this]() {
                QSettings settings;
                int maxUnlocked = settings.value("max_unlocked_level", 0).toInt();
                int nextLevel = m_currentLevel + 1;
                if (nextLevel > maxUnlocked) {
                    settings.setValue("max_unlocked_level", nextLevel);
                }
                m_score->stopTimer();
                m_score->addScore(m_score->timeLeft() * Constants::SCORE_TIME_BONUS);
                m_stateManager->setState(GameState::WIN);
            });
        } else {
            m_showStarError = true;
            QTimer::singleShot(4000, this, [this]() {
                m_showStarError = false;
            });
        }
        return;
    }

    InteractionResult result = obj->interact();
    processInteractionResult(result, obj);
}

void Game::processInteractionResult(const InteractionResult& result, InteractiveObject* obj) {
    switch (result.type) {
    case InteractionResult::Type::ShowPuzzle:
        emit puzzleRequired(result.puzzle);
        break;

    case InteractionResult::Type::RevealItems: {
        float spawnX = obj->x() + (obj->width() / 2.0f) - 18.0f;
        float spawnY = obj->y() + (obj->height() / 2.0f);

        QVector<Item> itemsToSpawn = result.revealedItems;

        QTimer::singleShot(100, this, [this, itemsToSpawn, spawnX, spawnY, obj, result]() {
            float currentX = spawnX;

            for (const Item& item : result.revealedItems) {
                auto* collectible = new CollectibleItem(obj->centreX() - 18, obj->centreY() - 18, item);
                collectible->popOut(obj->centreX(), obj->centreY()+50);
                m_entities.append(collectible);
            }
        });
        break;
    }
    case InteractionResult::Type::RevealArea: {
        float spawnX = obj->x();
        for (const Item& item : result.revealedItems) {
            spawnItemInWorld(item, spawnX, obj->y());
            spawnX += 60;
        }
        break;
    }
    case InteractionResult::Type::ShowMessage:
        emit messageReady(result.message);
        break;
    case InteractionResult::Type::None:
    default:
        break;
    }
}

GameObject* Game::findById(const QString& id) {
    for (GameObject* obj : m_entities)
        if (obj && obj->id() == id) return obj;
    return nullptr;
}

InteractiveObject* Game::findInteractableById(const QString& id) {
    GameObject* obj = findById(id);
    if (obj && obj->isInteractable())
        return static_cast<InteractiveObject*>(obj);
    return nullptr;
}
