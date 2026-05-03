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
    void onPlayButtonClicked();

    SlicedSprite m_background;
    SlicedSprite m_levelScreenBg;
    bool  m_isHovering = false;
    bool  m_showingLevels = false;

    SkyBackground* m_skyBg;
    QTimer* m_animationTimer;

    SpriteButton* m_titlePanel;
    SpriteButton* m_playBtn;
    SpriteButton* m_gridBtn;
    SpriteButton* m_exitBtn;
    PixelFont m_font;
    SpriteButton* m_backBtn;
    QVector<SpriteButton*> m_levelBtns;
    SlicedSprite m_lockedWarning;
    bool m_showLockedWarning = false;
    int m_warningTimerMs = 0;
    int m_warningTargetX = 0;
    int m_warningTargetY = 0;
};