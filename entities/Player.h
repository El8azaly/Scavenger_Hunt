#pragma once
#include "entities/Entity.h" 
#include "engine/InputHandler.h" 

class Player : public Entity { 
public:
    Player(float x, float y, InputHandler* input); 
    void update() override; 
    void draw(QPainter& painter, float camX, float camY) override; 
    void updateAnimation(); // New method
private:
    InputHandler* m_input;   // non-owning

    // Note: Old manual QPixmap and timers are removed since we
    // now inherit `AnimatedSprite* m_sprite` from GameObject.
};