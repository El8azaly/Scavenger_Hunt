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
#include "core/Level.h"
#include "entities/Entity.h"
#include "ui/sprite/SlicedSprite.h"
#include <QTimer>

#include "entities/Cannonball.h"
#include "entities/Door.h"

class SkyBackground;
class GameObject;
class Player;
class InteractiveObject;
class CollectibleItem;
class InventorySystem;
class QuestSystem;
class ScoreManager;
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
    void startNextLevel();
    void restartLevel();
    void pauseGame();
    void resumeGame();
    void exitToMenu();

    void onPuzzleSolved(const QString& objectId);
    void onPuzzleFailed(const QString& objectId);

    GameState        currentState()  const;
    InventorySystem* inventory()     const;
    QuestSystem* quest()         const;
    ScoreManager* score()         const;
    const Camera&    camera()        const { return *m_camera; }
    QString getWorldPosString(const QPoint& screenPos) const;
    int getCurrentLevel() const { return m_currentLevel; }
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
    std::unique_ptr<SlicedSprite> m_captainStarPopup;
    std::unique_ptr<GameStateManager>  m_stateManager;
    std::unique_ptr<InputHandler>      m_input;
    std::unique_ptr<Camera>            m_camera;
    std::unique_ptr<CollisionEngine>   m_collision;
    std::unique_ptr<InventorySystem>   m_inventory;
    std::unique_ptr<QuestSystem>       m_quest;
    std::unique_ptr<ScoreManager>      m_score;
    std::unique_ptr<SlicedSprite> m_interactPopup;
    float m_popupOpacity = 0.0f;
    float m_popupYOffset = -40.0f;
    float m_lastPopupX = 0.0f;
    float m_lastPopupY = 0.0f;
    bool m_lastPopupWasCaptainStar = false;
    bool m_allTargetsFound = false;
    bool m_showStarError = false;
    bool m_isDying = false;
    std::unique_ptr<SlicedSprite> m_captainStarErrorPopup;
    std::unique_ptr<Level> m_currentLevelObj;
    QVector<GameObject*> m_entities;
    enum class DoorPhase { None, WaitingForOpenAnim, FadingOut, FadingIn };
    DoorPhase m_doorPhase       = DoorPhase::None;
    Door*     m_activeDoor      = nullptr;
    float     m_doorFadeOpacity = 1.0f;
    int       m_doorFadeTimeMs  = 500;
    int       m_doorFadeElapsed = 0;
    bool      m_lastPopupWasDoor = false;
    Player* m_player = nullptr;
    int m_currentLevel = 0;
    InteractiveObject* m_nearestInteractable = nullptr;
    SkyBackground* m_skyBg;
    QTimer* m_animationTimer;
};