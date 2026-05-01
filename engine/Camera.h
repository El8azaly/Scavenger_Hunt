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

    float offsetX() const { return m_x; }
    float offsetY() const { return m_y; }

    float toScreenX(float worldX) const { return worldX - m_x; }
    float toScreenY(float worldY) const { return worldY - m_y; }
    QPointF toScreen(float wx, float wy) const { return {wx - m_x, wy - m_y}; }

    QRectF viewport() const;

private:
    float m_x = 0, m_y = 0;
    int   m_viewW, m_viewH;
    float m_worldW = 0, m_worldH = 0;

    float m_prevTargetX = 0;
    float m_targetLookAhead = 0;
    float m_currentLookAhead = 0;
};