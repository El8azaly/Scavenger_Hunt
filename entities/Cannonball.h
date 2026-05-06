#pragma once
#include "entities/Entity.h"
#include "ui/sprite/AnimatedSprite.h"

class Player;

class Cannonball : public Entity {
public:
    Cannonball(float x, float y, float velX, float range, Player* player);

    ~Cannonball();

    void update() override;
    void draw(QPainter& painter, float camX, float camY) override;

    bool hasExploded() const { return m_exploded; }
    bool isFinished() const { return m_finished; }

    void explode(bool hitPlayerDirectly);

    // NEW: Method to trigger the 2-second timer
    void onGroundContact();

    float getVelX() const { return m_velX; }
    float getVelY() const { return m_velY; }
    void setVelX(float v) { m_velX = v; }
    void setVelY(float v) { m_velY = v; }

private:
    AnimatedSprite* m_sprite;
    Player* m_player;
    bool m_exploded;
    bool m_finished;
    int m_explosionTimer;
    float m_baseDamage;

    float m_startX;
    float m_range;

    // NEW: Tracking ground contact and lifespan
    bool m_hasTouchedGround;
    int m_groundTimer;
};