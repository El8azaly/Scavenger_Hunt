#include "entities/Player.h"
#include <QPainter>
#include <cmath>

/**
 * Player Constructor
 * Initializes the entity dimensions and loads the animated sprite using the
 * specific resource paths defined in the .qrc file.
 */
Player::Player(float x, float y, InputHandler* input)
    : Entity(x, y, 40, 60), m_input(input)
{
    // Load the sprite and JSON from the Qt Resource System
    m_sprite = new AnimatedSprite(":/assets/sprites/player/captain.json",
                                  ":/assets/sprites/player/captain.png");

    // The Captain Clown Nose sprite sheet uses 96x96 frames.
    // We set these dimensions and calculate offsets to align the 96x96 visual
    // with the 40x60 physical hitbox.
    m_spriteWidth = 96;
    m_spriteHeight = 64;

    // Horizontal offset: Center the 96px sprite over the 40px hitbox
    m_spriteOffsetX = (m_w - m_spriteWidth) / 2.0f;

    // Vertical offset: Align the bottom of the sprite (feet) with the bottom of the hitbox
    m_spriteOffsetY = (m_h - m_spriteHeight);
}

/**
 * update()
 * Handles physics integration and updates the animation state machine.
 */
// Player.cpp
// Player.cpp
void Player::update() {
    Entity::update(); // Handle physics only

    if (m_velX > 0) m_facingRight = true;
    else if (m_velX < 0) m_facingRight = false;
}

void Player::updateAnimation() {
    if (!m_sprite) return;

    // This logic now runs AFTER collisions have been resolved in Game.cpp
    if (!m_onGround) {
        m_sprite->setState(m_velY < 0 ? "Jump" : "Fall");
    }
    else if (std::abs(m_velX) > 0.1f) {
        m_sprite->setState("Run");
    }
    else {
        m_sprite->setState("Idle");
    }

    m_sprite->update(16);
}

/**
 * draw()
 * Renders the sprite to the screen using the camera-relative position
 * and handling horizontal flipping.
 */
void Player::draw(QPainter& painter, float camX, float camY)
{
    // drawSprite is a protected helper in GameObject that applies
    // world-to-screen conversion and sprite offsets.
    drawSprite(painter, camX, camY, !m_facingRight);

    // Debug: Uncomment to visualize the 40x60 physical hitbox
    /*
    painter.setPen(Qt::red);
    painter.drawRect(m_x - camX, m_y - camY, m_w, m_h);
    */
}