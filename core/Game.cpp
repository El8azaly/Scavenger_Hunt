#include "Game.h"

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

// ── UI HUD header (draw only, no widgets) ────────────────────
#include "ui/HUD.h"

#include "core/Constants.h"

#include <QPainter>
#include <QDebug>
#include <algorithm>

// ─────────────────────────────────────────────────────────────
//  Constructor / Destructor
// ─────────────────────────────────────────────────────────────
Game::Game(QObject* parent)
    : QObject(parent)
{
    // Instantiate all subsystems
    m_stateManager = std::make_unique<GameStateManager>(this);
    m_input        = std::make_unique<InputHandler>(this);
    m_camera       = std::make_unique<Camera>();
    m_collision    = std::make_unique<CollisionEngine>();
    m_inventory    = std::make_unique<InventorySystem>(this);
    m_crafting     = std::make_unique<CraftingSystem>();
    m_quest        = std::make_unique<QuestSystem>(m_inventory.get(), this);
    m_score        = std::make_unique<ScoreManager>(this);

    // Propagate state changes upward so MainWindow can react
    connect(m_stateManager.get(), &GameStateManager::stateChanged,
            this,                 &Game::stateChanged);

    // When QuestSystem reports all targets found → trigger WIN
    connect(m_quest.get(), &QuestSystem::allTargetsFound,
            this, [this]() {
                m_score->stopTimer();
                // Award time bonus
                m_score->addScore(m_score->timeLeft() * Constants::SCORE_TIME_BONUS);
                m_stateManager->setState(GameState::WIN);
            });

    // When ScoreManager reports time's up → trigger GAME OVER
    connect(m_score.get(), &ScoreManager::timeUp,
            this, [this]() {
                m_stateManager->setState(GameState::GAME_OVER);
            });

    // Relay score changes upward for HUD
    connect(m_score.get(), &ScoreManager::scoreChanged,
            this, &Game::scoreChanged);
}

Game::~Game()
{
    clearLevel();
}

// ─────────────────────────────────────────────────────────────
//  Public control API  (called by UI buttons)
// ─────────────────────────────────────────────────────────────
void Game::startNewGame(int levelNumber)
{
    m_currentLevel = levelNumber;
    clearLevel();
    loadGame(); // reuse loadGame to also reset all systems, then:

    LevelData data = LevelLoader::load(m_currentLevel);

    m_camera->setWorldBounds(data.worldWidth, data.worldHeight);
    m_score->reset();
    m_score->startTimer(data.timeLimitSec);
    m_inventory->clear();
    m_crafting->loadRecipes(data.recipes);
    m_quest->setTargets(data.targetIds);

    spawnEntities(data);

    m_stateManager->setState(GameState::PLAYING);
}

void Game::restartLevel()
{
    startNewGame(m_currentLevel);
}

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
        m_score->startTimer(m_score->timeLeft()); // resume from where we left off
        m_stateManager->setState(GameState::PLAYING);
    }
}

void Game::exitToMenu()
{
    m_score->stopTimer();
    clearLevel();
    m_stateManager->setState(GameState::MAIN_MENU);
}

void Game::saveGame()
{
    SaveManager::save(this, Constants::SAVES_DIR + "slot1.json");
}

void Game::loadGame()
{
    // SaveManager restores entity state, inventory, score, level number
    // via Game's public setters. If no save file exists, this is a no-op.
    SaveManager::load(this, Constants::SAVES_DIR + "slot1.json");
}

// ─────────────────────────────────────────────────────────────
//  Puzzle resolution  (called by GameWidget after dialog closes)
// ─────────────────────────────────────────────────────────────
void Game::onPuzzleSolved(const QString& objectId)
{
    InteractiveObject* obj = findInteractableById(objectId);
    if (!obj) return;

    obj->unlock(); // marks the object as unlocked

    // Trigger the interaction again now that it's unlocked — this time
    // it returns RevealItems or RevealArea instead of ShowPuzzle
    InteractionResult result = obj->interact();
    processInteractionResult(result, obj);

    m_score->addScore(Constants::SCORE_PUZZLE_SOLVED);
}

void Game::onPuzzleFailed(const QString& objectId)
{
    // Currently: no penalty. Teams can add a time deduction here.
    Q_UNUSED(objectId)
    qDebug() << "[Game] Puzzle failed for" << objectId;
}

// ─────────────────────────────────────────────────────────────
//  Accessors
// ─────────────────────────────────────────────────────────────
GameState        Game::currentState() const { return m_stateManager->current(); }
InventorySystem* Game::inventory()    const { return m_inventory.get(); }
CraftingSystem*  Game::crafting()     const { return m_crafting.get(); }
QuestSystem*     Game::quest()        const { return m_quest.get(); }
ScoreManager*    Game::score()        const { return m_score.get(); }

// ─────────────────────────────────────────────────────────────
//  Per-frame update  (called by GameWidget via GameLoop::tick)
// ─────────────────────────────────────────────────────────────
void Game::update()
{
    if (!m_stateManager->isPlaying()) return;

    processInput();     // 1. read held keys → move player
    updateEntities();   // 2. advance physics for all entities
    runCollision();     // 3. detect & resolve collisions
    advanceCamera();    // 4. follow player with camera
    removeInactiveEntities(); // 5. purge dead/collected objects

    m_input->endFrame(); // 6. clear "just pressed" flags for next frame
}

