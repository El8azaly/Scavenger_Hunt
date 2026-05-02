#pragma once
#include "entities/Entity.h" 
#include "engine/InputHandler.h" 
#include "../ui/sprite/AnimatedSprite.h"

class Player : public Entity {
public:
    Player(float x, float y, InputHandler* input);
    ~Player();
    void update() override;
    void draw(QPainter& painter, float camX, float camY) override;
    void updateAnimation();
    void equipSword() { m_hasSword = true; }
    bool attack();

    int getHealth() const;

    int getMaxHealth() const;

    void takeDamage(int amount);
    bool isAttacking() const { return m_attackTimer > 0; }
    int  getAttackTimer() const { return m_attackTimer; }
    bool isFacingRight() const { return m_facingRight; }
private:
    InputHandler* m_input;
    AnimatedSprite* m_dustSprite;
    bool m_wasOnGround;

    bool m_hasSword = false;
    bool m_isDead = false;
    int m_attackTimer = 0;
    int m_hitTimer = 0;
    int m_deathTimer = 0;
    int m_landTimer = 0; // For the "Ground" animation tag;

    const float DUST_W = 52.0f;
    const float DUST_H = 20.0f;
    int m_health = 100;
    int m_maxHealth = 100;
};