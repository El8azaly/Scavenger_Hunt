#pragma once
#include "entities/GameObject.h"
#include "data/ItemData.h"
struct CollisionResult;
class CollectibleItem : public GameObject {
public:
    CollectibleItem(float x, float y, const Item& item);

    void update() override;
    void draw(QPainter& painter, float camX, float camY) override;

    bool isCollectible() const override { return m_pickupDelay <= 0; }
    const Item& item() const { return m_item; }
    bool isCollected() const { return m_collected; }
    void markCollected();

    void popOut(float startX, float startY);
    void handleSolidCollision(const CollisionResult& cr);
private:
    Item    m_item;
    bool    m_collected = false;

    float   m_bobOffset = 0;
    int     m_bobTimer  = 0;

    bool    m_popping   = false;
    float   m_velX      = 0.0f;
    float   m_velY      = 0.0f;
    float   m_popStartY = 0.0f;
    int     m_pickupDelay = 0;
};