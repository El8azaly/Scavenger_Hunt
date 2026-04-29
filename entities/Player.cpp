#include "entities/Player.h"
#include <QPainter>

Player::Player(float x, float y, InputHandler* input)
    : Entity(x, y, 40, 60), m_input(input) {}

void Player::update() {
    Entity::update();      // physics
    if (++m_frameTimer >= 8) { m_frameTimer = 0; m_animFrame = (m_animFrame+1)%4; }
}

void Player::draw(QPainter& painter, float camX, float camY) {
    float sx = m_x - camX, sy = m_y - camY;
    painter.fillRect(QRectF(sx, sy, m_w, m_h), QColor(60, 120, 200)); // placeholder
    painter.setPen(Qt::white);
    painter.drawRect(QRectF(sx, sy, m_w, m_h));
}
