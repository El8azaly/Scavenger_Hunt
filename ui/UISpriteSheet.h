#pragma once
#include <QPixmap>
#include <QHash>
#include <QRect>
#include <QString>

struct SpriteSegment {
    QRect rect;
    int sliceW;
    int sliceH;
    bool isNull() const { return rect.isEmpty(); }
};

class UISpriteSheet {
public:
    static void init(const QString& imagePath, const QString& jsonPath);
    static const QPixmap& texture();
    static SpriteSegment getSegment(const QString& id);

private:
    // FIX: Change to a pointer so it doesn't initialize before main()
    static QPixmap* s_texture;
    static QHash<QString, SpriteSegment> s_segments;
};