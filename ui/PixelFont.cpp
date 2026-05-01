#include "ui/PixelFont.h"
#include <QRect>
#include <QImage>
#include <QDebug>

// Define the static default colors
const QColor PixelFont::Dark("#33323d");
const QColor PixelFont::Light("#edc59b");

PixelFont::PixelFont() {
    QString fontPath = ":/assets/sprites/ui/font.png";

    if (!m_spriteSheet.load(fontPath)) {
        qWarning() << "CRITICAL ERROR: PixelFont failed to load sprite at:" << fontPath;
    }

    initCharMap();
}

void PixelFont::initCharMap() {
    QString row1 = "abcdefghijklmnopqrstuvwxyz";
    QString row2 = "1234567890-+×/=()#@!?.,:'$";

    for (int i = 0; i < row1.length(); ++i) { m_charMap[row1[i]] = i; }
    for (int i = 0; i < row2.length(); ++i) { m_charMap[row2[i]] = i + COLS; }

    m_charMap['*'] = m_charMap[QChar(0x00D7)];
}

void PixelFont::drawText(QPainter& painter, const QString& text, int x, int y, int scale, const QColor& color) {
    if (m_spriteSheet.isNull() || scale <= 0) return;

    QString lowerText = text.toLower();
    int currentX = x;

    for (QChar c : lowerText) {
        if (c == ' ') {
            currentX += CHAR_WIDTH * scale;
            continue;
        }

        if (m_charMap.contains(c)) {
            int index = m_charMap[c];
            int col = index % COLS;
            int row = index / COLS;

            QRect sourceRect(col * CHAR_WIDTH, row * CHAR_HEIGHT, CHAR_WIDTH, CHAR_HEIGHT);
            QRect destRect(currentX, y, CHAR_WIDTH * scale, CHAR_HEIGHT * scale);

            // 1. Convert to QImage for perfectly safe pixel-level color masking
            QImage glyph = m_spriteSheet.toImage().copy(sourceRect);
            glyph = glyph.convertToFormat(QImage::Format_ARGB32_Premultiplied);

            // 2. Tint the image
            QPainter tintPainter(&glyph);
            tintPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            tintPainter.fillRect(glyph.rect(), color);
            tintPainter.end();

            // 3. Convert back to Pixmap and draw
            painter.drawPixmap(destRect, QPixmap::fromImage(glyph));
        }

        currentX += CHAR_WIDTH * scale;
    }
}