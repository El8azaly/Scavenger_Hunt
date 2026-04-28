#pragma once
#include <QObject>

// All possible states the game can be in.
// Game.cpp is the only class allowed to call setState().
enum class GameState {
    MAIN_MENU,   // No game is loaded; the start screen is visible
    PLAYING,     // Active gameplay; game loop is ticking
    PAUSED,      // Game loop frozen; pause overlay is visible
    GAME_OVER,   // Time ran out or player died; game over screen visible
    WIN          // All quest targets collected; win screen visible
};

// GameStateManager wraps the enum and ensures transitions are
// only made via setState() so the stateChanged signal always fires.
class GameStateManager : public QObject
{
    Q_OBJECT
public:
    explicit GameStateManager(QObject* parent = nullptr);

    GameState current()   const { return m_state; }
    bool isPlaying()      const { return m_state == GameState::PLAYING; }
    bool isPaused()       const { return m_state == GameState::PAUSED;  }
    bool isInMenu()       const { return m_state == GameState::MAIN_MENU; }

    // Validates transition, sets state, emits stateChanged.
    void setState(GameState newState);

signals:
    // Emitted every time the state changes.
    // MainWindow connects to this to switch active screens.
    void stateChanged(GameState oldState, GameState newState);

private:
    GameState m_state = GameState::MAIN_MENU;
};
