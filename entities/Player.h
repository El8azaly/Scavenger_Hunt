#pragma once
#include "entities/Entity.h"
#include "engine/InputHandler.h"
#include <QPixmap>

class Player : public Entity {
public:
    Player(float x, float y, InputHandler* input);
    void update() override;
    void draw(QPainter& painter, float camX, float camY) override;

private:
    InputHandler* m_input;   // non-owning
    QPixmap       m_sprite;
    int           m_animFrame = 0;
    int           m_frameTimer = 0;
};
