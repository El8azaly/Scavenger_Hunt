#include "core/Level.h"
#include <QPainter>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <algorithm>

Level::Level(const QString& levelImagePath,
             const QString& collisionJsonPath,
             const QString& trapJsonPath,
             const QString& dataKey,
             std::unique_ptr<AnimatedSprite> bottomSprite,
             std::unique_ptr<AnimatedSprite> topSprite,
             QObject* parent, QColor bgColor)
    : QObject(parent),
      m_backgroundColor(bgColor),
      m_levelImagePath(levelImagePath),
      m_collisionJsonPath(collisionJsonPath),
      m_trapJsonPath(trapJsonPath),
      m_dataKey(dataKey),
      m_bottomSprite(std::move(bottomSprite)),
      m_topSprite(std::move(topSprite)) {
}

void Level::init() {
    if (!m_levelImagePath.isEmpty()) {
        m_levelImage.load(m_levelImagePath);
    }

    if (!m_collisionJsonPath.isEmpty()) {
        loadCollisionData(m_collisionJsonPath, m_dataKey);
    }

    if (!m_trapJsonPath.isEmpty()) {
        loadTrapData(m_trapJsonPath, m_dataKey);
    }
}

void Level::update(int deltaTimeMs) {
    m_animationAccumulator += deltaTimeMs;
    if (m_animationAccumulator >= 250) {
        if (m_bottomSprite) m_bottomSprite->update(m_animationAccumulator);
        if (m_topSprite) m_topSprite->update(m_animationAccumulator);
        m_animationAccumulator = 0;
    }
}

void Level::drawBackLayer(QPainter& painter, const Camera& camera) {
    if (!m_bottomSprite) return;

    float scale = Constants::GAME_SCALE;
    float yOffset = verticalOffset();

    QRectF destRect(
        camera.toScreenX(0),
        camera.toScreenY(yOffset),
        worldWidth(),
        m_levelImage.height() * scale
    );

    m_bottomSprite->draw(painter, destRect, false);
}

void Level::drawFrontLayer(QPainter& painter, const Camera& camera) {
    if (!m_topSprite) return;

    float scale = Constants::GAME_SCALE;
    float yOffset = verticalOffset();

    QRectF destRect(
        camera.toScreenX(0),
        camera.toScreenY(yOffset),
        worldWidth(),
        m_levelImage.height() * scale
    );

    m_topSprite->draw(painter, destRect, false);
}

float Level::verticalOffset() const {
    float scaledH = m_levelImage.height() * Constants::GAME_SCALE;
    float offset = (Constants::WINDOW_HEIGHT - scaledH) / 2.0f;
    return (offset > 0) ? offset : 0.0f;
}

void Level::drawLevelLayer(QPainter& painter, const Camera& camera) {
    if (m_levelImage.isNull()) return;

    float scale = Constants::GAME_SCALE;
    float scaledW = m_levelImage.width() * scale;
    float scaledH = m_levelImage.height() * scale;
    float yOffset = verticalOffset();

    QRectF worldRect(0, yOffset, scaledW, scaledH);

    QRectF view = camera.viewport();
    QRectF visibleWorldRect = view.intersected(worldRect);

    if (visibleWorldRect.isEmpty()) return;

    float srcX = (visibleWorldRect.x() - worldRect.x()) / scale;
    float srcY = (visibleWorldRect.y() - worldRect.y()) / scale;
    float srcW = visibleWorldRect.width() / scale;
    float srcH = visibleWorldRect.height() / scale;
    QRectF sourceRect(srcX, srcY, srcW, srcH);

    QRectF destRect(
        camera.toScreenX(visibleWorldRect.x()),
        camera.toScreenY(visibleWorldRect.y()),
        visibleWorldRect.width(),
        visibleWorldRect.height()
    );

    painter.drawPixmap(destRect, m_levelImage, sourceRect);
}

float Level::worldWidth() const {
    return m_levelImage.width() * Constants::GAME_SCALE;
}

float Level::worldHeight() const {
    float scaledH = m_levelImage.height() * Constants::GAME_SCALE;
    return std::max(scaledH + verticalOffset(), static_cast<float>(Constants::WINDOW_HEIGHT));
}

void Level::loadCollisionData(const QString& filepath, const QString& jsonKey) {
    m_collisionRects.clear();

    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[Level] Failed to open collision JSON:" << filepath;
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return;

    QJsonObject root = doc.object();
    if (!root.contains(jsonKey)) return;

    float scale = Constants::GAME_SCALE;
    float yOffset = verticalOffset();

    QJsonArray rectArray = root[jsonKey].toArray();
    for (const QJsonValue& val : rectArray) {
        QJsonObject obj = val.toObject();

        float x = obj["x"].toDouble() * scale;
        float y = obj["y"].toDouble() * scale + yOffset;
        float w = obj["w"].toDouble() * scale;
        float h = obj["h"].toDouble() * scale;
        m_collisionRects.append(QRectF(x, y, w, h));
    }
}

void Level::loadTrapData(const QString& filepath, const QString& jsonKey) {
    m_trapRects.clear();

    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[Level] Failed to open trap JSON:" << filepath;
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return;

    QJsonObject root = doc.object();
    if (!root.contains(jsonKey)) return;

    float scale = Constants::GAME_SCALE;
    float yOffset = verticalOffset();

    QJsonArray rectArray = root[jsonKey].toArray();
    for (const QJsonValue& val : rectArray) {
        QJsonObject obj = val.toObject();

        float x = obj["x"].toDouble() * scale;
        float y = obj["y"].toDouble() * scale + yOffset;
        float w = obj["w"].toDouble() * scale;
        float h = obj["h"].toDouble() * scale;
        m_trapRects.append(QRectF(x, y, w, h));
    }
}