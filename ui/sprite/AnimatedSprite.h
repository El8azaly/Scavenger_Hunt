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
    int duration;
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

    void setLoop(bool loop) { m_loop = loop; }
    bool isFinished() const { return m_finished; }
    bool isSpriteSheetNull() const { return m_spriteSheet.isNull(); }
    void forceState(const QString& stateName);
private:
    void loadData(const QString& jsonPath);

    QPixmap m_spriteSheet;
    QVector<SpriteFrame> m_frames;
    QMap<QString, AnimationTag> m_tags;

    QString m_currentState;
    int m_currentFrameIndex = 0;
    int m_elapsedTime = 0;

    bool m_loop = true;
    bool m_finished = false;
};