#include "Cannon.h"
#include "Cannonball.h"
#include <QPainter>
#include <QTimer>

Cannon::Cannon(float x, float y, Player* player, float shotPower, float shotRange)
    : Enemy(x, y, 64, 64, 80, player),
      m_hitboxOffsetX(0), m_hitboxOffsetY(0),
      m_spawnOffsetX(-10), m_spawnOffsetY(20),
      m_fireTimer(0), m_fireCooldown(150), m_isFiring(false),
      m_shotPower(shotPower), m_shotRange(shotRange) {

    m_sprite = new AnimatedSprite(":/assets/sprites/enemy/cannon.json", ":/assets/sprites/enemy/cannon.png");
    m_speed = 0.0f;

    // Give enough time for BOTH destroyed animations to play before fading away
    m_deathFadeDelay = 180;
}
Cannon::~Cannon() {
    // Restore the normal cleanup
    delete m_sprite;
    for (Cannonball* ball : m_pendingProjectiles) {
        delete ball;
    }
    m_pendingProjectiles.clear();
}

QVector<Cannonball*> Cannon::takeNewProjectiles() {
    QVector<Cannonball*> projectiles = m_pendingProjectiles;
    m_pendingProjectiles.clear();
    return projectiles;
}

void Cannon::updateBehavior() {
    if (m_isDead || m_hitTimer > 0) return;

    m_velX = 0;
    m_velY = 0; // Prevent it from falling/moving

    // Handle Fire logic
    m_fireTimer++;
    if (m_fireTimer >= m_fireCooldown) {
        m_isFiring = true;
        m_fireTimer = 0;

        // Spawn the cannonball
        float spawnX = m_x + m_spawnOffsetX;
        float spawnY = m_y + m_spawnOffsetY;

        // FIX: Pass -m_shotPower and m_shotRange instead of the hardcoded -3.5f
        auto* ball = new Cannonball(spawnX, spawnY, -m_shotPower, m_shotRange, m_player);
        m_pendingProjectiles.append(ball);
    }
}
void Cannon::updateAnimation() {
    if (!m_sprite) return;

    if (m_isDead) {
        if (m_deathTimer < 6) {
            m_sprite->setState("Destroyed");
            m_sprite->setLoop(false);
        } else {
            m_sprite->setState("VERYDestroyed");
            m_sprite->setLoop(false);
        }
    }
    else if (m_hitTimer > 0) {
        m_sprite->setState("Hit");
        m_isFiring = false;
    }
    else if (m_isFiring) {
        m_sprite->setState("Fire");
        if (m_sprite->isFinished() || m_fireTimer > 30) {
            m_isFiring = false;
        }
    }
    else {
        m_sprite->setState("Idle");
    }

    m_sprite->update(16);
}

void Cannon::draw(QPainter& painter, float camX, float camY) {
    if (!m_sprite || m_alpha <= 0.0f) return;

    painter.save();
    painter.setOpacity(m_alpha);

    // Original sprite is 96x96, doubled is 192x192
    float spriteW = 192.0f;
    float spriteH = 192.0f;

    // Cannon starts at x=48, y=32 and is 32x32. Doubled, it starts at 96, 64 and is 64x64.
    // Subtract 96 and 64 from the draw coordinates to align the 64x64 visual with our 64x64 hitbox.
    float drawX = m_x - camX - 96.0f + m_hitboxOffsetX;
    float drawY = m_y - camY - 64.0f + m_hitboxOffsetY;

    QRectF destRect(drawX, drawY, spriteW, spriteH);
    m_sprite->draw(painter, destRect, false);
    painter.restore();

    if (Constants::DRAW_COLLISION_BOXES) {
        painter.save();
        painter.setPen(Qt::magenta);
        painter.setBrush(Qt::NoBrush);
        QRectF box = boundingBox();
        box.translate(-camX, -camY);
        painter.drawRect(box);
        painter.restore();
    }
}