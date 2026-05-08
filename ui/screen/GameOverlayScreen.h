#pragma once
#include <QWidget>
#include <QShowEvent>
#include "ui/sprite/PixelFont.h"
#include "ui/sprite/SlicedSprite.h"

class Game;
class SpriteButton;

class GameOverlayScreen : public QWidget {
    Q_OBJECT
public:
    enum class Type {
        Pause,
        GameOver,
        Victory
    };

    explicit GameOverlayScreen(Game* game, Type type, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void showEvent(QShowEvent* event) override;

    signals:
        void resumeRequested();
    void restartRequested();
    void nextLevelRequested(int level);
    void exitRequested();

private:
    void setupUI();

    Game* m_game;
    Type m_type;
    PixelFont m_pixelFont;

    SlicedSprite m_panel;
    SlicedSprite m_titleLabel;

    SpriteButton* m_nextLevelBtn = nullptr;
};