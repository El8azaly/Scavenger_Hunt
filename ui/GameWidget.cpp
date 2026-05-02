#include "ui/GameWidget.h"
#include <QPainter>
#include <QKeyEvent>

GameWidget::GameWidget(Game* game, QWidget* parent)
    : QWidget(parent), m_game(game) {
    setFocusPolicy(Qt::StrongFocus);
    setFocus();
}

void GameWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.fillRect(rect(), QColor(30, 30, 40));
    m_game->draw(painter);
}

void GameWidget::keyPressEvent(QKeyEvent* e) {
    m_game->handleKeyPress(e->key());
}
void GameWidget::mousePressEvent(QMouseEvent *event) {
    m_game->handleMousePress(event->button());
}

void GameWidget::keyReleaseEvent(QKeyEvent* e) {
    m_game->handleKeyRelease(e->key());
}