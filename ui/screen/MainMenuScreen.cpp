#include "MainMenuScreen.h"
#include <QApplication>
#include "../sprite/SpriteButton.h"
#include "core/Constants.h"
#include <QPainter>
#include <QTimer>
#include <algorithm>
#include <QSettings>
#include <functional>

#include "data/LevelLoader.h"
#include "ui/HUD.h"

constexpr auto ui = Constants::UI_SCALE;

class TooltipOverlay : public QWidget {
public:
    std::function<void(QPainter&)> paintCallback;

    TooltipOverlay(QWidget* parent) : QWidget(parent) {
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_TranslucentBackground);
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        if (paintCallback) {
            QPainter painter(this);
            paintCallback(painter);
        }
    }
};

MainMenuScreen::MainMenuScreen(QWidget* p) : QWidget(p),
    m_background("window_bg"),
    m_levelScreenBg("level_screen_1x1"),
    m_lockedWarning("unlock_level_warning_1x1") {

    m_skyBg = new SkyBackground();
    m_animationTimer = new QTimer(this);

    connect(m_animationTimer, &QTimer::timeout, this, [this]{
        m_skyBg->update(16);
        if (m_showLockedWarning) {
            m_warningTimerMs += 16;
            if (m_warningTimerMs > 1200) {
                m_showLockedWarning = false;
            }
        }
        this->update();
    });
    m_animationTimer->start(16);

    m_titlePanel = SpriteButton::createPaper("paper_blank", "main menu", this);
    m_titlePanel->setText("main menu", 1, PixelFont::Dark);
    int titleW = 98 * ui;
    int titleH = 64 * ui;
    m_titlePanel->setFixedSize(titleW, titleH);
    int centerX = (Constants::WINDOW_WIDTH - titleW) / 2;
    m_titlePanel->move(centerX, 71 * ui);

    int btnWidth = 35 * ui;
    int btnHeight = 35 * ui;
    m_playBtn = SpriteButton::createGreen("play", this);
    m_playBtn->setText("play", 1, PixelFont::Dark);
    m_playBtn->setTextOffset(1,0);
    m_playBtn->setFixedSize(btnWidth, btnHeight);
    int playX = (Constants::WINDOW_WIDTH - btnWidth) / 2;
    m_playBtn->move(playX, 360);

    int settingsBtnWidth = 16 * ui;
    int settingsBtnHeight = 16 * ui;
    m_gridBtn = SpriteButton::createGreen("", this);
    m_gridBtn->setIcon("grid_icon_1x1", 1);
    m_gridBtn->setTextOffset(0,-1);
    m_gridBtn->setFixedSize(settingsBtnWidth, settingsBtnHeight);
    int settingsX = (Constants::WINDOW_WIDTH - btnWidth) / 2 - ui * 16;
    m_gridBtn->move(settingsX, 388);

    m_exitBtn = new SpriteButton("scroll_button", this);
    m_exitBtn->setText("exit", 1, PixelFont::Dark);
    m_exitBtn->setTextOffset(-4, -2);
    int exitW = 41 * ui;
    int exitH = 17 * ui;
    m_exitBtn->setFixedSize(exitW, exitH);
    int exitX = (Constants::WINDOW_WIDTH - exitW) / 2 + ui * 40;
    m_exitBtn->move(exitX, 410);

    m_backBtn = SpriteButton::createYellow("", this);
    m_backBtn->setFixedSize(16 * ui, 16 * ui);
    m_backBtn->setIcon("left_arrow_icon_1x1", 1);
    m_backBtn->setTextOffset(0, -1);
    m_backBtn->move((Constants::WINDOW_WIDTH + 62 * ui) / 2, 88*ui);
    m_backBtn->hide();

    QVector<int> registeredLevels = LevelLoader::getRegisteredLevels();
    int bgWidth = 128 * ui;
    int bgHeight = 86 * ui;
    int bgStartX = (Constants::WINDOW_WIDTH - bgWidth) / 2;
    int bgStartY = (Constants::WINDOW_HEIGHT - bgHeight) / 2;

    int gridOffsetX = 19 * ui;
    int gridOffsetY = 19 * ui;
    int startX = bgStartX + 12 * ui;
    int startY = bgStartY + 16 * ui;

    QSettings settings;
    int maxUnlocked = settings.value("max_unlocked_level", 0).toInt();

    int numButtons = std::min(6, static_cast<int>(registeredLevels.size()));
    for (int i = 0; i < numButtons; ++i) {
        int lvlNumber = registeredLevels[i];
        auto* btn = SpriteButton::createYellow("", this);

        if (lvlNumber <= maxUnlocked || lvlNumber == 0) {
            btn->setText(QString::number(lvlNumber), 1, PixelFont::Dark);
        } else {
            btn->setText("!", 1, PixelFont::Dark);
        }

        btn->setFixedSize(16 * ui, 16 * ui);
        btn->setTextOffset(0, -1);
        int col = i % 2;
        int row = i / 2;
        btn->move(startX + (col * gridOffsetX), startY + (row * gridOffsetY));
        btn->hide();
        m_levelBtns.append(btn);

        connect(btn, &SpriteButton::clicked, this, [this, btn, lvlNumber]() {
            QSettings settings;
            int currentMaxUnlocked = settings.value("max_unlocked_level", 0).toInt();

            if (lvlNumber <= currentMaxUnlocked || lvlNumber == 0) {
                emit levelSelected(lvlNumber);
            } else {

                m_showLockedWarning = true;
                m_warningTimerMs = 0;

                int warnW = 76 * Constants::UI_SCALE;
                int warnH = 22 * Constants::UI_SCALE;

                m_warningTargetX = btn->x() + (btn->width() - warnW) / 2;
                m_warningTargetY = btn->y() - warnH - (4 * Constants::UI_SCALE);
            }
        });
    }

    connect(m_playBtn, &SpriteButton::clicked, this, &MainMenuScreen::onPlayButtonClicked);
    connect(m_gridBtn, &SpriteButton::clicked, this, &MainMenuScreen::showLevelScreen);
    connect(m_exitBtn, &SpriteButton::clicked, this, []{ QApplication::quit(); });
    connect(m_backBtn, &SpriteButton::clicked, this, &MainMenuScreen::showMainMenu);

    auto tooltipOverlay = new TooltipOverlay(this);
    tooltipOverlay->resize(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT);
    tooltipOverlay->paintCallback = [this](QPainter& painter) {
        if (!m_showingLevels || !m_showLockedWarning) return;

        const float FADE_IN = 150.0f;
        const float HOLD = 800.0f;
        const float FADE_OUT = 250.0f;

        float opacity = 0.0f;

        if (m_warningTimerMs < FADE_IN) {
            opacity = m_warningTimerMs / FADE_IN;
        } else if (m_warningTimerMs < FADE_IN + HOLD) {
            opacity = 1.0f;
        } else if (m_warningTimerMs < FADE_IN + HOLD + FADE_OUT) {
            opacity = 1.0f - ((m_warningTimerMs - FADE_IN - HOLD) / FADE_OUT);
        }

        painter.save();
        painter.setOpacity(opacity);
        m_lockedWarning.draw(painter, m_warningTargetX + 30*ui, m_warningTargetY-8*ui, 160 * ui, 33*ui);
        painter.restore();
    };

    connect(m_animationTimer, &QTimer::timeout, tooltipOverlay, [tooltipOverlay] {
        tooltipOverlay->update();
    });
}

