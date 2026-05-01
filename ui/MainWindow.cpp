#include "ui/MainWindow.h"
#include "ui/GameWidget.h"
#include "UISpriteSheet.h"
#include "ui/MainMenuScreen.h"
#include "ui/PauseScreen.h"
#include "ui/GameOverScreen.h"
#include "ui/WinScreen.h"
#include "core/Constants.h"
#include <QVBoxLayout>


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setFixedSize(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT);
    m_game     = new Game(this);
    m_gameLoop = new GameLoop(this);
    buildUI();
    connectSignals();
    UISpriteSheet::init(":/assets/sprites/ui/ui_theme.png", ":/assets/sprites/ui/ui_theme.json");
}

void MainWindow::buildUI() {
    m_stack         = new QStackedWidget(this);
    m_menuScreen    = new MainMenuScreen(this);
    m_gameWidget    = new GameWidget(m_game, this);
    m_pauseScreen   = new PauseScreen(this);
    m_gameOverScreen= new GameOverScreen(this);
    m_winScreen     = new WinScreen(this);

    m_stack->addWidget(m_menuScreen);      // index 0
    m_stack->addWidget(m_gameWidget);      // index 1
    m_stack->addWidget(m_pauseScreen);     // index 2
    m_stack->addWidget(m_gameOverScreen);  // index 3
    m_stack->addWidget(m_winScreen);       // index 4
    m_stack->setCurrentIndex(0);
    setCentralWidget(m_stack);
}

void MainWindow::connectSignals() {
    // Game state → switch screen
    connect(m_game, &Game::stateChanged, this, &MainWindow::onStateChanged);
    // GameLoop tick → advance one frame
    connect(m_gameLoop, &GameLoop::tick, this, &MainWindow::onGameTick);

    // Menu buttons
    connect(m_menuScreen, &MainMenuScreen::startRequested,
            this, [this](){ m_game->startNewGame(1); m_gameLoop->start(); });

    // Pause buttons
    connect(m_pauseScreen, &PauseScreen::resumeRequested,
            this, [this](){ m_game->resumeGame(); m_gameLoop->start(); });
    connect(m_pauseScreen, &PauseScreen::restartRequested,
            this, [this](){ m_game->restartLevel(); m_gameLoop->start(); });
    connect(m_pauseScreen, &PauseScreen::exitRequested,
            this, [this](){ m_gameLoop->stop(); m_game->exitToMenu(); });

    // Game-over / win buttons
    connect(m_gameOverScreen, &GameOverScreen::restartRequested,
            this, [this](){ m_game->restartLevel(); m_gameLoop->start(); });
    connect(m_gameOverScreen, &GameOverScreen::exitRequested,
            this, [this](){ m_gameLoop->stop(); m_game->exitToMenu(); });
    connect(m_winScreen, &WinScreen::nextLevelRequested,
            this, [this](){ m_game->startNewGame(1); m_gameLoop->start(); }); // TODO next level
    connect(m_winScreen, &WinScreen::exitRequested,
            this, [this](){ m_gameLoop->stop(); m_game->exitToMenu(); });
}

void MainWindow::onGameTick() {
    m_game->update();
    m_gameWidget->update(); // triggers paintEvent
}

void MainWindow::onStateChanged(GameState /*old*/, GameState newState) {
    m_gameLoop->stop();
    switch(newState) {
        case GameState::MAIN_MENU: m_stack->setCurrentIndex(0); break;
        case GameState::PLAYING:   m_stack->setCurrentIndex(1); m_gameLoop->start(); break;
        case GameState::PAUSED:    m_stack->setCurrentIndex(2); break;
        case GameState::GAME_OVER: m_stack->setCurrentIndex(3); break;
        case GameState::WIN:       m_stack->setCurrentIndex(4); break;
    }
}
