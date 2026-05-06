#include "engine/Camera.h"
#include <algorithm>
#include <cstdlib>

Camera::Camera(int viewWidth, int viewHeight) : m_viewW(viewWidth), m_viewH(viewHeight) {}

void Camera::setWorldBounds(float worldWidth, float worldHeight) {
    m_worldW = worldWidth;
    m_worldH = worldHeight;
}

void Camera::follow(float targetCentreX, float targetCentreY) {
    constexpr float LOOKAHEAD_DIST   = 100.0f;
    constexpr float LOOKAHEAD_SPEED  = 0.02f;
    constexpr float CAMERA_SPEED_X   = 0.03f;
    constexpr float CAMERA_SPEED_Y   = 0.05f;

    float moveDeltaX = targetCentreX - m_prevTargetX;
    m_prevTargetX = targetCentreX;

    if (moveDeltaX > 0.5f) {
        m_targetLookAhead = LOOKAHEAD_DIST;
    } else if (moveDeltaX < -0.5f) {
        m_targetLookAhead = -LOOKAHEAD_DIST;
    }

    m_currentLookAhead += (m_targetLookAhead - m_currentLookAhead) * LOOKAHEAD_SPEED;

    float desiredX = (targetCentreX + m_currentLookAhead) - m_viewW / 2.0f;
    float desiredY = targetCentreY - m_viewH / 2.0f;

    if (m_worldW > 0)
        desiredX = std::clamp(desiredX, 0.0f, m_worldW - m_viewW);
    if (m_worldH > 0)
        desiredY = std::clamp(desiredY, 0.0f, m_worldH - m_viewH);

    m_x += (desiredX - m_x) * CAMERA_SPEED_X;
    m_y += (desiredY - m_y) * CAMERA_SPEED_Y;
}

void Camera::snapTo(float targetCentreX, float targetCentreY) {
    m_prevTargetX = targetCentreX;
    m_targetLookAhead = 0;
    m_currentLookAhead = 0;

    float desiredX = targetCentreX - m_viewW / 2.0f;
    float desiredY = targetCentreY - m_viewH / 2.0f;

    if (m_worldW > 0) desiredX = std::clamp(desiredX, 0.0f, m_worldW - m_viewW);
    if (m_worldH > 0) desiredY = std::clamp(desiredY, 0.0f, m_worldH - m_viewH);

    m_x = desiredX;
    m_y = desiredY;
}

void Camera::addShake(int durationFrames, float intensity) {
    m_shakeTimer = durationFrames;
    m_shakeIntensity = intensity;
}

void Camera::updateShake() {
    if (m_shakeTimer > 0) {
        m_shakeTimer--;
        float rawX = ((std::rand() % 100) / 50.0f - 1.0f) * m_shakeIntensity;
        float rawY = ((std::rand() % 100) / 50.0f - 1.0f) * m_shakeIntensity;
        if (m_worldW > 0) {
            float minShakeX = -m_x;
            float maxShakeX = (m_worldW - m_viewW) - m_x;
            m_shakeOffsetX = std::clamp(rawX, minShakeX, maxShakeX);
        } else {
            m_shakeOffsetX = rawX;
        }
        if (m_worldH > 0) {
            float minShakeY = -m_y;
            float maxShakeY = (m_worldH - m_viewH) - m_y;
            m_shakeOffsetY = std::clamp(rawY, minShakeY, maxShakeY);
        } else {
            m_shakeOffsetY = rawY;
        }

    } else {
        m_shakeOffsetX = 0.0f;
        m_shakeOffsetY = 0.0f;
    }
}

QRectF Camera::viewport() const {
    return { m_x + m_shakeOffsetX, m_y + m_shakeOffsetY,
             static_cast<float>(m_viewW), static_cast<float>(m_viewH) };
}