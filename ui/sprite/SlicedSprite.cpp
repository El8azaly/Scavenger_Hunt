#include "SlicedSprite.h"
#include "core/Constants.h"
#include <algorithm>

SlicedSprite::SlicedSprite(const QString& elementBaseName)
    : m_baseName(elementBaseName) {}

void SlicedSprite::draw(QPainter& painter, int x, int y, int targetW, int targetH) {
    SpriteSegment seg9 = UISpriteSheet::getSegment(m_baseName + "_9slice");
    SpriteSegment seg3h = UISpriteSheet::getSegment(m_baseName + "_3slice_h");
    SpriteSegment seg3v = UISpriteSheet::getSegment(m_baseName + "_3slice_v");
    SpriteSegment seg1x1 = UISpriteSheet::getSegment(m_baseName + "_1x1");

    SpriteSegment exact = UISpriteSheet::getSegment(m_baseName);
    if (!exact.isNull()) {
        draw1x1(painter, x, y, targetW, targetH, exact);
        return;
    }

    SpriteSegment baseSeg = !seg1x1.isNull() ? seg1x1 : (!seg9.isNull() ? seg9 : (!seg3h.isNull() ? seg3h : seg3v));
    if (baseSeg.isNull()) return;

    int scale = Constants::UI_SCALE;
    int scaledSliceW = baseSeg.sliceW * scale;
    int scaledSliceH = baseSeg.sliceH * scale;

    bool stretchW = targetW > scaledSliceW;
    bool stretchH = targetH > scaledSliceH;

    if (stretchW && stretchH && !seg9.isNull()) {
        draw9Slice(painter, x, y, targetW, targetH, seg9);
    }
    else if (stretchW && !stretchH && !seg3h.isNull()) {
        draw3SliceH(painter, x, y, targetW, targetH, seg3h);
    }
    else if (!stretchW && stretchH && !seg3v.isNull()) {
        draw3SliceV(painter, x, y, targetW, targetH, seg3v);
    }
    else if (!seg1x1.isNull()) {
        draw1x1(painter, x, y, targetW, targetH, seg1x1);
    }
    else if (!seg9.isNull()) {

        draw9Slice(painter, x, y, targetW, targetH, seg9);
    }
}

void SlicedSprite::draw9Slice(QPainter& p, int destX, int destY, int targetW, int targetH, const SpriteSegment& seg) {
    int sw = seg.sliceW;
    int sh = seg.sliceH;
    int sx = seg.rect.x();
    int sy = seg.rect.y();
    int scale = Constants::UI_SCALE;

    int cw = sw * scale;
    int ch = sh * scale;

    int leftW = std::min(cw, targetW / 2);
    int rightW = std::min(cw, targetW - leftW);
    int topH = std::min(ch, targetH / 2);
    int bottomH = std::min(ch, targetH - topH);

    int midW = std::max(0, targetW - leftW - rightW);
    int midH = std::max(0, targetH - topH - bottomH);

    const QPixmap& tex = UISpriteSheet::texture();

    auto drawZone = [&](int srcCol, int srcRow, int dx, int dy, int dw, int dh) {
        if (dw <= 0 || dh <= 0) return;
        QRect srcRect(sx + srcCol * sw, sy + srcRow * sh, sw, sh);
        QRect destRect(dx, dy, dw, dh);

        p.save();
        p.setClipRect(destRect);

        for (int ty = dy; ty < dy + dh; ty += ch) {
            for (int tx = dx; tx < dx + dw; tx += cw) {
                p.drawPixmap(tx, ty, cw, ch, tex, srcRect.x(), srcRect.y(), sw, sh);
            }
        }
        p.restore();
    };

    drawZone(0, 0, destX, destY, leftW, topH);
    drawZone(1, 0, destX + leftW, destY, midW, topH);
    drawZone(2, 0, destX + leftW + midW, destY, rightW, topH);

    drawZone(0, 1, destX, destY + topH, leftW, midH);
    drawZone(1, 1, destX + leftW, destY + topH, midW, midH);
    drawZone(2, 1, destX + leftW + midW, destY + topH, rightW, midH);

    drawZone(0, 2, destX, destY + topH + midH, leftW, bottomH);
    drawZone(1, 2, destX + leftW, destY + topH + midH, midW, bottomH);
    drawZone(2, 2, destX + leftW + midW, destY + topH + midH, rightW, bottomH);
}

void SlicedSprite::draw3SliceH(QPainter& p, int destX, int destY, int targetW, int targetH, const SpriteSegment& seg) {
    int sw = seg.sliceW;
    int sh = seg.sliceH;
    int sx = seg.rect.x();
    int sy = seg.rect.y();
    int scale = Constants::UI_SCALE;

    int cw = sw * scale;
    int ch = sh * scale;

    int leftW = std::min(cw, targetW / 2);
    int rightW = std::min(cw, targetW - leftW);
    int midW = std::max(0, targetW - leftW - rightW);

    const QPixmap& tex = UISpriteSheet::texture();

    auto drawZone = [&](int srcCol, int dx, int dw) {
        if (dw <= 0 || targetH <= 0) return;
        QRect srcRect(sx + srcCol * sw, sy, sw, sh);
        QRect destRect(dx, destY, dw, targetH);

        p.save();
        p.setClipRect(destRect);
        for (int ty = destY; ty < destY + targetH; ty += ch) {
            for (int tx = dx; tx < dx + dw; tx += cw) {
                p.drawPixmap(tx, ty, cw, ch, tex, srcRect.x(), srcRect.y(), sw, sh);
            }
        }
        p.restore();
    };

    drawZone(0, destX, leftW);
    drawZone(1, destX + leftW, midW);
    drawZone(2, destX + leftW + midW, rightW);
}

void SlicedSprite::draw3SliceV(QPainter& p, int destX, int destY, int targetW, int targetH, const SpriteSegment& seg) {
    int sw = seg.sliceW;
    int sh = seg.sliceH;
    int sx = seg.rect.x();
    int sy = seg.rect.y();
    int scale = Constants::UI_SCALE;

    int cw = sw * scale;
    int ch = sh * scale;

    int topH = std::min(ch, targetH / 2);
    int bottomH = std::min(ch, targetH - topH);
    int midH = std::max(0, targetH - topH - bottomH);

    const QPixmap& tex = UISpriteSheet::texture();

    auto drawZone = [&](int srcRow, int dy, int dh) {
        if (dh <= 0 || targetW <= 0) return;
        QRect srcRect(sx, sy + srcRow * sh, sw, sh);
        QRect destRect(destX, dy, targetW, dh);

        p.save();
        p.setClipRect(destRect);
        for (int ty = dy; ty < dy + dh; ty += ch) {
            for (int tx = destX; tx < destX + targetW; tx += cw) {
                p.drawPixmap(tx, ty, cw, ch, tex, srcRect.x(), srcRect.y(), sw, sh);
            }
        }
        p.restore();
    };

    drawZone(0, destY, topH);
    drawZone(1, destY + topH, midH);
    drawZone(2, destY + topH + midH, bottomH);
}

void SlicedSprite::draw1x1(QPainter &p, int destX, int destY, int targetW, int targetH, const SpriteSegment &seg) {
    if (seg.isNull()) return;

    p.drawPixmap(QRect(destX, destY, targetW, targetH), UISpriteSheet::texture(), seg.rect);
}