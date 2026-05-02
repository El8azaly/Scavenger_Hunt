// entities/Enemy.cpp
#include "entities/Enemy.h"
#include "entities/Player.h"
#include "entities/CollectibleItem.h"
#include "../ui/sprite/AnimatedSprite.h"
#include <QPainter>
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
{
    // Load the dialogue box sprite
    m_dialogSprite = new AnimatedSprite(":/assets/sprites/enemy/dialogue.json", ":/assets/sprites/enemy/dialogue.png");
    m_dialogSprite->setLoop(false);
}

Enemy::~Enemy() {
    if (m_beltItem && !m_itemDropped) {
        delete m_beltItem; // Prevent memory leak if enemy is deleted before dying
    }
    // Clean up dialog sprite
    delete m_dialogSprite;
}

void Enemy::setHeldItem(const Item& item) {
    m_beltItem = new CollectibleItem(m_x, m_y, item);
    m_prevX = m_x;
    m_prevY = m_y;
}

CollectibleItem* Enemy::getBeltItem() const {
    return m_itemDropped ? nullptr : m_beltItem;
}

CollectibleItem* Enemy::takeDroppedItem() {
    CollectibleItem* item = m_droppedItemToTransfer;
    m_droppedItemToTransfer = nullptr;
    return item;
}

// Helper to trigger dialogue
void Enemy::showDialog(const QString& type) {
    m_dialogState = type;
    m_dialogTimer = 120; // 2 Seconds at 60fps
    if (m_dialogSprite) {
        m_dialogSprite->setState(type + " In");
        m_dialogSprite->setLoop(false); // Play "In" and stop on last frame
    }
}

void Enemy::updateBehavior() {
    if (m_isDead || m_hitTimer > 0 || m_attackTimer > 0 || !m_player) return;

    float distToPlayerX = m_player->x() - m_x;
    float absDistX = std::abs(distToPlayerX);
    float absDistY = std::abs(m_player->y() - m_y);

    if (!m_isAggroed) {
        if (absDistX < m_triggerRange && absDistY < 150.0f) {
            m_isAggroed = true;
            m_reactionTimer = 25;

            // First time seeing player!
            if (!m_hasSeenPlayer) {
                m_hasSeenPlayer = true;
                showDialog("Exclamation");
            }
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
    m_prevX = m_x;
    m_prevY = m_y;

    updateBehavior();
    updateAnimation();

    // Handle Dialog Animation Sequence
    if (m_dialogSprite && m_dialogState != "None" && m_dialogState != "") {
        m_dialogSprite->update(16);

        if (m_dialogSprite->isFinished()) {
            if (m_dialogSprite->currentState().endsWith("In")) {
                if (m_dialogState != "Dead") { // Dead never leaves
                    if (m_dialogTimer > 0) {
                        m_dialogTimer--; // Wait 2 seconds on last frame
                    } else {
                        m_dialogSprite->setState(m_dialogState + " Out");
                        m_dialogSprite->setLoop(false);
                    }
                }
            } else if (m_dialogSprite->currentState().endsWith("Out")) {
                m_dialogState = "None"; // Clear dialog when completely out
            }
        }
    }

    if (m_isDead) {
        // Play Dead dialog at the exact moment of death
        if (m_deathTimer == 0) {
            showDialog("Dead");
        }

        // On death, pop the item and stage it to be grabbed by the global game loop
        if (m_beltItem && !m_itemDropped) {
            m_itemDropped = true;
            m_beltItem->popOut(m_x + m_w/2 - m_beltItem->width()/2, m_y + m_h - 10 - m_beltItem->height());
            m_droppedItemToTransfer = m_beltItem;
        }

        m_deathTimer++;
        m_velX = 0;
        if (m_deathTimer > m_deathFadeDelay) {
            m_alpha -= m_fadeSpeed;
            if (m_alpha <= 0.0f) {
                m_alpha = 0.0f;
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

    // Attach belt item 1 frame delayed, looking like it belongs on a belt
    if (m_beltItem && !m_itemDropped) {
        float beltX = m_prevX + (m_facingRight ? m_w/2 - 15 : m_w/2 + 5) - m_beltItem->width()/2;
        float beltY = m_prevY + m_h/2;
        m_beltItem->setPosition(beltX, beltY);
        m_beltItem->update();
    }
}

void Enemy::onHit() {
    m_reactionTimer = 0;
    m_isAggroed = true;

    // First time attacked!
    if (!m_hasBeenAttacked) {
        m_hasBeenAttacked = true;
        m_hasSeenPlayer = true; // Pre-empt Exclamation if hit from behind
        showDialog("Interrogation");
    }
}

// Draw logic for the dialogue
void Enemy::drawDialog(QPainter& painter, float camX, float camY) {
    // 2. Draw dialog box overlapping the enemy (and match opacity fade)
    if (m_dialogSprite && m_dialogState != "None" && m_dialogState != "") {
        float dx = m_x + (m_w / 2.0f) - 16.0f - camX; // Centered (assuming 32x32 dialog)
        float dy = m_y - 24.0f - camY; // Placed above head

        if (m_isDead) {
            // Drop down smoothly when dead to sit near their body
            dy += std::min(m_deathTimer * 0.8f, m_h - 5.0f);
        }

        painter.save();
        painter.setOpacity(m_alpha); // Applies the death fade effect
        m_dialogSprite->draw(painter, QRectF(dx, dy, 32.0f, 32.0f), false);
        painter.restore();
    }
}