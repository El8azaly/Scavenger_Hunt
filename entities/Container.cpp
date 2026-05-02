#include "entities/Container.h"
#include <QPainter>

Container::Container(float x, float y, float w, float h, const QString& type)
    : InteractiveObject(x, y, w, h), m_type(type)
{
    m_sprite = std::make_unique<AnimatedSprite>(":/assets/sprites/objects/interactables.json",
                                                ":/assets/sprites/objects/interactables.png");

    // Scale the sprite exactly to the w and h set in LevelLoader
    m_spriteWidth = w;
    m_spriteHeight = h;

    // No offset needed since it's exactly the size of the collision box
    m_spriteOffsetX = 0.0f;
    m_spriteOffsetY = 0.0f;

    updateAnimation();
}

Container::~Container() = default;

void Container::update() {
    // Transition from Opening to Opened once the animation concludes
    if (m_opening && m_sprite->isFinished()) {
        m_opening = false;
        m_opened = true;
        updateAnimation(); // Lock to final state
    }

    m_sprite->update(16);
}

void Container::updateAnimation() {
    if (!m_sprite) return;

    QString targetState;
    bool loop = false;

    // --- Animation Priority Tree ---
    if (m_opened) {
        // Final static broken/unlocked states
        if (m_type == "box") targetState = "Box Destroyed";
        else if (m_type == "barrel") targetState = "Barrel Drestroyed"; // Matching JSON spelling
        else targetState = "Chest Unlocked";
    }
    else if (m_opening) {
        // Play the destroy/unlock animation once when interacted with
        if (m_type == "box") targetState = "Box Destroyed";
        else if (m_type == "barrel") targetState = "Barrel Drestroyed";
        else targetState = "Chest Unlocked";
        loop = false; // Important: Must be false so it finishes and opens!
    }
    else {
        // Idle states
        if (m_type == "box") targetState = "Box Idle";
        else if (m_type == "barrel") targetState = "Barrel Idle";
        else targetState = "Chest Idle";
        loop = true;
    }

    // Only apply if the state has changed
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

    // Trigger the destroy/opening animation
    m_opening = true;
    updateAnimation();

    res.type = InteractionResult::Type::RevealItems;
    res.revealedItems = m_contents;
    m_contents.clear();

    return res;
}