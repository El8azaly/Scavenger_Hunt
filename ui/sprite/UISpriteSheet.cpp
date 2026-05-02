#include "UISpriteSheet.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

QPixmap* UISpriteSheet::s_texture = nullptr;
QHash<QString, SpriteSegment> UISpriteSheet::s_segments;

void UISpriteSheet::init(const QString& imagePath, const QString& jsonPath) {

    if (!s_texture) {
        s_texture = new QPixmap();
    }
    s_texture->load(imagePath);

    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open UI JSON:" << jsonPath;
        return;
    }

    QJsonObject root = QJsonDocument::fromJson(file.readAll()).object();
    for (auto it = root.begin(); it != root.end(); ++it) {
        QJsonObject obj = it.value().toObject();
        s_segments[it.key()] = {
            QRect(obj["x"].toInt(), obj["y"].toInt(), obj["w"].toInt(), obj["h"].toInt()),
            obj["sliceW"].toInt(),
            obj["sliceH"].toInt()
        };
    }
}

const QPixmap& UISpriteSheet::texture() {
    return *s_texture;
}

SpriteSegment UISpriteSheet::getSegment(const QString& id) {
    return s_segments.value(id);
}