// entities/Enemy.cpp
#include "entities/Enemy.h"
#include "entities/Player.h"
#include <cmath>

Enemy::Enemy(float x, float y, float w, float h, int maxHealth, Player* player)
    : AttackableEntity(x, y, w, h, maxHealth),
      m_player(player),
      m_attackTimer(0),
      m_landTimer(0),
      m_jumpCooldown(0),
      m_reactionTimer(0),
      m_isAggroed(false),
      m_attackRange(40.0f),
      m_triggerRange(300.0f),
      m_loseAggroRange(2250.0f),
      m_speed(1.8f),
      m_jumpForce(-12.0f),
      m_attackDuration(60),
      m_alpha(1.0f),
      m_deathFadeDelay(80),
      m_fadeSpeed(0.02f)
{}

void Enemy::updateBehavior() {
    if (m_isDead || m_hitTimer > 0 || m_attackTimer > 0 || !m_player) return;

    float distToPlayerX = m_player->x() - m_x;
    float absDistX = std::abs(distToPlayerX);
    float absDistY = std::abs(m_player->y() - m_y);

    if (!m_isAggroed) {
        if (absDistX < m_triggerRange && absDistY < 150.0f) {
            m_isAggroed = true;
            m_reactionTimer = 25;
        }
    } else {
        if (absDistX > m_loseAggroRange || absDistY > 500.0f) m_isAggroed = false;
    }

    if (!m_isAggroed) {
        m_velX = 0;
        return;
    }

    if (m_reactionTimer > 0) {
        m_reactionTimer--;
        m_velX *= 0.8f;
        return;
    }

    if (absDistX < m_attackRange && absDistY < 60.0f) {
        m_velX = 0;
        m_attackTimer = m_attackDuration;
        m_facingRight = (distToPlayerX > 0);
        return;
    }

    bool shouldFaceRight = (distToPlayerX > 0);
    if (m_facingRight != shouldFaceRight && std::abs(m_velX) > 0.5f) {
        m_reactionTimer = 15;
    } else {
        m_facingRight = shouldFaceRight;
        m_velX = m_facingRight ? m_speed : -m_speed;
    }

    bool isBlocked = false;
    float sensorX = m_facingRight ? (m_x + m_w + 1.0f) : (m_x - 5.0f);
    QRectF wallSensor(sensorX, m_y + m_h - 20.0f, 4.0f, 10.0f);

    for (const QRectF& platform : m_platforms) {
        if (platform.intersects(wallSensor)) {
            isBlocked = true;
            break;
        }
    }

    bool playerIsHigher = (m_player->y() < m_y - 60.0f) && (absDistX < 200.0f);
    if (isBlocked || playerIsHigher) {
        if (m_onGround && m_jumpCooldown == 0) {
            m_velY = m_jumpForce;
            m_onGround = false;
            m_jumpCooldown = 50;
        } else if (isBlocked) {
            m_velX = 0;
        }
    }
}

void Enemy::update() {
    updateBehavior();

    updateAnimation();

    if (m_isDead) {
        m_deathTimer++;
        m_velX = 0;
        if (m_deathTimer > m_deathFadeDelay) {
            m_alpha -= m_fadeSpeed;
            if (m_alpha <= 0.0f) {
                m_alpha = 0.0f;
                // Note: If you don't have m_active in GameObject,
                // replace this with however your game handles deletion.
                m_active = false;
            }
        }
    } else {
        if (m_hitTimer > 0) m_hitTimer--;
        if (m_landTimer > 0) m_landTimer--;
        if (m_jumpCooldown > 0) m_jumpCooldown--;
        if (m_attackTimer > 0) m_attackTimer--;

        Entity::update();
    }

    if (m_velX > 0) m_facingRight = true;
    else if (m_velX < 0) m_facingRight = false;
}

void Enemy::onHit() {
    m_reactionTimer = 0;
    m_isAggroed = true; // Instantly aggro if sniped from far away
}