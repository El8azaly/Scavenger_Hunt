#pragma once
#include "entities/InteractiveObject.h"
#include "data/ItemData.h"
#include <QVector>

class Container : public InteractiveObject {
public:
    Container(float x, float y, float w, float h);

    void update() override;
    void draw(QPainter& painter, float camX, float camY) override;
    InteractionResult interact() override;

    void addItem(const Item& item);

private:
    QVector<Item> m_contents;
    bool          m_opened = false;
};