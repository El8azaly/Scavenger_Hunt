#include "ui/MainMenuScreen.h"
#include <QApplication>
#include "ui/SpriteButton.h"
#include "core/Constants.h"
#include <QPainter>

constexpr auto ui = Constants::UI_SCALE;
MainMenuScreen::MainMenuScreen(QWidget* p) : QWidget(p), m_background("window_bg") {
    auto* titlePanel = SpriteButton::createPaper("paper_blank", "main menu", this);
    titlePanel->setText("main menu", 1, PixelFont::Dark);
    int titleW = 98 * ui;
    int titleH = 64 * ui;
    titlePanel->setFixedSize(titleW, titleH);
    int centerX = (Constants::WINDOW_WIDTH - titleW) / 2;
    titlePanel->move(centerX, 71* ui);

    int btnWidth = 35 * ui;
    int btnHeight = 35 * ui;
    auto* playBtn = SpriteButton::createGreen("play", this);
    playBtn->setText("play", 1, PixelFont::Dark);
    playBtn->setTextOffset(1,0);
    playBtn->setFixedSize(btnWidth, btnHeight);
    int playX = (Constants::WINDOW_WIDTH - btnWidth) / 2;
    playBtn->move(playX, 360);

    int settingsBtnWidth = 16 * ui;
    int settingsBtnHeight = 16 * ui;
    auto* settingsButton = SpriteButton::createGreen("", this);
    settingsButton->setIcon("circle_icon_1x1", 1);
    settingsButton->setTextOffset(0,-1);
    settingsButton->setFixedSize(settingsBtnWidth, settingsBtnHeight);
    int settingsX = (Constants::WINDOW_WIDTH - btnWidth) / 2 - ui *16;
    settingsButton->move(settingsX, 388);
    connect(playBtn, &SpriteButton::clicked, this, &MainMenuScreen::startRequested);

    auto* exitBtn = new SpriteButton("scroll_button", this);
    exitBtn->setText("exit", 1, PixelFont::Dark);
    exitBtn->setTextOffset(-4, -2);
    int exitW = 41 * ui;
    int exitH = 17 * ui;
    exitBtn->setFixedSize(exitW, exitH);
    int exitX = (Constants::WINDOW_WIDTH - exitW) / 2 + ui*40;
    exitBtn->move(exitX, 410);
    connect(exitBtn, &SpriteButton::clicked, this, []{ QApplication::quit(); });
}

void MainMenuScreen::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    int bgSize = 96 * ui;
    int bgX = (width() - bgSize) / 2;
    int bgY = (height() - bgSize) / 2;
    m_background.draw(painter, bgX, bgY, bgSize, bgSize);
    QWidget::paintEvent(event);
}