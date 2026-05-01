#pragma once
#include "entities/Entity.h" 
#include "engine/InputHandler.h" 
#include "engine/AnimatedSprite.h"

class Player : public Entity {
public:
    Player(float x, float y, InputHandler* input);
    ~Player();
    void update() override;
    void draw(QPainter& painter, float camX, float camY) override;
    void updateAnimation();

private:
    InputHandler* m_input;
    AnimatedSprite* m_dustSprite;
    bool m_wasOnGround; // Used to detect landing and jumping

    // Dust dimensions from dust.json
    const float DUST_W = 52.0f;
    const float DUST_H = 20.0f;
};