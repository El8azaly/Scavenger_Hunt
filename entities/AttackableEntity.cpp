// entities/AttackableEntity.cpp
#include "entities/AttackableEntity.h"

AttackableEntity::AttackableEntity(float x, float y, float w, float h, int maxHealth)
    : Entity(x, y, w, h),
      m_maxHealth(maxHealth),
      m_health(maxHealth),
      m_isDead(false),
      m_hitTimer(0),
      m_deathTimer(0)
{}

void AttackableEntity::takeDamage(int amount) {
    if (m_isDead) return;

    m_health -= amount;
    m_hitTimer = 20;

    if (m_health <= 0) {
        m_health = 0;
        m_isDead = true;
        m_deathTimer = 0;
        m_velX = 0;
        onDeath();
    } else {
        onHit();
    }
}