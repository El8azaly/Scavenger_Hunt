#include "SpriteButton.h"
#include "SlicedSprite.h"
#include <QPainter>
#include <QMouseEvent>
#include "core/Constants.h"

SpriteButton::SpriteButton(const QString& baseStyle, QWidget* parent) 
    : QWidget(parent), m_baseStyle(baseStyle) {
    setMouseTracking(true); // Needed to detect hover states
}

void SpriteButton::setText(const QString& text, int scale, QColor color) {
    m_text = text; m_textScale = scale; m_textColor = color; update();
}

void SpriteButton::setIcon(const QString& iconName, int scale) {
    m_iconName = iconName; m_iconScale = scale; update();
}

void SpriteButton::setInteractable(bool interactable) {
    m_interactable = interactable;
    if (!interactable) { m_isHovered = false; m_isDown = false; }
    update();
}

void SpriteButton::setTextOffset(int offsetX, int offsetY) {
    m_textOffsetX = offsetX;
    m_textOffsetY = offsetY;
    update();
}

// --- PRESETS ---
SpriteButton* SpriteButton::createGreen(const QString& text, QWidget* parent) {
    auto* btn = new SpriteButton("green_button", parent);
    btn->setText(text, 2, PixelFont::Dark);
    return btn;
}

SpriteButton* SpriteButton::createYellow(const QString& text, QWidget* parent) {
    auto* btn = new SpriteButton("yellow_button", parent);
    btn->setText(text, 2, PixelFont::Dark);
    return btn;
}

SpriteButton* SpriteButton::createPaper(const QString& paperType, const QString& text, QWidget* parent) {
    auto* panel = new SpriteButton(paperType, parent);
    panel->setInteractable(false); // Paper shouldn't react to mouse clicks
    if (!text.isEmpty()) panel->setText(text, 2, PixelFont::Dark);
    return panel;
}

void SpriteButton::setButtonNudge(int offsetX, int offsetY) {
    m_nudgeX = offsetX;
    m_nudgeY = offsetY;
    update();
}

// --- NEW: Calculate True Hit Box ---
QRect SpriteButton::getHitRect() const {
    int scaledX = m_nudgeX * Constants::UI_SCALE;
    int scaledY = m_nudgeY * Constants::UI_SCALE;
    return rect().translated(scaledX, scaledY);
}

// --- MOUSE EVENTS ---
void SpriteButton::mousePressEvent(QMouseEvent* e) {
    if (m_interactable && e->button() == Qt::LeftButton) {
        m_isDown = true;
        update();
    }
}

void SpriteButton::mouseReleaseEvent(QMouseEvent* e) {
    if (m_interactable && e->button() == Qt::LeftButton) {
        m_isDown = false;
        // FIX: Check against the nudged hit rect, not the layout rect
        if (getHitRect().contains(e->pos())) emit clicked();
        update();
    }
}

void SpriteButton::mouseMoveEvent(QMouseEvent* e) {
    if (!m_interactable) return;
    // FIX: Hover should trigger on the nudged hit rect
    bool hover = getHitRect().contains(e->pos());
    if (hover != m_isHovered) { m_isHovered = hover; update(); }
}

void SpriteButton::leaveEvent(QEvent*) {
    if (m_interactable) { m_isHovered = false; m_isDown = false; update(); }
}

// --- RENDERING ---
void SpriteButton::paintEvent(QPaintEvent*) {
    QPainter painter(this);

    // NEW: Shift the entire drawing context by the nudge amount!
    int scaledNudgeX = m_nudgeX * Constants::UI_SCALE;
    int scaledNudgeY = m_nudgeY * Constants::UI_SCALE;
    painter.translate(scaledNudgeX, scaledNudgeY);

    QString activeStyle = m_baseStyle;
    if (m_interactable) {
        if (m_isDown) activeStyle += "_down";
        else if (m_isHovered) activeStyle += "_hovered";
    }

    SlicedSprite bg(activeStyle);
    bg.draw(painter, 0, 0, width(), height());

    drawContent(painter);
}

void SpriteButton::drawContent(QPainter& p) {
    // 1. Scale the 4px base margin by the global UI scale
    int margin = 4 * Constants::UI_SCALE;
    QRect contentBox = rect().adjusted(margin, margin, -margin, -margin);
    if (contentBox.isEmpty()) return;

    static PixelFont font;

    // 2. Calculate Effective Scales
    int effectiveTextScale = m_textScale * Constants::UI_SCALE;
    int effectiveIconScale = m_iconScale * Constants::UI_SCALE;

    int scaledOffsetX = m_textOffsetX * Constants::UI_SCALE;
    int scaledOffsetY = m_textOffsetY * Constants::UI_SCALE;

    // --- ADDED: Push text down 1 UI pixel when button is pressed ---
    if (m_isDown) {
        scaledOffsetY += Constants::UI_SCALE;
    }

    // Split text into wrapped lines[cite: 1]
    QStringList words = m_text.split(" ");
    QStringList lines;
    QString currentLine = "";

    for (const QString& word : words) {
        QString testLine = currentLine.isEmpty() ? word : currentLine + " " + word;
        int testWidth = testLine.length() * 6 * effectiveTextScale;

        if (testWidth > contentBox.width() && !currentLine.isEmpty()) {
            lines.append(currentLine);
            currentLine = word;
        } else {
            currentLine = testLine;
        }
    }
    if (!currentLine.isEmpty()) lines.append(currentLine);

    // Calculate total block height[cite: 1]
    int lineHeight = 6 * effectiveTextScale;
    int lineSpacing = 2 * effectiveTextScale;
    int textHeight = lines.size() * lineHeight + qMax(0, (int)lines.size() - 1) * lineSpacing;

    int iconSize = m_iconName.isEmpty() ? 0 : 8 * effectiveIconScale;
    int iconMargin = (!m_text.isEmpty() && iconSize > 0) ? 2 * effectiveTextScale : 0;

    int totalHeight = iconSize + iconMargin + textHeight;

    // Factor in our dynamic scaled offsetY (now including press-down offset)[cite: 1]
    int currentY = contentBox.y() + (contentBox.height() - totalHeight) / 2 + scaledOffsetY;

    // Draw Icon Centered[cite: 1]
    if (iconSize > 0) {
        int iconX = contentBox.x() + (contentBox.width() - iconSize) / 2 + scaledOffsetX;
        SlicedSprite(m_iconName).draw(p, iconX, currentY, iconSize, iconSize);
        currentY += iconSize + iconMargin;
    }

    // Draw Text Centered[cite: 1]
    for (const QString& line : lines) {
        int lineWidth = line.length() * 6 * effectiveTextScale;
        int lineX = contentBox.x() + (contentBox.width() - lineWidth) / 2 + scaledOffsetX;
        font.drawText(p, line, lineX, currentY, effectiveTextScale, m_textColor);
        currentY += lineHeight + lineSpacing;
    }
}