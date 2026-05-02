#pragma once
#include <QObject>
#include <QPixmap>
#include <QVector>
#include <QRectF>
#include <QString>
#include <QColor>
#include "engine/Camera.h"
#include "core/Constants.h"

class QPainter;

class Level : public QObject
{
    Q_OBJECT
public:
    explicit Level(QObject* parent = nullptr);
    virtual ~Level() = default;

    virtual void init() = 0;
    virtual void update(int deltaTimeMs) {}

    virtual void drawBackLayer(QPainter& painter, const Camera& camera) {}
    virtual void drawLevelLayer(QPainter& painter, const Camera& camera);
    virtual void drawMiddleLayer(QPainter& painter, const Camera& camera) {}
    virtual void drawFrontLayer(QPainter& painter, const Camera& camera) {}

    // Accessors
    float worldWidth() const;
    float worldHeight() const;
    float verticalOffset() const; // Used to center the level image vertically

    QVector<QRectF> getCollisionRects() const { return m_collisionRects; }

protected:
    void loadCollisionData(const QString& filepath, const QString& jsonKey);

    QPixmap m_levelImage;
    QVector<QRectF> m_collisionRects;
    QColor m_backgroundColor = Qt::black; // Solid color for out-of-bounds
    int m_animationAccumulator = 0; // Tracks time until 100ms is reached
};