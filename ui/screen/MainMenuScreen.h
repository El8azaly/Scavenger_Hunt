#pragma once
#include <QWidget>
#include <QRect>
#include <QVector>
#include "ui/sprite/PixelFont.h"
#include "ui/sprite/SkyBackground.h"
#include "ui/sprite/SlicedSprite.h"

class SpriteButton;

class MainMenuScreen : public QWidget {
    Q_OBJECT
public:
    explicit MainMenuScreen(QWidget* parent = nullptr);
    signals:
        void startRequested();
    void levelSelected(int levelNumber);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void showLevelScreen();
    void showMainMenu();
    void showResetConfirm();
    void performReset();
    void onPlayButtonClicked();

    SlicedSprite m_background;
    SlicedSprite m_levelScreenBg;
    SlicedSprite m_paperBg;
    bool  m_isHovering = false;
    bool  m_showingLevels = false;
    bool  m_showingResetConfirm = false;

    SkyBackground* m_skyBg;
    QTimer* m_animationTimer;

    SpriteButton* m_titlePanel;
    SpriteButton* m_playBtn;
    SpriteButton* m_resetBtn;
    SpriteButton* m_exitBtn;
    SpriteButton* m_yesBtn;
    SpriteButton* m_noBtn;

    PixelFont m_font;
    SpriteButton* m_backBtn;
    QVector<SpriteButton*> m_levelBtns;
    SlicedSprite m_lockedWarning;
    bool m_showLockedWarning = false;
    int m_warningTimerMs = 0;
    int m_warningTargetX = 0;
    int m_warningTargetY = 0;
};