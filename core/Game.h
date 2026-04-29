#pragma once
#include <QObject>
#include <QVector>
#include <memory>

#include "core/GameState.h"
#include "engine/InputHandler.h"
#include "engine/Camera.h"
#include "engine/CollisionEngine.h"
#include "entities/InteractionResult.h"
#include "data/LevelLoader.h"

// Forward declarations — avoids including every system/entity header here.
// Each system header is included only in Game.cpp.
class GameObject;
class Player;
class InteractiveObject;
class CollectibleItem;
class InventorySystem;
class CraftingSystem;
class QuestSystem;
class ScoreManager;
class QPainter;

// ─────────────────────────────────────────────────────────────
//  Game  —  the central controller of the entire game
//
//  Responsibilities:
//    1. Owns all subsystems (inventory, score, quest, …)
//    2. Owns all live entities (as raw pointers in m_entities)
//    3. Runs the per-frame update loop:
//         a. read input
//         b. update all entities
//         c. run collision detection
//         d. process collision results (physics + pickups + interactions)
//         e. advance camera
//    4. Runs the draw loop: draw all visible entities, then HUD
//    5. Responds to state transitions (start, pause, win, game-over)
//
//  What Game does NOT do:
//    • Render Qt widgets — that is MainWindow / GameWidget
//    • Define game rules inside entities — that belongs to each class
//    • Handle file I/O directly — delegated to SaveManager / LevelLoader
//
//  Signal contract with UI:
//    • stateChanged  → MainWindow switches active screen
//    • puzzleRequired → GameWidget shows the appropriate PuzzleDialog
//    • messageReady  → GameWidget shows a brief overlay text
//    • itemCollected → (convenience) HUD / quest display can connect
// ─────────────────────────────────────────────────────────────
class Game : public QObject
{
    Q_OBJECT
public:
    explicit Game(QObject* parent = nullptr);
    ~Game();

    // ── Per-frame API (called by GameWidget) ──────────────────
    void update();                        // advance game logic one frame
    void draw(QPainter& painter);         // paint world + HUD

    // ── Input forwarding (called by GameWidget key events) ────
    void handleKeyPress(int qtKey);
    void handleKeyRelease(int qtKey);

    // ── Control API (called by UI screen buttons) ─────────────
    void startNewGame(int levelNumber = 1);
    void restartLevel();
    void pauseGame();
    void resumeGame();
    void exitToMenu();
    void saveGame();
    void loadGame();

    // ── Puzzle resolution (called by GameWidget after dialog) ─
    // When a PuzzleDialog is accepted, GameWidget calls this
    // with the id of the Container/HiddenArea that requested the puzzle.
    void onPuzzleSolved(const QString& objectId);
    void onPuzzleFailed(const QString& objectId);

    // ── Read-only accessors for UI panels ─────────────────────
    GameState        currentState()  const;
    InventorySystem* inventory()     const;
    CraftingSystem*  crafting()      const;
    QuestSystem*     quest()         const;
    ScoreManager*    score()         const;
    const Camera&    camera()        const { return *m_camera; }

signals:
    // Emitted on every state transition — MainWindow connects this
    void stateChanged(GameState oldState, GameState newState);

    // Emitted when a Container/HiddenArea needs a puzzle shown.
    // GameWidget connects this and shows the correct PuzzleDialog subclass.
    void puzzleRequired(const PuzzleData& puzzle);

    // Emitted when a container opens and drops items or a clue appears
    void messageReady(const QString& text);

    // Convenience signals for HUD / quest tracker
    void itemCollected(const QString& itemId);
    void scoreChanged(int newScore);

private:
    // ── Frame sub-steps ───────────────────────────────────────
    void processInput();
    void updateEntities();
    void runCollision();
    void advanceCamera();
    void removeInactiveEntities();

    // ── Collision result handlers ─────────────────────────────
    void resolveSolidCollision(const CollisionResult& cr);
    void resolveProximity(const QVector<GameObject*>& nearbyObjects);

    // ── Entity management ─────────────────────────────────────
    void spawnEntities(const LevelData& data);
    void clearLevel();
    // Spawns a CollectibleItem in the world at (x, y) — used when
    // a Container is opened and reveals its contents
    void spawnItemInWorld(const Item& item, float x, float y);

    // ── Internal helpers ──────────────────────────────────────
    // Returns a raw pointer to an entity by its id, or nullptr
    GameObject*          findById(const QString& id);
    InteractiveObject*   findInteractableById(const QString& id);
    void triggerInteraction(InteractiveObject* obj);
    void processInteractionResult(const InteractionResult& result, InteractiveObject* obj);

    // ── Subsystems (Game owns these) ──────────────────────────
    std::unique_ptr<GameStateManager>  m_stateManager;
    std::unique_ptr<InputHandler>      m_input;
    std::unique_ptr<Camera>            m_camera;
    std::unique_ptr<CollisionEngine>   m_collision;
    std::unique_ptr<InventorySystem>   m_inventory;
    std::unique_ptr<CraftingSystem>    m_crafting;
    std::unique_ptr<QuestSystem>       m_quest;
    std::unique_ptr<ScoreManager>      m_score;

    // ── Entity storage ────────────────────────────────────────
    // Game owns all entities. They are stored as raw pointers
    // because multiple subsystems need to reference them without
    // ownership confusion. Game allocates them in spawnEntities()
    // and deletes them in clearLevel() or when m_entities is cleared.
    QVector<GameObject*> m_entities;

    // Non-owning convenience pointer to the player entity inside m_entities
    Player* m_player = nullptr;

    int m_currentLevel = 1;

    // Tracks which interactive object is currently nearest the player
    // (used to show/remove interaction hint icon each frame)
    InteractiveObject* m_nearestInteractable = nullptr;
};
