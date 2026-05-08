#include "AnimatedSprite.h"

AnimatedSprite::AnimatedSprite(const QString& jsonPath, const QString& imagePath) {
    m_spriteSheet.load(imagePath);
    loadData(jsonPath);
    if (!m_tags.isEmpty()) {
        setState(m_tags.begin().key());
    }
}

void AnimatedSprite::loadData(const QString& jsonPath) {
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "FAILED TO OPEN JSON:" << jsonPath;
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject root = doc.object();

    QJsonArray framesArr = root["frames"].toArray();

    for (const QJsonValue& v : framesArr) {
        QJsonObject fObj = v.toObject();
        QJsonObject rectObj = fObj["frame"].toObject();

        m_frames.append({
            QRect(rectObj["x"].toInt(), rectObj["y"].toInt(),
                  rectObj["w"].toInt(), rectObj["h"].toInt()),
            fObj["duration"].toInt()
        });
    }

    QJsonArray tagsArr = root["meta"].toObject()["frameTags"].toArray();
    for (const QJsonValue& v : tagsArr) {
        QJsonObject t = v.toObject();
        m_tags[t["name"].toString()] = { t["from"].toInt(), t["to"].toInt() };
    }
    if (m_tags.isEmpty() && m_frames.size() > 0) {
        m_tags["Default"] = {0, (int)m_frames.size() - 1};
    }
}
void AnimatedSprite::setState(const QString& stateName) {
    if (m_currentState == stateName) return;
    if (!m_tags.contains(stateName)) return;

    m_currentState = stateName;
    m_currentFrameIndex = m_tags[m_currentState].from;
    m_elapsedTime = 0;
    m_finished = false;
}
void AnimatedSprite::forceState(const QString& stateName) {
    if (!m_tags.contains(stateName)) return;
    m_currentState      = stateName;
    m_currentFrameIndex = m_tags[m_currentState].from;
    m_elapsedTime       = 0;
    m_finished          = false;
}
void AnimatedSprite::update(int deltaTimeMs) {
    if (m_frames.isEmpty() || !m_tags.contains(m_currentState)) return;
    if (m_finished) return;

    m_elapsedTime += deltaTimeMs;
    int duration = m_frames[m_currentFrameIndex].duration;
    if (duration <= 0) duration = 100;

    while (m_elapsedTime >= duration) {
        m_elapsedTime -= duration;
        m_currentFrameIndex++;

        if (m_currentFrameIndex > m_tags[m_currentState].to) {
            if (!m_loop) {
                m_currentFrameIndex = m_tags[m_currentState].to;
                m_finished = true;
                break;
            } else {
                m_currentFrameIndex = m_tags[m_currentState].from;
            }
        }

        duration = m_frames[m_currentFrameIndex].duration;
        if (duration <= 0) break;
    }
}

void AnimatedSprite::draw(QPainter& painter, const QRectF& destRect, bool flipX) {
    if (m_currentFrameIndex < 0 || m_currentFrameIndex >= m_frames.size()) {
        return;
    }
    QRect sourceRect = m_frames[m_currentFrameIndex].rect;

    if (flipX) {
        painter.save();
        painter.translate(destRect.left() + destRect.width(), 0);
        painter.scale(-1, 1);
        painter.drawPixmap(QRectF(0, destRect.top(), destRect.width(), destRect.height()),
                           m_spriteSheet, sourceRect);
        painter.restore();
    } else {
        painter.drawPixmap(destRect, m_spriteSheet, sourceRect);
    }
}