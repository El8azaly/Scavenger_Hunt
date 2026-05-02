#pragma once
#include <QWidget>
#include <QString>
#include "PixelFont.h"

class SpriteButton : public QWidget {
    Q_OBJECT
public:

    SpriteButton(const QString& baseStyle, QWidget* parent = nullptr);

    void setText(const QString& text, int scale = 2, QColor color = PixelFont::Dark);
    void setIcon(const QString& iconName, int scale = 2);
    void setInteractable(bool interactable);

    void setTextOffset(int offsetX, int offsetY);
    void setButtonNudge(int offsetX, int offsetY);

    QRect getHitRect() const;

    static SpriteButton* createGreen(const QString& text, QWidget* parent = nullptr);
    static SpriteButton* createYellow(const QString& text, QWidget* parent = nullptr);
    static SpriteButton* createPaper(const QString& paperType, const QString& text = "", QWidget* parent = nullptr);

    signals:
        void clicked();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    QString m_baseStyle;
    QString m_text;
    int m_textScale = 2;
    QColor m_textColor = PixelFont::Dark;

    int m_textOffsetX = 0;
    int m_textOffsetY = 0;

    QString m_iconName;
    int m_iconScale = 2;

    bool m_interactable = true;
    bool m_isHovered = false;
    bool m_isDown = false;
    int m_nudgeX = 0;
    int m_nudgeY = 0;
    void drawContent(QPainter& p);
};