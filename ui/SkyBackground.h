#pragma once
#include <QPainter>
#include <QVector>
#include <QPoint>
#include "engine/AnimatedSprite.h"

class SkyBackground {
public:

    SkyBackground();

    ~SkyBackground();

    void update(int deltaTimeMs);
    void draw(QPainter& painter, int screenW, int screenH);

    void setCloudSpeed(float pixelsPerSecond);
    void setCloudOffsetY(int offsetY);
    void setWaterSprite(int index, int x, int y, const QString& state);

private:
    struct WaterInstance {
        AnimatedSprite* sprite = nullptr;
        QPoint pos;
    };
    float m_cloudOffsetX = 0.0f;
    float m_cloudSpeed = 7.0f;
    int m_cloudOffsetY = -74;

    QVector<WaterInstance> m_waters;
};