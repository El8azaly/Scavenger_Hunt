#pragma once
#include <QObject>
#include <QPixmap>
#include <QVector>
#include <QRectF>
#include <QString>
#include <QColor>
#include <memory>
#include "engine/Camera.h"
#include "ui/sprite/AnimatedSprite.h"

class QPainter;
const QColor DEFAULT_COLOR = QColor(30, 30, 45);
class Level : public QObject {
    Q_OBJECT
public:
    Level(const QString& levelImagePath,
          const QString& collisionJsonPath,
          const QString& trapJsonPath,
          const QString& dataKey,
          std::unique_ptr<AnimatedSprite> bottomSprite,
          std::unique_ptr<AnimatedSprite> topSprite,
          QObject* parent = nullptr,
          QColor bgColor = DEFAULT_COLOR);

    virtual ~Level() = default;

    virtual void init();
    virtual void update(int deltaTimeMs);

    virtual void drawBackLayer(QPainter& painter, const Camera& camera);
    virtual void drawLevelLayer(QPainter& painter, const Camera& camera);
    virtual void drawMiddleLayer(QPainter& painter, const Camera& camera) {}
    virtual void drawFrontLayer(QPainter& painter, const Camera& camera);

    float worldWidth() const;
    float worldHeight() const;
    float verticalOffset() const;

    QVector<QRectF> getTrapRects() const { return m_trapRects; }
    QVector<QRectF> getCollisionRects() const { return m_collisionRects; }

protected:
    void loadTrapData(const QString& filepath, const QString& jsonKey);
    void loadCollisionData(const QString& filepath, const QString& jsonKey);

    QString m_levelImagePath;
    QString m_collisionJsonPath;
    QString m_trapJsonPath;
    QString m_dataKey;

    QColor m_backgroundColor = Qt::black;
    QPixmap m_levelImage;
    QVector<QRectF> m_trapRects;
    QVector<QRectF> m_collisionRects;
    int m_animationAccumulator = 0;

    std::unique_ptr<AnimatedSprite> m_bottomSprite;
    std::unique_ptr<AnimatedSprite> m_topSprite;
};