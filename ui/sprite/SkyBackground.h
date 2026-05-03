#pragma once
#include <QPainter>
#include <QVector>
#include <QPoint>
#include "AnimatedSprite.h"

class SkyBackground {
public:

    SkyBackground();

    ~SkyBackground();

    void update(int deltaTimeMs);
    void draw(QPainter& painter, int screenW, int screenH, float cameraX = 0.0f);

    void setCloudSpeed(float pixelsPerSecond);
    void setCloudOffsetY(int offsetY);
    void setWaterSprite(int index, int x, int y, const QString& state);

private:
    struct WaterInstance {
        AnimatedSprite* sprite = nullptr;
        QPoint pos;
    };

    static float s_cloudOffsetX;
    static float s_shipX;
    static bool s_shipInitialized;

    float m_cloudSpeed = 7.0f;
    int m_cloudOffsetY = -74;

    QVector<WaterInstance> m_waters;

    AnimatedSprite* m_shipSprite = nullptr;
    AnimatedSprite* m_waterWavesSprite = nullptr;
};