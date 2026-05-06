#include "Cannonball.h"
#include "entities/Player.h"
#include "core/Constants.h"
#include <QPainter>
#include <cmath>

Cannonball::Cannonball(float x, float y, float velX, float range, Player* player)
    : Entity(x, y, 30, 30), m_player(player), m_exploded(false), m_finished(false),
      m_explosionTimer(0), m_startX(x), m_range(range),
      m_hasTouchedGround(false), m_groundTimer(0) {

    // INCREASED RANDOMNESS: X Speed variance between -3.5 and +3.5
    float speedVariance = ((rand() % 700) - 350) / 100.0f;
    m_velX = velX + speedVariance;

    // INCREASED RANDOMNESS: Y pop variance between 0.0 and 5.0 (makes some jump really high, some shoot straight)
    float arcVariance = (rand() % 500) / 100.0f;
    m_velY = -2.0f - arcVariance;

    m_baseDamage = 40.0f;

    m_sprite = new AnimatedSprite(":/assets/sprites/enemy/cannon.json", ":/assets/sprites/enemy/cannon.png");
    m_sprite->setState("Ball");
}
Cannonball::~Cannonball() {
    // Restore the normal cleanup
    delete m_sprite;
}

void Cannonball::onGroundContact() {
    if (!m_hasTouchedGround) {
        m_hasTouchedGround = true;
        m_groundTimer = 0;
    }
}
void Cannonball::update() {
    if (m_finished) return;

    if (!m_exploded) {
        m_velY += Constants::GRAVITY;

        Entity::update();
        m_sprite->update(16);

        if (std::abs(m_x - m_startX) > m_range || m_y > Constants::WORLD_HEIGHT) {
            explode(false);
        }

        // EXPLODE WHEN STILL LOGIC
        if (m_hasTouchedGround) {
            m_groundTimer++;

            // Allow a tiny bit of Y velocity variance for gravity (up to 2.0f),
            // but if X velocity hits zero (less than 0.1f), detonate instantly!
            if (std::abs(m_velX) < 0.1f && std::abs(m_velY) < 2.0f) {
                explode(false);
            }

            // Fallback max lifespan
            if (m_groundTimer >= 120) {
                explode(false);
            }
        }

        if (m_player && !m_player->isDead()) {
            if (m_player->boundingBox().intersects(this->boundingBox())) {
                explode(true);
            }
        }
    } else {
        m_sprite->update(16);
        m_explosionTimer++;
        if (m_explosionTimer > 25 || m_sprite->isFinished()) {
            m_finished = true;
            m_active = false;
        }
    }
}

void Cannonball::explode(bool hitPlayerDirectly) {
    if (m_exploded) return;

    m_exploded = true;
    m_velX = 0;
    m_velY = 0;
    m_sprite->setState("Ball Explosion");
    m_sprite->setLoop(false);

    if (!m_player || m_player->isDead()) return;

    if (hitPlayerDirectly) {
        int dmg = m_player->getMaxHealth() / 2;
        m_player->takeDamage(dmg);
    } else {
        float dx = m_player->centreX() - this->centreX();
        float dy = m_player->centreY() - this->centreY();
        float distance = std::sqrt(dx * dx + dy * dy);

        float damageFloat = m_baseDamage * std::exp(-distance / 80.0f);
        int finalDamage = static_cast<int>(damageFloat);
        if (finalDamage > 0) m_player->takeDamage(finalDamage);
    }
}

void Cannonball::draw(QPainter& painter, float camX, float camY) {
    if (!m_sprite || m_finished) return;

    // Original sprite is 96x96, doubled is 192x192
    float spriteW = 192.0f;
    float spriteH = 192.0f;

    // Ball starts at x=41, y=41 and is 15x15. Doubled, it starts at 82, 82 and is 30x30.
    // Subtract 82 from the draw coordinates to perfectly align the 30x30 visual with our 30x30 hitbox.
    float drawX = m_x - camX - 82.0f;
    float drawY = m_y - camY - 82.0f;

    QRectF destRect(drawX, drawY, spriteW, spriteH);
    m_sprite->draw(painter, destRect, false);

    if (Constants::DRAW_COLLISION_BOXES) {
        painter.save();
        painter.setPen(Qt::red);
        painter.setBrush(Qt::NoBrush);
        QRectF box = boundingBox();
        box.translate(-camX, -camY);
        painter.drawRect(box);
        painter.restore();
    }
}