void MainMenuScreen::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    static QPixmap vignetteCache;
    static bool isVignetteCached = false;
    if (!isVignetteCached) HUD::makeVignette(vignetteCache, isVignetteCached);
    m_skyBg->draw(painter, width(), height());
    painter.drawPixmap(0, 0, vignetteCache);

    if (m_showingLevels) {
        int bgWidth = 128 * ui;
        int bgHeight = 86 * ui;
        int bgX = (width() - bgWidth) / 2;
        int bgY = (height() - bgHeight) / 2;
        m_levelScreenBg.draw(painter, bgX, bgY, bgWidth, bgHeight);

    } else {
        int bgSize = 96 * ui;
        int bgX = (width() - bgSize) / 2;
        int bgY = (height() - bgSize) / 2;
        m_background.draw(painter, bgX, bgY, bgSize, bgSize);
        int scale = ui;
        QString line1 = "Scavenger";
        QString line2 = "Hunt";
        int w1 = line1.length() * 11 * scale;
        int w2 = line2.length() * 11 * scale;
        int x1 = (width() - w1) / 2;
        int x2 = (width() - w2) / 2;
        int y1 = 25 * scale;
        int y2 = y1 + (13 * scale);
        m_font.drawText(painter, line1, x1, y1, scale, PixelFont::Dark, true);
        m_font.drawText(painter, line2, x2, y2, scale, PixelFont::Dark, true);
    }

    QWidget::paintEvent(event);
}

void MainMenuScreen::onPlayButtonClicked() {
    QSettings settings;
    int targetLevel = settings.value("max_unlocked_level", 0).toInt();

    QVector<int> registeredLevels = LevelLoader::getRegisteredLevels();
    if (registeredLevels.contains(targetLevel)) {
        emit levelSelected(targetLevel);
    } else {
        showLevelScreen();
    }
}

void MainMenuScreen::showLevelScreen() {
    m_showingLevels = true;

    m_titlePanel->hide();
    m_playBtn->hide();
    m_gridBtn->hide();
    m_exitBtn->hide();

    m_backBtn->show();

    QSettings settings;
    int maxUnlocked = settings.value("max_unlocked_level", 0).toInt();
    QVector<int> registeredLevels = LevelLoader::getRegisteredLevels();

    for(int i = 0; i < m_levelBtns.size(); ++i) {
        SpriteButton* btn = m_levelBtns[i];
        int lvlNumber = registeredLevels[i];
        if (lvlNumber <= maxUnlocked || lvlNumber == 0) {
            btn->setText(QString::number(lvlNumber), 1, PixelFont::Dark);
        } else {
            btn->setText("!", 1, PixelFont::Dark);
        }
        btn->show();
    }

    update();
}

void MainMenuScreen::showMainMenu() {
    m_showingLevels = false;

    m_titlePanel->show();
    m_playBtn->show();
    m_gridBtn->show();
    m_exitBtn->show();

    m_backBtn->hide();
    for(auto* btn : m_levelBtns) {
        btn->hide();
    }

    update();
}