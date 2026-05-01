#include "entities/Player.h"
#include <QPainter>

Player::Player(float x, float y, InputHandler* input)
    : Entity(x, y, 40, 60), m_input(input), m_wasOnGround(true)
{
    // Player Setup
    m_sprite = new AnimatedSprite(":/assets/sprites/player/captain.json",
                                  ":/assets/sprites/player/captain.png");
    m_spriteWidth = 96;
    m_spriteHeight = 64;
    m_spriteOffsetX = (m_w - m_spriteWidth) / 2.0f;
    m_spriteOffsetY = (m_h - m_spriteHeight);

    // Dust Setup
    m_dustSprite = new AnimatedSprite(":/assets/sprites/player/dust.json",
                                      ":/assets/sprites/player/dust.png");
}

Player::~Player() {
    delete m_dustSprite;
}

void Player::update() {
    Entity::update(); // Handle physics
    if (m_velX > 0) m_facingRight = true;
    else if (m_velX < 0) m_facingRight = false;
}

void Player::updateAnimation() {
    if (!m_sprite || !m_dustSprite) return;

    // Player Animations
    if (!m_onGround) {
        m_sprite->setState(m_velY < 0 ? "Jump" : "Fall");
    } else if (std::abs(m_velX) > 0.1f) {
        m_sprite->setState("Run");
    } else {
        m_sprite->setState("Idle");
    }
    m_sprite->update(16);

    // Dust Animation Logic
    bool landed = !m_wasOnGround && m_onGround;
    bool jumping = m_wasOnGround && !m_onGround && m_velY < 0;
    bool running = m_onGround && std::abs(m_velX) > 0.1f;

    if (landed) {
        m_dustSprite->setState("Fall"); // Uses "Fall" tag (frames 13-18)
    } else if (jumping) {
        m_dustSprite->setState("Jump"); // Uses "Jump" tag (frames 6-12)
    } else if (running) {
        // Only set to Run if a priority animation (Jump/Fall) isn't playing
        if (m_dustSprite->currentState() != "Fall" && m_dustSprite->currentState() != "Jump") {
            m_dustSprite->setState("Run");
        }
    } else {
        m_dustSprite->setState(""); // Hide dust when idle
    }

    m_dustSprite->update(16);
    m_wasOnGround = m_onGround;
}

void Player::draw(QPainter& painter, float camX, float camY) {
    // 1. Draw Dust (Rendered first so it appears behind/under the player)
    if (m_dustSprite && !m_dustSprite->currentState().isEmpty()) {
        // Calculate center position: (Hitbox Center) - (Half Dust Size)
        float centerX = m_x + (m_w / 2.0f);
        float centerY = m_y + (m_h / 2.0f);

        QRectF destRect(centerX - (DUST_W / 2.0f) - camX,
                        centerY - (DUST_H / 2.0f) - camY + 20,
                        DUST_W, DUST_H);

        // Use the existing draw() method from AnimatedSprite
        m_dustSprite->draw(painter, destRect, !m_facingRight);
    }

    // 2. Draw Player
    drawSprite(painter, camX, camY, !m_facingRight);
}