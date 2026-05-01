#pragma once
#include "entities/GameObject.h"
#include "core/Constants.h"

class Entity : public GameObject {
public:
    Entity(float x, float y, float w, float h);
    void update() override;
    void draw(QPainter& painter, float camX, float camY) override = 0;

    void land();
    void stopVertical();
    void stopHorizontal();
    void jump(float velocity);

    void setVelocityX(float vx) { m_velX = vx; }
    void setVelocityY(float vy) { m_velY = vy; }
    float velX() const { return m_velX; }
    float velY() const { return m_velY; }
    bool  isOnGround() const { return m_onGround; }

protected:
    float m_velX = 0, m_velY = 0;
    bool  m_onGround   = false;
    bool  m_facingRight = true;
};