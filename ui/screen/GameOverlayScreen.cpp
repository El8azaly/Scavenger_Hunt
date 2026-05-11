#include "GameOverlayScreen.h"
#include "ui/sprite/SpriteButton.h"
#include "core/Constants.h"
#include "core/Game.h"
#include "systems/ScoreManager.h"
#include <QPainter>
#include <QKeyEvent>

constexpr auto ui = Constants::UI_SCALE;

GameOverlayScreen::GameOverlayScreen(Game* game, Type type, QWidget* p)
    : QWidget(p),
    m_game(game),
    m_type(type),
    m_panel("game_over_screen_1x1"),
    m_titleLabel("paused_label_1x1") {
    setFocusPolicy(Qt::StrongFocus);
    setupUI();
}

void GameOverlayScreen::setupUI() {
    int panelH = 91 * ui;
    int panelY = (Constants::WINDOW_HEIGHT - panelH) / 2;
    switch (m_type) {
    case Type::Pause:
        m_titleLabel = SlicedSprite("paused_label_1x1");
        break;
    case Type::GameOver:
        m_titleLabel = SlicedSprite("game_over_label_1x1");
        break;
    case Type::Victory:
        m_titleLabel = SlicedSprite("victory_label_1x1");
        break;
    }

    switch (m_type) {
    case Type::Pause: {
        auto* restartBtn = SpriteButton::createYellow("", this);
        restartBtn->setIcon("restart_icon_1x1", 1);
        restartBtn->setTextOffset(0, -1);
        restartBtn->setFixedSize(16 * ui, 16 * ui);
        restartBtn->move((Constants::WINDOW_WIDTH - 16 * ui) / 2 - 26 * ui, panelY + 67 * ui);
        connect(restartBtn, &SpriteButton::clicked, this, &GameOverlayScreen::restartRequested);

        auto* resumeBtn = SpriteButton::createYellow("", this);
        resumeBtn->setIcon("play_icon_1x1", 1);
        resumeBtn->setTextOffset(0, -1);
        resumeBtn->setFixedSize(34 * ui, 16 * ui);
        resumeBtn->move((Constants::WINDOW_WIDTH - 16 * ui) / 2 - 10 * ui, panelY + 67 * ui);
        connect(resumeBtn, &SpriteButton::clicked, this, &GameOverlayScreen::resumeRequested);
        break;
    }
    case Type::GameOver: {
        auto* restartBtn = SpriteButton::createYellow("", this);
        restartBtn->setIcon("restart_icon_1x1", 1);
        restartBtn->setTextOffset(0, -1);
        restartBtn->setFixedSize(34 * ui, 16 * ui);
        restartBtn->move((Constants::WINDOW_WIDTH - 16 * ui) / 2 - 10 * ui, panelY + 67 * ui);
        connect(restartBtn, &SpriteButton::clicked, this, &GameOverlayScreen::restartRequested);
        break;
    }
    case Type::Victory: {
        bool hasNextLevel = false;
        if (m_game) {
            int nextLevel = m_game->getCurrentLevel() + 1;
            hasNextLevel = LevelLoader::getRegisteredLevels().contains(nextLevel);
        }
        auto* restartBtn = SpriteButton::createYellow("", this);
        restartBtn->setIcon("restart_icon_1x1", 1);
        restartBtn->setTextOffset(0, -1);
        connect(restartBtn, &SpriteButton::clicked, this, &GameOverlayScreen::restartRequested);
        m_nextLevelBtn = SpriteButton::createYellow("", this);
        m_nextLevelBtn->setIcon("resume_icon_1x1", 1);
        m_nextLevelBtn->setTextOffset(0, -1);
        connect(m_nextLevelBtn, &SpriteButton::clicked, this, [this]() {
            if (m_game) emit nextLevelRequested(m_game->getCurrentLevel() + 1);
        });
        if (hasNextLevel) {
            restartBtn->setFixedSize(16 * ui, 16 * ui);
            restartBtn->move((Constants::WINDOW_WIDTH - 16 * ui) / 2 - 26 * ui, panelY + 67 * ui);

            m_nextLevelBtn->setFixedSize(34 * ui, 16 * ui);
            m_nextLevelBtn->move((Constants::WINDOW_WIDTH - 16 * ui) / 2 - 10 * ui, panelY + 67 * ui);
            m_nextLevelBtn->show();
        } else {
            restartBtn->setFixedSize(34 * ui, 16 * ui);
            restartBtn->move((Constants::WINDOW_WIDTH - 16 * ui) / 2 - 10 * ui, panelY + 67 * ui);
            m_nextLevelBtn->hide();
        }
        auto* exitBtn = SpriteButton::createYellow("", this);
        exitBtn->setIcon("home_icon_1x1", 1);
        exitBtn->setTextOffset(0, -1);
        exitBtn->setFixedSize(16 * ui, 16 * ui);
        int exitX = (Constants::WINDOW_WIDTH - 16 * ui) / 2 + 28 * ui;
        exitBtn->move(exitX, panelY + 67 * ui);
        connect(exitBtn, &SpriteButton::clicked, this, &GameOverlayScreen::exitRequested);

        break;
    }
    }

    auto* exitBtn = new SpriteButton("scroll_button", this);
    exitBtn->setText("exit", 1, PixelFont::Dark);
    exitBtn->setTextOffset(-4, -2);
    int exitW = 41 * ui;
    int exitH = 17 * ui;
    exitBtn->setFixedSize(exitW, exitH);
    int exitX = (Constants::WINDOW_WIDTH - exitW) / 2 + 38 * ui;
    exitBtn->move(exitX, panelY + 69 * ui);
    connect(exitBtn, &SpriteButton::clicked, this, &GameOverlayScreen::exitRequested);
}

void GameOverlayScreen::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    if (m_game) m_game->draw(painter);
    painter.fillRect(rect(), QColor(0, 0, 0, 150));

    int panelW = 102 * ui;
    int panelH = 91 * ui;
    int panelX = (Constants::WINDOW_WIDTH - panelW) / 2 + 8 * Constants::UI_SCALE;
    int panelY = (Constants::WINDOW_HEIGHT - panelH) / 2;
    m_panel.draw(painter, panelX, panelY, panelW, panelH);

    int titleW = 78 * ui;
    int titleH = (m_type == Type::GameOver) ? 22 * ui : 17 * ui;
    int titleX = (Constants::WINDOW_WIDTH - titleW) / 2;
    int titleY = panelY - titleH - (5 * ui);
    m_titleLabel.draw(painter, titleX, titleY, titleW, titleH);

    if (m_game && m_game->score()) {
        QString scoreText = QString("score:%1").arg(m_game->score()->currentScore());

        int textScale = ui;
        int charW = 6 * textScale;
        int textWidth = scoreText.length() * charW;

        int textX = (Constants::WINDOW_WIDTH - textWidth) / 2 ;
        int textY = panelY + 24 * ui;
        m_pixelFont.drawText(painter, scoreText, textX, textY, textScale, PixelFont::Dark);
    }

    QWidget::paintEvent(event);
}

void GameOverlayScreen::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape && m_type == Type::Pause) {
        emit resumeRequested();
    } else {
        QWidget::keyPressEvent(event);
    }
}
void GameOverlayScreen::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    if (m_type == Type::Victory && m_nextLevelBtn && m_game) {
        int nextLevel = m_game->getCurrentLevel() + 1;
        bool hasNextLevel = LevelLoader::getRegisteredLevels().contains(nextLevel);
        m_nextLevelBtn->setVisible(hasNextLevel);
    }
}