// ── 1. Input processing ───────────────────────────────────────
void Game::processInput()
{
    if (!m_player) return;

    // Movement is applied every frame while key is held
    float velX = 0;
    if (m_input->isHeld(GameAction::MOVE_LEFT))  velX = -Constants::PLAYER_SPEED;
    if (m_input->isHeld(GameAction::MOVE_RIGHT)) velX =  Constants::PLAYER_SPEED;
    m_player->setVelocityX(velX);

    if (m_input->isHeld(GameAction::JUMP) && m_player->isOnGround())
        m_player->jump(Constants::JUMP_VELOCITY);

    // One-shot actions are handled by InputHandler's oneShotAction signal
    // which Game connected in the constructor (see connectOneShotActions below).
    // This keeps update() clean.
}

// ── 2. Entity update ──────────────────────────────────────────
void Game::updateEntities()
{
    for (GameObject* obj : m_entities) {
        if (obj && obj->isActive())
            obj->update();
    }
}

// ── 3. Collision ──────────────────────────────────────────────
void Game::runCollision()
{
    if (!m_player) return;

    QRectF playerRect = m_player->boundingBox();

    // 3a. Solid geometry — push player out of platforms/walls
    auto solidHits = m_collision->checkSolid(playerRect, m_entities);
    for (const CollisionResult& cr : solidHits)
        resolveSolidCollision(cr);

    // 3b. Proximity — find interactables / collectibles near player
    auto nearby = m_collision->checkProximity(playerRect, m_entities,
                                               Constants::INTERACT_RANGE);
    resolveProximity(nearby);
}

void Game::resolveSolidCollision(const CollisionResult& cr)
{
    if (!cr.object || !m_player) return;
    QRectF solid = cr.object->boundingBox();

    if (cr.fromTop) {
        // Player landed on top of something
        m_player->setPosition(m_player->x(), solid.top() - m_player->height());
        m_player->land(); // zeroes velY, sets m_onGround = true
    }
    else if (cr.fromBottom) {
        // Player hit ceiling
        m_player->setPosition(m_player->x(), solid.bottom());
        m_player->stopVertical();
    }
    else if (cr.fromLeft) {
        m_player->setPosition(solid.left() - m_player->width(), m_player->y());
        m_player->stopHorizontal();
    }
    else if (cr.fromRight) {
        m_player->setPosition(solid.right(), m_player->y());
        m_player->stopHorizontal();
    }
}

void Game::resolveProximity(const QVector<GameObject*>& nearby)
{
    m_nearestInteractable = nullptr;

    for (GameObject* obj : nearby) {
        if (!obj) continue;

        // Automatic pickup on direct overlap (no keypress needed)
        if (obj->isCollectible()) {
            auto* item = static_cast<CollectibleItem*>(obj);
            if (!item->isCollected() &&
                m_player->boundingBox().intersects(obj->boundingBox()))
            {
                m_inventory->addItem(item->item());
                m_score->addScore(Constants::SCORE_ITEM_FOUND);
                emit itemCollected(item->item().id);
                item->markCollected(); // sets active = false
            }
            continue;
        }

        // Interactable — show hint on nearest one
        if (obj->isInteractable()) {
            m_nearestInteractable = static_cast<InteractiveObject*>(obj);
        }
    }

    // If INTERACT was just pressed and there is a nearby interactable:
    // (INTERACT is a one-shot action handled via signal, but we also
    //  check wasJustPressed here as a backup)
    if (m_nearestInteractable && m_input->wasJustPressed(GameAction::INTERACT)) {
        triggerInteraction(m_nearestInteractable);
    }
}

// ── 4. Camera ─────────────────────────────────────────────────
void Game::advanceCamera()
{
    if (m_player)
        m_camera->follow(m_player->centreX(), m_player->centreY());
}

// ── 5. Cleanup ────────────────────────────────────────────────
void Game::removeInactiveEntities()
{
    auto it = std::remove_if(m_entities.begin(), m_entities.end(),
                              [](GameObject* obj) {
                                  if (obj && !obj->isActive()) {
                                      delete obj;
                                      return true;
                                  }
                                  return false;
                              });
    m_entities.erase(it, m_entities.end());
}

// ─────────────────────────────────────────────────────────────
//  Draw  (called by GameWidget::paintEvent)
// ─────────────────────────────────────────────────────────────
void Game::draw(QPainter& painter)
{
    if (m_stateManager->isInMenu()) return;

    QRectF view = m_camera->viewport();
    float  camX = m_camera->offsetX();
    float  camY = m_camera->offsetY();

    // Draw entities back-to-front (painter's algorithm)
    // Skip entities outside the camera viewport — big performance win
    for (GameObject* obj : m_entities) {
        if (!obj || !obj->isActive()) continue;
        if (!view.intersects(obj->boundingBox())) continue;
        obj->draw(painter, camX, camY);
    }

    // Draw interaction hint on the nearest interactable
    if (m_nearestInteractable) {
        float sx = m_camera->toScreenX(m_nearestInteractable->centreX());
        float sy = m_camera->toScreenY(m_nearestInteractable->y()) - 30;
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 12, QFont::Bold));
        painter.drawText(QPointF(sx - 5, sy), "[E]");
    }

    // HUD drawn last — fixed screen-space (no camera offset)
    HUD::draw(painter, m_score.get(), m_inventory.get(), m_quest.get());
}

