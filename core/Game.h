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
#include "core/Level.h" // Added Level base class
#include "entities/Entity.h"

class SkyBackground;
class GameObject;
class Player;
class InteractiveObject;
class CollectibleItem;
class InventorySystem;
class CraftingSystem;
class QuestSystem;
class ScoreManager;
#include <QTimer>
class QPainter;

class Game : public QObject
{
    Q_OBJECT
public:
    explicit Game(QObject* parent = nullptr);
    ~Game();

    void update(int deltaTimeMs);
    void draw(QPainter& painter);

    void handleKeyPress(int qtKey);
    void handleKeyRelease(int qtKey);
    void handleMousePress(int qtMouseButton);

    void startNewGame(int levelNumber = 1);
    void restartLevel();
    void pauseGame();
    void resumeGame();
    void exitToMenu();
    void saveGame();
    void loadGame();

    void onPuzzleSolved(const QString& objectId);
    void onPuzzleFailed(const QString& objectId);

    GameState        currentState()  const;
    InventorySystem* inventory()     const;
    CraftingSystem* crafting()      const;
    QuestSystem* quest()         const;
    ScoreManager* score()         const;
    const Camera&    camera()        const { return *m_camera; }

signals:
    void stateChanged(GameState oldState, GameState newState);
    void puzzleRequired(const PuzzleData& puzzle);
    void messageReady(const QString& text);
    void itemCollected(const QString& itemId);
    void scoreChanged(int newScore);

private:
    void processInput();
    void updateEntities();
    void runCollision();
    void advanceCamera();
    void removeInactiveEntities();

    void resolveSolidCollision(Entity* subject, const CollisionResult& cr);
    void resolveProximity(const QVector<GameObject*>& nearbyObjects);

    void spawnEntities(const LevelData& data);
    void clearLevel();
    void spawnItemInWorld(const Item& item, float x, float y);

    GameObject* findById(const QString& id);
    InteractiveObject* findInteractableById(const QString& id);
    void triggerInteraction(InteractiveObject* obj);
    void processInteractionResult(const InteractionResult& result, InteractiveObject* obj);

    std::unique_ptr<GameStateManager>  m_stateManager;
    std::unique_ptr<InputHandler>      m_input;
    std::unique_ptr<Camera>            m_camera;
    std::unique_ptr<CollisionEngine>   m_collision;
    std::unique_ptr<InventorySystem>   m_inventory;
    std::unique_ptr<CraftingSystem>    m_crafting;
    std::unique_ptr<QuestSystem>       m_quest;
    std::unique_ptr<ScoreManager>      m_score;

    std::unique_ptr<Level> m_currentLevelObj;

    QVector<GameObject*> m_entities;

    Player* m_player = nullptr;
    int m_currentLevel = 0;
    InteractiveObject* m_nearestInteractable = nullptr;

    SkyBackground* m_skyBg;
    QTimer* m_animationTimer;
};