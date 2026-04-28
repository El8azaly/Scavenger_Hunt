#pragma once
#include <QPointF>
#include <QRectF>
#include "core/Constants.h"

// ─────────────────────────────────────────────────────────────
//  Camera  —  creates the side-scrolling effect
//
//  The world is larger than the screen. Camera tracks the player
//  and computes an offset. Every entity's draw() call receives
//  (camX, camY) and subtracts it from its world position to get
//  its screen position:
//      screenX = worldX - camera.offsetX()
//      screenY = worldY - camera.offsetY()
//
//  Camera is clamped to world bounds so it never shows black
//  space past the level edges.
// ─────────────────────────────────────────────────────────────
class Camera
{
public:
    Camera(int viewWidth  = Constants::WINDOW_WIDTH,
           int viewHeight = Constants::WINDOW_HEIGHT);

    // Call once per frame with the player's world-space centre
    void follow(float targetCentreX, float targetCentreY);

    // Set world size so the camera doesn't scroll past edges
    void setWorldBounds(float worldWidth, float worldHeight);

    // Current top-left world position visible on screen
    float offsetX() const { return m_x; }
    float offsetY() const { return m_y; }

    // Convert a world-space point to screen-space
    float toScreenX(float worldX) const { return worldX - m_x; }
    float toScreenY(float worldY) const { return worldY - m_y; }
    QPointF toScreen(float wx, float wy) const { return {wx - m_x, wy - m_y}; }

    // The world-space rectangle currently visible on screen.
    // Used by Game to skip draw() calls on off-screen entities.
    QRectF viewport() const;

private:
    float m_x = 0, m_y = 0;       // current offset (world-space top-left)
    int   m_viewW, m_viewH;        // screen dimensions
    float m_worldW = 0, m_worldH = 0; // world boundary
};
