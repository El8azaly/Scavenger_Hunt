#pragma once
#include <QObject>

enum class GameState {
    MAIN_MENU,
    PLAYING,
    PAUSED,
    GAME_OVER,
    WIN
};

class GameStateManager : public QObject
{
    Q_OBJECT
public:
    explicit GameStateManager(QObject* parent = nullptr);

    GameState current()   const { return m_state; }
    bool isPlaying()      const { return m_state == GameState::PLAYING; }
    bool isPaused()       const { return m_state == GameState::PAUSED;  }
    bool isInMenu()       const { return m_state == GameState::MAIN_MENU; }

    void setState(GameState newState);

signals:

    void stateChanged(GameState oldState, GameState newState);

private:
    GameState m_state = GameState::MAIN_MENU;
};