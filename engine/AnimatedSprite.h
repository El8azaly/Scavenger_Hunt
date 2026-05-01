#pragma once
#include <QPixmap>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QMap>
#include <QRect>
#include <QPainter>

struct SpriteFrame {
    QRect rect;
    int duration; // Duration in milliseconds from JSON
};

struct AnimationTag {
    int from;
    int to;
};

class AnimatedSprite {
public:
    AnimatedSprite(const QString& jsonPath, const QString& imagePath);

    void update(int deltaTimeMs);
    void draw(QPainter& painter, const QRectF& destRect, bool flipX);

    void setState(const QString& stateName);
    QString currentState() const { return m_currentState; }

private:
    void loadData(const QString& jsonPath);

    QPixmap m_spriteSheet;
    QVector<SpriteFrame> m_frames;
    QMap<QString, AnimationTag> m_tags;

    QString m_currentState;
    int m_currentFrameIndex = 0;
    int m_elapsedTime = 0;
};