#include "ui/GameWidget.h"
#include <QPainter>
#include <QKeyEvent>
#include <QGuiApplication>
#include <QClipboard>

GameWidget::GameWidget(Game* game, QWidget* parent)
    : QWidget(parent), m_game(game) {
    setFocusPolicy(Qt::StrongFocus);
    setFocus();
    setMouseTracking(true);
}

void GameWidget::mouseMoveEvent(QMouseEvent* event) {
    m_mousePos = event->pos();
}

void GameWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.fillRect(rect(), QColor(30, 30, 40));
    m_game->draw(painter);
}

void GameWidget::keyPressEvent(QKeyEvent* e) {
    if (e->key() == Qt::Key_Delete) {
        QString worldPosText = m_game->getWorldPosString(m_mousePos);
        QGuiApplication::clipboard()->setText(worldPosText);
    }
    m_game->handleKeyPress(e->key());
}
void GameWidget::mousePressEvent(QMouseEvent *event) {
    m_game->handleMousePress(event->button());
}

void GameWidget::keyReleaseEvent(QKeyEvent* e) {
    m_game->handleKeyRelease(e->key());
}