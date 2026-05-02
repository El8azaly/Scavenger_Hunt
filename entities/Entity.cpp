#include "entities/Entity.h"
#include <algorithm>

Entity::Entity(float x, float y, float w, float h)
    : GameObject(x, y, w, h) {}

void Entity::update() {
    m_onGround = false;
    m_velY += Constants::GRAVITY;
    m_velY = std::min(m_velY, Constants::MAX_FALL_SPEED);
    m_x += m_velX;
    m_y += m_velY;
}

void Entity::land()           { m_velY = 0; m_onGround = true; }
void Entity::stopVertical()   { m_velY = 0; }
void Entity::stopHorizontal() { m_velX = 0; }
void Entity::jump(float v)    { m_velY = v; m_onGround = false; }