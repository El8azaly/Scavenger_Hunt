#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include "core/Game.h"
#include "core/GameLoop.h"

class GameWidget;
class MainMenuScreen;
class PauseScreen;
class GameOverScreen;
class WinScreen;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
private slots:
    void onStateChanged(GameState oldState, GameState newState);
    void onGameTick(int deltaTimeMs);
private:
    Game*            m_game;
    GameLoop*        m_gameLoop;
    QStackedWidget*  m_stack;
    GameWidget*      m_gameWidget;
    MainMenuScreen*  m_menuScreen;
    PauseScreen*     m_pauseScreen;
    GameOverScreen*  m_gameOverScreen;
    WinScreen*       m_winScreen;
    void buildUI();
    void connectSignals();
};
