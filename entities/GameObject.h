#pragma once
#include <QPainter>
#include <QRectF>
#include <QString>

// ─────────────────────────────────────────────────────────────
//  GameObject  —  abstract base class for EVERYTHING in the world
//
//  Every visible, collidable, or interactive thing inherits from
//  this class. Game stores all entities as QVector<GameObject*>
//  and calls update() + draw() on each one every frame via
//  polymorphism — this is the core OOP design of the project.
//
//  RULES for subclasses:
//    • Must implement update() and draw().
//    • Must NOT include Qt UI headers (QWidget, QDialog, etc.).
//    • Must NOT know about InventorySystem, QuestSystem, etc.
//    • Should only know about its own position/state and,
//      if needed, the InputHandler.
// ─────────────────────────────────────────────────────────────
class GameObject
{
public:
    explicit GameObject(float x, float y, float w, float h);
    virtual ~GameObject() = default;

    // ── Lifecycle (called by Game each frame) ─────────────────
    virtual void update() = 0;

    // camX / camY are the Camera's current offset.
    // Subtract them from world position to get screen position:
    //   float sx = m_x - camX;
    //   float sy = m_y - camY;
    virtual void draw(QPainter& painter, float camX, float camY) = 0;

    // ── Spatial ───────────────────────────────────────────────
    QRectF boundingBox() const;

    float x()      const { return m_x; }
    float y()      const { return m_y; }
    float width()  const { return m_w; }
    float height() const { return m_h; }
    float centreX() const { return m_x + m_w / 2.0f; }
    float centreY() const { return m_y + m_h / 2.0f; }

    void setPosition(float x, float y) { m_x = x; m_y = y; }
    void setSize(float w, float h)     { m_w = w; m_h = h; }

    // ── Identity ──────────────────────────────────────────────
    const QString& id()    const { return m_id; }
    void           setId(const QString& id) { m_id = id; }

    // ── Flags ─────────────────────────────────────────────────
    bool isActive()        const { return m_active; }
    void setActive(bool b)       { m_active = b; }

    // Override to true in StaticPlatform / wall entities only.
    // CollisionEngine uses this to separate physics-solid geometry
    // from items and interactive objects.
    virtual bool isSolid()        const { return false; }

    // Override to true in InteractiveObject subclasses.
    // Game uses this to know which nearby objects to highlight
    // with an interaction hint.
    virtual bool isInteractable() const { return false; }

    // Override to true in CollectibleItem.
    // Game uses this to trigger automatic pickup on overlap.
    virtual bool isCollectible()  const { return false; }

protected:
    float   m_x, m_y; // world-space top-left corner
    float   m_w, m_h; // dimensions in pixels
    QString m_id;      // unique identifier set by LevelLoader
    bool    m_active = true; // false = skip update/draw, pending removal
};
