#pragma once
#include <QPixmap>
#include <QString>
#include <QPainter>
#include <QHash>
#include <QColor>

class PixelFont {
public:
    // Default color options
    static const QColor Dark;
    static const QColor Light;
    static const QColor Bright;

    // Hardcoded to load from your resources automatically
    PixelFont();

    // Draws text at (x, y).
    // - scale: multiplies the 6x6 size (e.g., 2 draws 12x12).
    // - color: defaults to PixelFont::Light if not specified.
    void drawText(QPainter& painter, const QString& text, int x, int y,
                  int scale = 1, const QColor& color = Light);

private:
    QPixmap m_spriteSheet;

    static const int CHAR_WIDTH = 6;
    static const int CHAR_HEIGHT = 6;
    static const int COLS = 26;

    QHash<QChar, int> m_charMap;

    void initCharMap();
};