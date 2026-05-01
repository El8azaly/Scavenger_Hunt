#include "entities/Container.h"
#include <QPainter>

Container::Container(float x, float y, float w, float h)
    : InteractiveObject(x, y, w, h)
{
    // Load the animated sprite for the chest
    m_sprite = new AnimatedSprite(":/assets/sprites/chest/chest.json",
                                  ":/assets/sprites/chest/chest.png");

    m_spriteWidth = w;
    m_spriteHeight = h;

    // Set default closed state matching your JSON frame tags
    m_sprite->setState("Closed");
}

void Container::addItem(const Item& i) {
    m_contents.append(i);
}

InteractionResult Container::interact() {
    // If it's already open, do nothing
    if (m_opened) {
        return {InteractionResult::Type::None};
    }

    // Handle Locks
    if (m_isLocked) {
        PuzzleData p;
        p.type = m_puzzleType;
        p.data = m_puzzleData;
        p.objectId = m_id;
        return {InteractionResult::Type::ShowPuzzle, p};
    }

    // Open the container
    m_opened = true;

    // Update the animation to the open state
    if (m_sprite) {
        m_sprite->setState("Open");
    }

    InteractionResult r;
    r.type = InteractionResult::Type::RevealItems;
    r.revealedItems = m_contents;
    return r;
}

void Container::update() {
    // Advance the chest's animation (e.g. for a shining effect or the opening lid sequence)
    if (m_sprite) {
        m_sprite->update(16);
    }
}

void Container::draw(QPainter& painter, float camX, float camY) {
    if (m_sprite) {
        // Uses the base GameObject method (applies m_x, m_y, camX, camY automatically)
        drawSprite(painter, camX, camY);
    }
    else {
        // Fallback colored box
        float sx = m_x - camX;
        float sy = m_y - camY;
        painter.fillRect(QRectF(sx, sy, m_w, m_h), m_opened ? QColor(180, 120, 60) : QColor(120, 70, 30));
    }
}