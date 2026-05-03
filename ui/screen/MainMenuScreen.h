#pragma once
#include <QWidget>
#include <QRect>
#include <QVector>

#include "../sprite/SkyBackground.h"
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

    SpriteButton* m_backBtn;
    QVector<SpriteButton*> m_levelBtns;
};