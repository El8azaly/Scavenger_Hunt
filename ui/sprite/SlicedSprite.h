#pragma once
#include "UISpriteSheet.h"
#include <QPainter>

class SlicedSprite {
public:

    explicit SlicedSprite(const QString& elementBaseName);

    void draw(QPainter& painter, int x, int y, int targetW, int targetH);

private:
    QString m_baseName;

    void draw9Slice(QPainter& painter, int x, int y, int targetW, int targetH, const SpriteSegment& seg);
    void draw3SliceH(QPainter& painter, int x, int y, int targetW, int targetH, const SpriteSegment& seg);
    void draw3SliceV(QPainter& painter, int x, int y, int targetW, int targetH, const SpriteSegment& seg);
    void draw1x1(QPainter& painter, int destX, int destY, int targetW, int targetH, const SpriteSegment& seg);
};