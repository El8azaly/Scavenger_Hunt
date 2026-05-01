#pragma once
#include "entities/GameObject.h"
#include "data/ItemData.h"

class CollectibleItem : public GameObject {
public:
    CollectibleItem(float x, float y, const Item& item);

    void update() override;
    void draw(QPainter& painter, float camX, float camY) override;

    bool isCollectible() const override { return true; }
    const Item& item() const { return m_item; }
    bool isCollected() const { return m_collected; }
    void markCollected();

private:
    Item    m_item;
    bool    m_collected = false;

    // We keep the bobbing effect as it looks great on animated items too!
    float   m_bobOffset = 0;
    int     m_bobTimer  = 0;
};