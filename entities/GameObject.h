#pragma once
#include <QPainter>
#include <QRectF>
#include <QString>

#include "ui/sprite/AnimatedSprite.h"

class GameObject
{
public:
    explicit GameObject(float x, float y, float w, float h);

    virtual ~GameObject() { delete m_sprite; }

    virtual void update() = 0;
    virtual void draw(QPainter& painter, float camX, float camY) = 0;

    QRectF boundingBox() const;

    float x()      const { return m_x; }
    float y()      const { return m_y; }
    float width()  const { return m_w; }
    float height() const { return m_h; }
    float centreX() const { return m_x + m_w / 2.0f; }
    float centreY() const { return m_y + m_h / 2.0f; }

    void setPosition(float x, float y) { m_x = x; m_y = y; }
    void setSize(float w, float h)     { m_w = w; m_h = h; }

    const QString& id()    const { return m_id; }
    void           setId(const QString& id) { m_id = id; }

    bool isActive()        const { return m_active; }
    void setActive(bool b)       { m_active = b; }

    virtual bool isSolid()        const { return false; }
    virtual bool isInteractable() const { return false; }
    virtual bool isCollectible()  const { return false; }

protected:
    float   m_x, m_y;
    float   m_w, m_h;
    QString m_id;
    bool    m_active = true;

    AnimatedSprite* m_sprite = nullptr;
    float m_spriteOffsetX = 0;
    float m_spriteOffsetY = 0;
    float m_spriteWidth = 0;
    float m_spriteHeight = 0;

    void drawSprite(QPainter& painter, float camX, float camY, bool flipX = false);
};