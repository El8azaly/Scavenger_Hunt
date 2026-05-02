#include "core/Game.h"

// ── Entity headers ────────────────────────────────────────────
#include "entities/GameObject.h"
#include "entities/Entity.h"
#include "entities/Player.h"
#include "entities/InteractiveObject.h"
#include "entities/Container.h"
#include "entities/CollectibleItem.h"
#include "entities/HiddenArea.h"
#include "entities/StaticPlatform.h"

// ── System headers ────────────────────────────────────────────
#include "systems/InventorySystem.h"
#include "systems/CraftingSystem.h"
#include "systems/QuestSystem.h"
#include "systems/ScoreManager.h"

// ── Data headers ──────────────────────────────────────────────
#include "data/LevelLoader.h"
#include "data/SaveManager.h"

// ── UI / Visual headers ────────────────────────────────────────
#include "ui/HUD.h"
#include "core/Constants.h"
#include <QPainter>
#include <QDebug>
#include <algorithm>
#include <QMap>
#include "ui/SkyBackground.h"
#include "data/levels/Level0.h"
#include "entities/FierceTooth.h"
// ── Include specific levels ───────────────────────────────────

class SkyBackground;

Game::Game(QObject* parent)
    : QObject(parent)
{
    m_stateManager = std::make_unique<GameStateManager>(this);
    m_input        = std::make_unique<InputHandler>(this);
    m_camera       = std::make_unique<Camera>();
    m_collision    = std::make_unique<CollisionEngine>();
    m_inventory    = std::make_unique<InventorySystem>(this);
    m_crafting     = std::make_unique<CraftingSystem>();
    m_quest        = std::make_unique<QuestSystem>(m_inventory.get(), this);
    m_score        = std::make_unique<ScoreManager>(this);

    connect(m_stateManager.get(), &GameStateManager::stateChanged, this, &Game::stateChanged);
    connect(m_quest.get(), &QuestSystem::allTargetsFound, this, [this]() {
        m_score->stopTimer();
        m_score->addScore(m_score->timeLeft() * Constants::SCORE_TIME_BONUS);
        m_stateManager->setState(GameState::WIN);
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

Game::~Game()
{
    clearLevel();
}

void Game::startNewGame(int levelNumber)
{
    m_currentLevel = levelNumber;
    clearLevel();
    loadGame();

    LevelData data = LevelLoader::load(m_currentLevel);

    m_score->reset();
    m_score->startTimer(data.timeLimitSec);
    m_inventory->clear();
    m_crafting->loadRecipes(data.recipes);
    m_quest->setTargets(data.targetIds);

    // Bounding occurs inside spawnEntities depending on LevelObj
    spawnEntities(data);

    m_stateManager->setState(GameState::PLAYING);
}

void Game::restartLevel() { startNewGame(m_currentLevel); }

void Game::pauseGame()
{
    if (m_stateManager->isPlaying()) {
        m_score->stopTimer();
        m_stateManager->setState(GameState::PAUSED);
    }
}

void Game::resumeGame()
{
    if (m_stateManager->isPaused()) {
        m_score->startTimer(m_score->timeLeft());
        m_stateManager->setState(GameState::PLAYING);
    }
}

void Game::exitToMenu()
{
    m_score->stopTimer();
    clearLevel();
    m_stateManager->setState(GameState::MAIN_MENU);
}

void Game::saveGame() { SaveManager::save(this, Constants::SAVES_DIR + "slot1.json"); }
void Game::loadGame() { SaveManager::load(this, Constants::SAVES_DIR + "slot1.json"); }

void Game::onPuzzleSolved(const QString& objectId)
{
    InteractiveObject* obj = findInteractableById(objectId);
    if (!obj) return;
    obj->unlock();
    InteractionResult result = obj->interact();
    processInteractionResult(result, obj);
    m_score->addScore(Constants::SCORE_PUZZLE_SOLVED);
}

void Game::onPuzzleFailed(const QString& objectId) { Q_UNUSED(objectId) }

GameState        Game::currentState() const { return m_stateManager->current(); }
InventorySystem* Game::inventory()    const { return m_inventory.get(); }
CraftingSystem* Game::crafting()     const { return m_crafting.get(); }
QuestSystem* Game::quest()        const { return m_quest.get(); }
ScoreManager* Game::score()        const { return m_score.get(); }

void Game::update() {
    if (!m_stateManager->isPlaying()) return;

    processInput();
    updateEntities();
    runCollision();

    if (m_player) m_player->updateAnimation();
    if (m_currentLevelObj) m_currentLevelObj->update();

    advanceCamera();
    removeInactiveEntities();
    m_input->endFrame();
}

void Game::processInput()
{
    if (!m_player) return;
    float velX = 0;
    if (m_input->isHeld(GameAction::MOVE_LEFT))  velX = -Constants::PLAYER_SPEED;
    if (m_input->isHeld(GameAction::MOVE_RIGHT)) velX =  Constants::PLAYER_SPEED;
    m_player->setVelocityX(velX);

    if (m_input->isHeld(GameAction::JUMP) && m_player->isOnGround())
        m_player->jump(Constants::JUMP_VELOCITY);
}

// Add the new mouse handling method
void Game::handleMousePress(int qtMouseButton) {
    if (m_stateManager->isPlaying() && m_player && qtMouseButton == Qt::LeftButton) {
        m_player->attack();
    }
}



void Game::updateEntities()
{
    // Change hit frame to 23 (2 frames after button press) so it feels instantaneous
    bool playerJustHit = (m_player && m_player->getAttackTimer() == 23);

    for (GameObject* obj : m_entities) {
        if (!obj || !obj->isActive()) continue;

        obj->update();

        if (playerJustHit) {
            // Replaced FierceTooth cast with AttackableEntity
            if (AttackableEntity* attackable = dynamic_cast<AttackableEntity*>(obj)) {
                float dist = std::abs(m_player->centreX() - attackable->centreX());
                float verticalDist = std::abs(m_player->y() - attackable->y());

                bool facingEnemy = (m_player->isFacingRight() && attackable->x() > m_player->x()) ||
                                   (!m_player->isFacingRight() && attackable->x() < m_player->x());

                if (dist < 105.0f && verticalDist < 60.0f && facingEnemy) {
                    attackable->takeDamage(15);
                }
            }
        }
    }
}
void Game::runCollision()
{
    for (GameObject* obj : m_entities) {
        if (Entity* entity = dynamic_cast<Entity*>(obj)) {
            auto solidHits = m_collision->checkSolid(entity->boundingBox(), m_entities);
            for (const CollisionResult& cr : solidHits) {
                resolveSolidCollision(entity, cr);
            }
        }
    }

    if (!m_player) return;
    auto nearby = m_collision->checkProximity(m_player->boundingBox(), m_entities, Constants::INTERACT_RANGE);
    resolveProximity(nearby);
}

void Game::resolveSolidCollision(Entity* subject, const CollisionResult& cr)
{
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
void Game::resolveProximity(const QVector<GameObject*>& nearby)
{
    m_nearestInteractable = nullptr;
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

        if (obj->isInteractable()) m_nearestInteractable = static_cast<InteractiveObject*>(obj);
    }

    if (m_nearestInteractable && m_input->wasJustPressed(GameAction::INTERACT))
        triggerInteraction(m_nearestInteractable);
}

void Game::advanceCamera()
{
    if (m_player) m_camera->follow(m_player->centreX(), m_player->centreY());
}

void Game::removeInactiveEntities()
{
    auto it = std::remove_if(m_entities.begin(), m_entities.end(), [](GameObject* obj) {
        if (obj && !obj->isActive()) { delete obj; return true; }
        return false;
    });
    m_entities.erase(it, m_entities.end());
}

void Game::draw(QPainter& painter)
{
    if (m_stateManager->isInMenu()) return;
    m_skyBg->draw(painter, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT);

    // ── 1. Draw Background Level Layers ───────────────────────
    if (m_currentLevelObj) {
        m_currentLevelObj->drawBackLayer(painter, *m_camera);
        m_currentLevelObj->drawLevelLayer(painter, *m_camera);
        m_currentLevelObj->drawMiddleLayer(painter, *m_camera);
    }

    // ── 2. Draw Entities ───────────────────────────────────────
    QRectF view = m_camera->viewport();
    float  camX = m_camera->offsetX();
    float  camY = m_camera->offsetY();

    for (GameObject* obj : m_entities) {
        if (!obj || !obj->isActive()) continue;
        if (!view.intersects(obj->boundingBox())) continue;

        // Identify if the object is an invisible collision box
        bool isCollisionBox = obj->id().startsWith("level_collision_") ||
                              obj->id() == "floor" ||
                              obj->id() == "plat1";

// If the debug macro is NOT defined, skip drawing the collision box completely
#ifndef DEBUG_DRAW_COLLISIONS
        if (isCollisionBox) {
            continue;
        }
#else
// If the debug macro IS defined, draw it normally and continue
        if (isCollisionBox) {
            obj->draw(painter, camX, camY);
            continue;
        }
#endif

        // Draw normal visible game objects (player, items, chests, etc.)
        obj->draw(painter, camX, camY);
    }

    if (m_nearestInteractable) {
        float sx = m_camera->toScreenX(m_nearestInteractable->centreX());
        float sy = m_camera->toScreenY(m_nearestInteractable->y()) - 30;
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 12, QFont::Bold));
        painter.drawText(QPointF(sx - 5, sy), "[E]");
    }

    // ── 3. Draw Foreground Level Layer ────────────────────────
    if (m_currentLevelObj) {
        m_currentLevelObj->drawFrontLayer(painter, *m_camera);
    }

    HUD::draw(painter, m_score.get(), m_inventory.get(), m_quest.get());
}

void Game::handleKeyPress(int qtKey) { m_input->keyPressEvent(qtKey); }
void Game::handleKeyRelease(int qtKey) { m_input->keyReleaseEvent(qtKey); }
void Game::spawnEntities(const LevelData& data)
{
    float scale = 1.0f;
    float yOffset = 0.0f;

    if (m_currentLevel == 0) {
        m_currentLevelObj = std::make_unique<Level0>(this);
    }

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

                // --- STEP 2: PASS ENVIRONMENT TO ENEMY AI ---
                enemy->setEnvironment(environmentRects);
                obj = enemy;
            }
        }
        else if (e.type == "collectible") {
            QString itemId = e.properties.value("itemId").toString();
            Item item = itemLib.value(itemId, Item::null());
            if (!item.isNull()) obj = new CollectibleItem(eX, eY, item);
        }
        else if (e.type == "container") {
            auto* c = new Container(eX, eY, eW, eH);
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

        if (obj) {
            obj->setId(e.id);
            if (e.type == "player_start") continue;
            m_entities.append(obj);
        }
    }
}
void Game::clearLevel()
{
    for (GameObject* obj : m_entities) delete obj;
    m_entities.clear();
    m_player = nullptr;
    m_nearestInteractable = nullptr;
    m_currentLevelObj.reset(); // Destroy the level instance
}

void Game::spawnItemInWorld(const Item& item, float x, float y)
{
    auto* collectible = new CollectibleItem(x, y, item);
    m_entities.append(collectible);
}

void Game::triggerInteraction(InteractiveObject* obj)
{
    if (!obj) return;
    InteractionResult result = obj->interact();
    processInteractionResult(result, obj);
}

void Game::processInteractionResult(const InteractionResult& result, InteractiveObject* obj)
{
    switch (result.type) {
    case InteractionResult::Type::ShowPuzzle:
        emit puzzleRequired(result.puzzle);
        break;
    case InteractionResult::Type::RevealItems: {
        float spawnX = obj->x();
        for (const Item& item : result.revealedItems) {
            spawnItemInWorld(item, spawnX, obj->y() - 50);
            spawnX += 60;
        }
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

GameObject* Game::findById(const QString& id)
{
    for (GameObject* obj : m_entities)
        if (obj && obj->id() == id) return obj;
    return nullptr;
}

InteractiveObject* Game::findInteractableById(const QString& id)
{
    GameObject* obj = findById(id);
    if (obj && obj->isInteractable())
        return static_cast<InteractiveObject*>(obj);
    return nullptr;
}