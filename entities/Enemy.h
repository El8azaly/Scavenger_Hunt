// entities/Enemy.h
#pragma once
#include "entities/AttackableEntity.h"
#include <QVector>
#include <QRectF>

class Player; // Forward declaration to keep the header clean

class Enemy : public AttackableEntity {
public:
    Enemy(float x, float y, float w, float h, int maxHealth, Player* player);

    void setEnvironment(const QVector<QRectF>& platforms) { m_platforms = platforms; }

    void update() override;

protected:
    virtual void updateBehavior();
    virtual void updateAnimation() {}

    void onHit() override;

    Player* m_player;
    QVector<QRectF> m_platforms;

    int m_attackTimer;
    int m_landTimer;
    int m_jumpCooldown;
    int m_reactionTimer;
    bool m_isAggroed;

    // Attributes tunable by derived specific enemies
    float m_attackRange;
    float m_triggerRange;
    float m_loseAggroRange;
    float m_speed;
    float m_jumpForce;
    int m_attackDuration;

    float m_alpha;
    int m_deathFadeDelay;
    float m_fadeSpeed;
};