// ─────────────────────────────────────────────────────────────
//  Input forwarding  (from GameWidget key events)
// ─────────────────────────────────────────────────────────────
void Game::handleKeyPress(int qtKey)
{
    m_input->keyPressEvent(qtKey);
}

void Game::handleKeyRelease(int qtKey)
{
    m_input->keyReleaseEvent(qtKey);
}

// ─────────────────────────────────────────────────────────────
//  Level loading / entity spawning
// ─────────────────────────────────────────────────────────────
void Game::spawnEntities(const LevelData& data)
{
    // Build a quick lookup for item data from the level's item library
    QMap<QString, Item> itemLib;
    for (const Item& item : data.itemLibrary)
        itemLib[item.id] = item;

    for (const EntitySpawnData& e : data.entities) {
        GameObject* obj = nullptr;

        if (e.type == "platform") {
            auto* p = new StaticPlatform(e.x, e.y, e.w, e.h);
            obj = p;
        }
        else if (e.type == "collectible") {
            QString itemId = e.properties.value("itemId").toString();
            Item item = itemLib.value(itemId, Item::null());
            if (!item.isNull()) {
                auto* c = new CollectibleItem(e.x, e.y, item);
                obj = c;
            }
        }
        else if (e.type == "container") {
            auto* c = new Container(e.x, e.y, e.w, e.h);

            bool locked = e.properties.value("locked", false).toBool();
            if (locked) {
                QString pType = e.properties.value("puzzleType").toString();
                QStringList pData = e.properties.value("puzzleData").toStringList();
                c->setLocked(pType, pData);
            }

            // Load contents from itemId list
            QStringList contentIds = e.properties.value("contents").toStringList();
            for (const QString& id : contentIds) {
                Item item = itemLib.value(id, Item::null());
                if (!item.isNull()) c->addItem(item);
            }
            obj = c;
        }
        else if (e.type == "hidden_area") {
            int triggers = e.properties.value("requiredTriggers",
                                Constants::DEFAULT_TRIGGER_COUNT).toInt();
            auto* ha = new HiddenArea(e.x, e.y, e.w, e.h, triggers);

            QStringList hiddenIds = e.properties.value("itemIds").toStringList();
            for (const QString& id : hiddenIds) {
                Item item = itemLib.value(id, Item::null());
                if (!item.isNull()) ha->addHiddenItem(item);
            }
            obj = ha;
        }

        if (obj) {
            obj->setId(e.id);

            // Special case: if this is the player start, create Player
            if (e.type == "player_start") {
                auto* player = new Player(e.x, e.y, m_input.get());
                m_player = player;
                m_entities.append(player);
                continue;
            }
            m_entities.append(obj);
        }
    }

    // If no player_start entity was in the JSON, place at level defaults
    if (!m_player) {
        m_player = new Player(data.playerStartX, data.playerStartY, m_input.get());
        m_entities.append(m_player);
    }
}

void Game::clearLevel()
{
    for (GameObject* obj : m_entities)
        delete obj;
    m_entities.clear();
    m_player = nullptr;
    m_nearestInteractable = nullptr;
}

void Game::spawnItemInWorld(const Item& item, float x, float y)
{
    // Called when a Container is opened — drops its items into the world
    auto* collectible = new CollectibleItem(x, y, item);
    m_entities.append(collectible);
}

// ─────────────────────────────────────────────────────────────
//  Interaction handling
// ─────────────────────────────────────────────────────────────
void Game::triggerInteraction(InteractiveObject* obj)
{
    if (!obj) return;
    InteractionResult result = obj->interact();
    processInteractionResult(result, obj);
}

void Game::processInteractionResult(const InteractionResult& result,
                                     InteractiveObject*       obj)
{
    switch (result.type) {

    case InteractionResult::Type::ShowPuzzle:
        // Tell the UI layer to display the appropriate puzzle dialog.
        // When the dialog closes, UI calls Game::onPuzzleSolved/Failed.
        emit puzzleRequired(result.puzzle);
        break;

    case InteractionResult::Type::RevealItems: {
        // Spawn each item in the world near the container position
        float spawnX = obj->x();
        for (const Item& item : result.revealedItems) {
            spawnItemInWorld(item, spawnX, obj->y() - 50);
            spawnX += 60; // spread items side by side
        }
        break;
    }

    case InteractionResult::Type::RevealArea: {
        // A HiddenArea has just revealed itself.
        // Its hidden items are now in result.revealedItems — spawn them.
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

// ─────────────────────────────────────────────────────────────
//  Internal helpers
// ─────────────────────────────────────────────────────────────
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
