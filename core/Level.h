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

    float worldWidth() const;
    float worldHeight() const;
    float verticalOffset() const;
    QVector<QRectF> getTrapRects() const { return m_trapRects; }
    QVector<QRectF> getCollisionRects() const { return m_collisionRects; }

protected:

    void loadTrapData(const QString& filepath, const QString& jsonKey);
    void loadCollisionData(const QString& filepath, const QString& jsonKey);

    QPixmap m_levelImage;
    QVector<QRectF> m_trapRects;
    QVector<QRectF> m_collisionRects;
    QColor m_backgroundColor = Qt::black;
    int m_animationAccumulator = 0;
};