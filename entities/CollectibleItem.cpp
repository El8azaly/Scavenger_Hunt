#include "entities/CollectibleItem.h"
#include <QPainter>
#include <cmath>

// Use m_item's custom width and height instead of hardcoding 36
CollectibleItem::CollectibleItem(float x, float y, const Item& item)
    : GameObject(x, y, item.spriteWidth, item.spriteHeight), m_item(item)
{
    // Initialize the AnimatedSprite
    if (!m_item.spriteJsonPath.isEmpty()) {
        m_sprite = new AnimatedSprite(m_item.spriteJsonPath, m_item.spriteImagePath);
        m_sprite->setState("Idle");

        // Apply custom sizes
        m_spriteWidth = item.spriteWidth;
        m_spriteHeight = item.spriteHeight;
    }
}

void CollectibleItem::update() {
    // Hovering/bobbing effect
    m_bobOffset = 5.0f * std::sin(m_bobTimer++ * 0.08f);

    // Advance the animation
    if (m_sprite) {
        m_sprite->update(16); // 16ms for ~60fps
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
        // Draw the animated sprite with the bob offset applied
        QRectF dest(sx + m_spriteOffsetX, sy + m_spriteOffsetY, m_spriteWidth, m_spriteHeight);
        m_sprite->draw(painter, dest, false);
    }
    else {
        // Fallback placeholder if no animation exists
        painter.fillRect(QRectF(sx, sy, m_w, m_h), QColor(255, 220, 0));
        painter.setPen(Qt::black);
        painter.drawRect(QRectF(sx, sy, m_w, m_h));
    }
}