#pragma once
#include "entities/GameObject.h"
#include <QColor>

class StaticPlatform : public GameObject {
public:
    StaticPlatform(float x,float y,float w,float h,const QColor& c=QColor(100,80,60));
    bool isSolid() const override { return true; }
    void update() override {}
    void draw(QPainter& painter, float camX, float camY) override;
private:
    QColor m_color;
};
