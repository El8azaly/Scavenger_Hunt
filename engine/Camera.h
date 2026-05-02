#pragma once
#include <QPointF>
#include <QRectF>
#include "core/Constants.h"

class Camera
{
public:
    Camera(int viewWidth  = Constants::WINDOW_WIDTH,
           int viewHeight = Constants::WINDOW_HEIGHT);

    void follow(float targetCentreX, float targetCentreY);
    void snapTo(float targetCentreX, float targetCentreY);

    void setWorldBounds(float worldWidth, float worldHeight);

    void addShake(int durationFrames, float intensity);
    void updateShake();

    float offsetX() const { return m_x + m_shakeOffsetX; }
    float offsetY() const { return m_y + m_shakeOffsetY; }

    float toScreenX(float worldX) const { return worldX - (m_x + m_shakeOffsetX); }
    float toScreenY(float worldY) const { return worldY - (m_y + m_shakeOffsetY); }
    QPointF toScreen(float wx, float wy) const { return {wx - (m_x + m_shakeOffsetX), wy - (m_y + m_shakeOffsetY)}; }

    QRectF viewport() const;

private:
    float m_x = 0, m_y = 0;
    int   m_viewW, m_viewH;
    float m_worldW = 0, m_worldH = 0;

    float m_prevTargetX = 0;
    float m_targetLookAhead = 0;
    float m_currentLookAhead = 0;

    int m_shakeTimer = 0;
    float m_shakeIntensity = 0.0f;
    float m_shakeOffsetX = 0.0f;
    float m_shakeOffsetY = 0.0f;
};