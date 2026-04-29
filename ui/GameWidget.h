#pragma once
#include <QWidget>
#include "core/Game.h"

class GameWidget : public QWidget {
    Q_OBJECT
public:
    explicit GameWidget(Game* game, QWidget* parent = nullptr);
protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
private:
    Game* m_game;  // non-owning
};
