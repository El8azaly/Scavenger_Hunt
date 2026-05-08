#include "Cannon.h"
#include "Cannonball.h"
#include <QPainter>
#include <QTimer>
#include "core/Constants.h"
#include <QPainterPath>
#include <cmath>

Cannon::Cannon(float x, float y, Player* player, float maxSpeed, float shotRange)
    : Enemy(x, y, 64, 64, 80, player),
      m_fireTimer(0), m_fireCooldown(150), m_isFiring(false),
      m_maxSpeed(maxSpeed), m_shotRange(shotRange) {

    m_sprite = new AnimatedSprite(":/assets/sprites/enemy/cannon.json", ":/assets/sprites/enemy/cannon.png");
    m_speed = 0.0f;
    m_deathFadeDelay = 180;
}

Cannon::~Cannon() {
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

void Cannon::updateAnimation() {
    if (!m_sprite) return;

    if (m_isDead) {
        if (m_deathTimer < 12) {
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

void Cannon::updateBehavior() {
    if (m_isDead || m_hitTimer > 0) return;

    m_velX = 0;
    m_velY = 0;

    m_fireTimer++;
    if (m_fireTimer >= m_fireCooldown) {
        m_isFiring = true;
        m_fireTimer = 0;

        float spawnX = m_x + SPAWN_OFFSET_X;
        float spawnY = m_y + SPAWN_OFFSET_Y;

        float initialVelX = -m_maxSpeed;

        if (m_hasTarget) {
            float dy = m_targetY - spawnY;
            if (dy > 0) {
                float t = std::sqrt(2.0f * dy / Constants::GRAVITY);
                initialVelX = (m_targetX - spawnX) / t;

                if (std::abs(initialVelX) > m_maxSpeed) {
                    initialVelX = (initialVelX > 0) ? m_maxSpeed : -m_maxSpeed;
                }
            }
        }

        auto* ball = new Cannonball(spawnX, spawnY, initialVelX, m_shotRange, m_player);
        m_pendingProjectiles.append(ball);
    }
}

void Cannon::draw(QPainter& painter, float camX, float camY) {
    if (!m_sprite || m_alpha <= 0.0f) return;

    painter.save();
    painter.setOpacity(m_alpha);

    float spriteW = 192.0f;
    float spriteH = 192.0f;

    float drawX = m_x - camX - 96.0f;
    float drawY = m_y - camY - 64.0f;

    QRectF destRect(drawX, drawY, spriteW, spriteH);
    m_sprite->draw(painter, destRect, false);
    painter.restore();

    if constexpr (Constants::DRAW_COLLISION_BOXES) {
        painter.save();
        painter.setPen(Qt::magenta);
        painter.setBrush(Qt::NoBrush);
        QRectF box = boundingBox();
        box.translate(-camX, -camY);
        painter.drawRect(box);

        painter.setPen(QPen(Qt::yellow, 2, Qt::DashLine));

        float spawnX = m_x + SPAWN_OFFSET_X;
        float spawnY = m_y + SPAWN_OFFSET_Y;
        float simVx = -m_maxSpeed;

        if (m_hasTarget) {
            float dy = m_targetY - spawnY;
            if (dy > 0) {
                float t = std::sqrt(2.0f * dy / Constants::GRAVITY);
                simVx = (m_targetX - spawnX) / t;
                if (std::abs(simVx) > m_maxSpeed) {
                    simVx = (simVx > 0) ? m_maxSpeed : -m_maxSpeed;
                }
            }
        }

        float simX = spawnX;
        float simY = spawnY;
        float simVy = 0.0f;

        QPainterPath path;
        path.moveTo(simX - camX, simY - camY);

        for (int i = 0; i < 300; ++i) {
            simVy += Constants::GRAVITY;
            simX += simVx;
            simY += simVy;
            path.lineTo(simX - camX, simY - camY);

            if (m_hasTarget && simY >= m_targetY) break;
            if (!m_hasTarget && std::abs(simX - spawnX) > m_shotRange) break;
            if (simY > m_y + 1500.0f) break;
        }
        painter.drawPath(path);

        if (m_hasTarget) {
            painter.setPen(QPen(Qt::red, 2));
            float tX = m_targetX - camX;
            float tY = m_targetY - camY;
            painter.drawLine(tX - 10, tY, tX + 10, tY);
            painter.drawLine(tX, tY - 10, tX, tY + 10);
        }

        painter.restore();
    }
}