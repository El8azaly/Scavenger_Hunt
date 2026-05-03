#include "PixelFont.h"
#include <QRect>
#include <QImage>
#include <QDebug>

const QColor PixelFont::Dark("#33323d");
const QColor PixelFont::Light("#edc59b");
const QColor PixelFont::Bright("#f6f5fa");

PixelFont::PixelFont() {
    QString fontPath = ":/assets/sprites/ui/font.png";
    QString bigFontPath = ":/assets/sprites/ui/big_font.png";

    if (!m_spriteSheet.load(fontPath)) {
        qWarning() << "CRITICAL ERROR: PixelFont failed to load sprite at:" << fontPath;
    }

    if (!m_bigSpriteSheet.load(bigFontPath)) {
        qWarning() << "CRITICAL ERROR: PixelFont failed to load big font sprite at:" << bigFontPath;
    }

    initCharMap();
    initBigCharMap();
}

void PixelFont::initCharMap() {
    QString row1 = "abcdefghijklmnopqrstuvwxyz";
    QString row2 = "1234567890-+×/=()#@!?.,:'$";

    for (int i = 0; i < row1.length(); ++i) { m_charMap[row1[i]] = i; }
    for (int i = 0; i < row2.length(); ++i) { m_charMap[row2[i]] = i + COLS; }

    m_charMap['*'] = m_charMap[QChar(0x00D7)];
}

void PixelFont::initBigCharMap() {
    QString row1 = "abcdefghijklmnopqrstuvwxyz";
    QString row2 = "1234567890";

    for (int i = 0; i < row1.length(); ++i) { m_bigCharMap[row1[i]] = i; }
    for (int i = 0; i < row2.length(); ++i) { m_bigCharMap[row2[i]] = i + COLS; }
}

void PixelFont::drawText(QPainter& painter, const QString& text, int x, int y, int scale, const QColor& color, bool isBig) {
    QPixmap& currentSheet = isBig ? m_bigSpriteSheet : m_spriteSheet;
    if (currentSheet.isNull() || scale <= 0) return;

    QString lowerText = text.toLower();
    int currentX = x;

    int charW = isBig ? BIG_CHAR_WIDTH : CHAR_WIDTH;
    int charH = isBig ? BIG_CHAR_HEIGHT : CHAR_HEIGHT;
    QHash<QChar, int>& currentMap = isBig ? m_bigCharMap : m_charMap;
    for (QChar c : lowerText) {
        if (c == ' ') {
            currentX += charW * scale;
            continue;
        }
        if (currentMap.contains(c)) {
            int index = currentMap[c];
            int col = index % COLS;
            int row = index / COLS;
            QRect sourceRect(col * charW, row * charH, charW, charH);
            QRect destRect(currentX, y, charW * scale, charH * scale);
            QImage glyph = currentSheet.toImage().copy(sourceRect);
            if (!isBig) {
                glyph = glyph.convertToFormat(QImage::Format_ARGB32_Premultiplied);
                QPainter tintPainter(&glyph);
                tintPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
                tintPainter.fillRect(glyph.rect(), color);
                tintPainter.end();
            }
            painter.drawPixmap(destRect, QPixmap::fromImage(glyph));
        }
        currentX += charW * scale;
    }
}