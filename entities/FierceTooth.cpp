#include "FierceTooth.h"
#include <QPainter>

#include "Player.h"

FierceTooth::FierceTooth(float x, float y, Player* player)
    : Enemy(x, y, 64, 64, 60, player), m_wasOnGround(true)
{
    m_sprite = new AnimatedSprite(":/assets/sprites/enemy/fierce_tooth.json",
                                  ":/assets/sprites/enemy/fierce_tooth.png");
    m_spriteWidth = 64;
    m_spriteHeight = 64;

    m_speed = 1.8f;
    m_attackRange = 40.0f;
}

FierceTooth::~FierceTooth() {
    delete m_sprite;
}

void FierceTooth::draw(QPainter& painter, float camX, float camY) {
    if (!m_sprite || m_alpha <= 0.0f) return;

    painter.save();
    painter.setOpacity(m_alpha);
    QRectF destRect(m_x - camX, m_y - camY, m_w, m_h);
    m_sprite->draw(painter, destRect, m_facingRight);
    painter.restore();
}

void FierceTooth::updateAnimation() {
    if (!m_sprite) return;

    if (m_isDead) {
        if (m_deathTimer < 30) m_sprite->setState("Dead Hit");
        else m_sprite->setState("Dead Ground");
    }
    else if (m_hitTimer > 0) {
        m_sprite->setState("Hit");
    }
    else if (m_attackTimer > 0) {
        if (m_attackTimer > 35) {
            m_sprite->setState("Anticipation");
        } else {
            m_sprite->setState("Attack");
            if (m_attackTimer == 34 && m_player) {
                m_player->takeDamage(10);
            }
        }
    }
    else if (m_landTimer > 0) {
        m_sprite->setState("Ground");
    }
    else {
        if (!m_onGround) {
            m_sprite->setState(m_velY < 0 ? "Jump" : "Fall");
        } else if (std::abs(m_velX) > 0.1f) {
            m_sprite->setState("Run");
        } else {
            m_sprite->setState("Idle");
        }
    }

    m_sprite->update(16);

    // Landing Tracker Logic
    bool landed = !m_wasOnGround && m_onGround;
    if (landed) m_landTimer = 12;

    m_wasOnGround = m_onGround;
}