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
    if (m_isDead) {
        m_deathTimer++;
        m_velX = 0; // Prevent sliding while dead
    } else {
        Entity::update(); // Handle physics
        if (m_hitTimer > 0) m_hitTimer--;
        if (m_landTimer > 0) m_landTimer--;
    }

    if (m_velX > 0) m_facingRight = true;
    else if (m_velX < 0) m_facingRight = false;
}

void Player::attack() {
    // Prevent attacking if no sword, if dead, or if an attack is already playing[cite: 23]
    if (!m_hasSword || m_isDead || m_attackTimer > 0) return;

    m_attackTimer = 25;

    if (!m_onGround) {
        m_sprite->setState(rand() % 2 == 0 ? "Air Attack 1" : "Air Attack 2");
    } else {
        int r = rand() % 3;
        if (r == 0) m_sprite->setState("Attack 1");
        else if (r != 1) m_sprite->setState("Attack 2");
        else m_sprite->setState("Attack 3");
    }
}

int Player::getHealth() const { return m_health; }
int Player::getMaxHealth() const { return m_maxHealth; }

void Player::takeDamage(int amount) {
    if (m_isDead) return;

    m_health -= amount;
    m_hitTimer = 20; // Trigger Hit animation

    if (m_health <= 0) {
        m_health = 0;
        m_isDead = true;
        m_deathTimer = 0;
    }
}

void Player::updateAnimation() {
    if (!m_sprite || !m_dustSprite) return;

    QString suffix = m_hasSword ? " S" : "";

    // --- Animation Priority Tree ---
    if (m_isDead) {
        // Frames 21-24 or 46-49 (Dead Hit), then Dead Ground[cite: 18]
        if (m_deathTimer < 30) m_sprite->setState("Dead Hit" + suffix);
        else m_sprite->setState("Dead Ground");
    }
    else if (m_hitTimer > 0) {
        m_sprite->setState("Hit"); // Generic hit tag[cite: 18]
    }
    else if (m_attackTimer > 0) {
        m_attackTimer--; // Tick down the attack timer[cite: 23]
        // Note: attack() already set the specific Attack state
    }
    else if (m_landTimer > 0) {
        m_sprite->setState("Ground" + suffix); // Landing frames 15-16 or 44-45[cite: 18]
    }
    else {
        // Normal movement animations[cite: 23]
        if (!m_onGround) {
            m_sprite->setState((m_velY < 0 ? "Jump" : "Fall") + suffix);
        } else if (std::abs(m_velX) > 0.1f) {
            m_sprite->setState("Run" + suffix);
        } else {
            m_sprite->setState("Idle" + suffix);
        }
    }

    m_sprite->update(16);

    // --- Dust Animation Logic (Preserved) ---
    bool landed = !m_wasOnGround && m_onGround;
    bool jumping = m_wasOnGround && !m_onGround && m_velY < 0;
    bool running = m_onGround && std::abs(m_velX) > 0.1f;

    if (landed) {
        m_landTimer = 12; // Start the "Ground" player animation[cite: 18]
        m_dustSprite->setState("Fall");
    } else if (jumping) {
        m_dustSprite->setState("Jump");
    } else if (running) {
        if (m_dustSprite->currentState() != "Fall" && m_dustSprite->currentState() != "Jump") {
            m_dustSprite->setState("Run");
        }
    } else {
        m_dustSprite->setState("");
    }

    m_dustSprite->update(16);
    m_wasOnGround = m_onGround;
}

void Player::draw(QPainter& painter, float camX, float camY) {
    // 1. Draw Dust (Behind player)[cite: 23]
    if (m_dustSprite && !m_dustSprite->currentState().isEmpty()) {
        float centerX = m_x + (m_w / 2.0f);
        float centerY = m_y + (m_h / 2.0f);

        QRectF destRect(centerX - (DUST_W / 2.0f) - camX,
                        centerY - (DUST_H / 2.0f) - camY + 20,
                        DUST_W, DUST_H);

        m_dustSprite->draw(painter, destRect, !m_facingRight);
    }

    // 2. Draw Player[cite: 23]
    drawSprite(painter, camX, camY, !m_facingRight);
}