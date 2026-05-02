#pragma once
#include "entities/InteractiveObject.h"
#include "data/ItemData.h"
#include <QVector>

class HiddenArea : public InteractiveObject {
public:
    HiddenArea(float x, float y, float w, float h, int requiredTriggers);
    void update() override {}
    void draw(QPainter& painter, float camX, float camY) override;
    InteractionResult interact() override;
    void addHiddenItem(const Item& item);

private:
    int           m_triggerCount = 0;
    int           m_requiredTriggers;
    bool          m_revealed = false;
    QVector<Item> m_hiddenItems;
};