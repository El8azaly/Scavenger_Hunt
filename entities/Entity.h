#pragma once
#include "entities/GameObject.h"
#include "core/Constants.h"

class Entity : public GameObject {
public:
    Entity(float x, float y, float w, float h);
    void update() override;
    void draw(QPainter&, float, float) override = 0; // still abstract

    void land();           // called by Game on fromTop collision
    void stopVertical();   // called by Game on fromBottom collision
    void stopHorizontal(); // called by Game on fromLeft/Right collision
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
