#include "Camera.h"
#include <algorithm>

Camera::Camera(int viewWidth, int viewHeight)
    : m_viewW(viewWidth), m_viewH(viewHeight)
{}

void Camera::setWorldBounds(float worldWidth, float worldHeight)
{
    m_worldW = worldWidth;
    m_worldH = worldHeight;
}

void Camera::follow(float targetCentreX, float targetCentreY)
{
    // Centre the camera on the target
    float desiredX = targetCentreX - m_viewW / 2.0f;
    float desiredY = targetCentreY - m_viewH / 2.0f;

    // Clamp so we never go past world edges (shows black otherwise)
    if (m_worldW > 0)
        desiredX = std::clamp(desiredX, 0.0f, m_worldW - m_viewW);
    if (m_worldH > 0)
        desiredY = std::clamp(desiredY, 0.0f, m_worldH - m_viewH);

    m_x = desiredX;
    m_y = desiredY;
}

QRectF Camera::viewport() const
{
    return { m_x, m_y, static_cast<float>(m_viewW), static_cast<float>(m_viewH) };
}
