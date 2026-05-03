#pragma once
#include <QPixmap>
#include <QString>
#include <QPainter>
#include <QHash>
#include <QColor>

class PixelFont {
public:

    static const QColor Dark;
    static const QColor Light;
    static const QColor Bright;

    PixelFont();

    void drawText(QPainter& painter, const QString& text, int x, int y,
                  int scale = 1, const QColor& color = Light, bool isBig = false);

private:
    QPixmap m_spriteSheet;
    QPixmap m_bigSpriteSheet;

    static const int CHAR_WIDTH = 6;
    static const int CHAR_HEIGHT = 6;

    static const int BIG_CHAR_WIDTH = 11;
    static const int BIG_CHAR_HEIGHT = 11;

    static const int COLS = 26;

    QHash<QChar, int> m_charMap;
    QHash<QChar, int> m_bigCharMap;

    void initCharMap();
    void initBigCharMap();
};