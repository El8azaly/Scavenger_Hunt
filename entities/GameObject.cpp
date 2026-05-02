#include "entities/GameObject.h"

GameObject::GameObject(float x, float y, float w, float h)
    : m_x(x), m_y(y), m_w(w), m_h(h)
{}

QRectF GameObject::boundingBox() const
{
    return { static_cast<double>(m_x),
             static_cast<double>(m_y),
             static_cast<double>(m_w),
             static_cast<double>(m_h) };
}
void GameObject::drawSprite(QPainter &painter, float camX, float camY, bool flipX) {
    if (m_sprite) {
        QRectF dest(m_x - camX + m_spriteOffsetX,
                    m_y - camY + m_spriteOffsetY,
                    m_spriteWidth, m_spriteHeight);
        m_sprite->draw(painter, dest, flipX);
    }
}