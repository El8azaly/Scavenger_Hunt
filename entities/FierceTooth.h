// entities/FierceTooth.h
#pragma once
#include "entities/Enemy.h"
#include "engine/AnimatedSprite.h"

class FierceTooth : public Enemy {
public:
    FierceTooth(float x, float y, Player* player);
    ~FierceTooth();

    void draw(QPainter& painter, float camX, float camY) override;

private:
    void updateAnimation();

    AnimatedSprite* m_sprite;
    bool m_wasOnGround = true;
};