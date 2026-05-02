#include "entities/Container.h"
#include <QPainter>

Container::Container(float x, float y, float w, float h, const QString& type)
    : InteractiveObject(x, y, w, h), m_type(type) {
    m_sprite = std::make_unique<AnimatedSprite>(":/assets/sprites/objects/interactables.json",
                                                ":/assets/sprites/objects/interactables.png");

    m_spriteWidth = w;
    m_spriteHeight = h;

    m_spriteOffsetX = 0.0f;
    m_spriteOffsetY = 0.0f;

    updateAnimation();
}

Container::~Container() = default;

void Container::update() {

    if (m_opening && m_sprite->isFinished()) {
        m_opening = false;
        m_opened = true;
        updateAnimation();
    }

    m_sprite->update(16);
}

void Container::updateAnimation() {
    if (!m_sprite) return;

    QString targetState;
    bool loop = false;

    if (m_opened) {

        if (m_type == "box") targetState = "Box Destroyed";
        else if (m_type == "barrel") targetState = "Barrel Drestroyed";
        else targetState = "Chest Unlocked";
    }
    else if (m_opening) {

        if (m_type == "box") targetState = "Box Destroyed";
        else if (m_type == "barrel") targetState = "Barrel Drestroyed";
        else targetState = "Chest Unlocked";
        loop = false;
    }
    else {

        if (m_type == "box") targetState = "Box Idle";
        else if (m_type == "barrel") targetState = "Barrel Idle";
        else targetState = "Chest Idle";
        loop = true;
    }

    if (m_sprite->currentState() != targetState) {
        m_sprite->setState(targetState);
        m_sprite->setLoop(loop);
    }
}

void Container::draw(QPainter& painter, float camX, float camY) {
    if (!m_sprite) return;

    QRectF destRect(
        x() + m_spriteOffsetX - camX,
        y() + m_spriteOffsetY - camY,
        m_spriteWidth,
        m_spriteHeight
    );

    m_sprite->draw(painter, destRect, false);
}

void Container::addItem(const Item& item) {
    m_contents.append(item);
}

InteractionResult Container::interact() {
    InteractionResult res;

    if (m_opened || m_opening) {
        res.type = InteractionResult::Type::None;
        return res;
    }

    if (m_isLocked) {
        res.type = InteractionResult::Type::ShowPuzzle;
        res.puzzle.type = m_puzzleType;
        res.puzzle.data = m_puzzleData;
        res.puzzle.objectId = id();
        return res;
    }

    m_opening = true;
    updateAnimation();

    res.type = InteractionResult::Type::RevealItems;
    res.revealedItems = m_contents;
    m_contents.clear();

    return res;
}