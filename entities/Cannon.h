#pragma once
#include "entities/Enemy.h"
#include <QVector>

class Cannonball;

class Cannon : public Enemy {
public:
    Cannon(float x, float y, Player* player, float shotPower, float shotRange);

    ~Cannon();

    void updateAnimation() override;
    void draw(QPainter& painter, float camX, float camY) override;

    // We use this to expose newly fired cannonballs to the Game state
    QVector<Cannonball*> takeNewProjectiles();

    void setHitboxOffsets(float xOffset, float yOffset) {
        m_hitboxOffsetX = xOffset;
        m_hitboxOffsetY = yOffset;
    }

    void setSpawnOffsets(float xOffset, float yOffset) {
        m_spawnOffsetX = xOffset;
        m_spawnOffsetY = yOffset;
    }

protected:
    void updateBehavior() override;

private:
    float m_hitboxOffsetX;
    float m_hitboxOffsetY;

    float m_spawnOffsetX;
    float m_spawnOffsetY;
    float m_shotPower;
    float m_shotRange;
    int m_fireTimer;
    int m_fireCooldown;
    bool m_isFiring;

    QVector<Cannonball*> m_pendingProjectiles;
};