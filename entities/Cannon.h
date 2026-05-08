#pragma once
#include "entities/Enemy.h"
#include <QVector>

class Cannonball;

class Cannon : public Enemy {
public:
    Cannon(float x, float y, Player* player, float maxSpeed, float shotRange);
    ~Cannon();

    void updateAnimation() override;
    void draw(QPainter& painter, float camX, float camY) override;

    QVector<Cannonball*> takeNewProjectiles();

    void setTrajectoryTarget(float targetX, float targetY) {
        m_targetX = targetX;
        m_targetY = targetY;
        m_hasTarget = true;
    }

protected:
    void updateBehavior() override;

private:

    static constexpr float SPAWN_OFFSET_X = -10.0f;
    static constexpr float SPAWN_OFFSET_Y = 20.0f;

    float m_maxSpeed;
    float m_shotRange;
    int m_fireTimer;
    int m_fireCooldown;
    bool m_isFiring;

    bool m_hasTarget = false;
    float m_targetX = 0.0f;
    float m_targetY = 0.0f;

    QVector<Cannonball*> m_pendingProjectiles;
};