#include "entities/CollectibleItem.h"
#include <QPainter>
#include <cmath>
#include <cstdlib>

CollectibleItem::CollectibleItem(float x, float y, const Item& item)
    : GameObject(x, y, item.spriteWidth, item.spriteHeight), m_item(item) {
    if (!m_item.spriteJsonPath.isEmpty()) {
        m_sprite = new AnimatedSprite(m_item.spriteJsonPath, m_item.spriteImagePath);
        m_sprite->setState("Idle");

        m_spriteWidth = item.spriteWidth;
        m_spriteHeight = item.spriteHeight;
    }
}

void CollectibleItem::popOut(float startX, float startY) {
    m_x = startX;
    m_y = startY;
    m_velX = ((std::rand() % 400) - 200) / 100.0f;
    m_velY = -6.0f;
    m_popping = true;
    m_popStartY = startY;

    m_pickupDelay = 180;
}

void CollectibleItem::update() {

    if (m_pickupDelay > 0) {
        m_pickupDelay--;
    }

    if (m_popping) {
        m_bobOffset = 0;
        m_x += m_velX;
        m_y += m_velY;
        m_velY += 0.4f;

        if (m_velY > 0 && m_y >= m_popStartY) {
            m_y = m_popStartY;
            m_popping = false;
        }
    } else {
        m_bobOffset = 5.0f * std::sin(m_bobTimer++ * 0.08f);
    }

    if (m_sprite) {
        m_sprite->update(16);
    }
}
void CollectibleItem::markCollected() {
    m_collected = true;
    setActive(false);
}

void CollectibleItem::draw(QPainter& painter, float camX, float camY) {
    float sx = m_x - camX;
    float sy = m_y - camY + m_bobOffset;

    if (m_sprite) {
        QRectF dest(sx + m_spriteOffsetX, sy + m_spriteOffsetY, m_spriteWidth, m_spriteHeight);
        m_sprite->draw(painter, dest, false);
    }
    else {
        painter.fillRect(QRectF(sx, sy, m_w, m_h), QColor(255, 220, 0));
        painter.setPen(Qt::black);
        painter.drawRect(QRectF(sx, sy, m_w, m_h));
    }
}