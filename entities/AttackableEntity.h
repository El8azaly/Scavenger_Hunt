// entities/AttackableEntity.h
#pragma once
#include "entities/Entity.h"

class AttackableEntity : public Entity {
public:
    AttackableEntity(float x, float y, float w, float h, int maxHealth);

    virtual void takeDamage(int amount);

    int getHealth() const { return m_health; }
    int getMaxHealth() const { return m_maxHealth; }
    bool isDead() const { return m_isDead; }
    int hitTimer() const { return m_hitTimer; }

protected:
    virtual void onDeath() {}
    virtual void onHit() {}

    int m_maxHealth;
    int m_health;
    bool m_isDead;
    int m_hitTimer;
    int m_deathTimer;
};