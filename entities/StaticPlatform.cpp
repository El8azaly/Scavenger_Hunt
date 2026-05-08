#include "entities/StaticPlatform.h"
#include <QPainter>
StaticPlatform::StaticPlatform(float x,float y,float w,float h,const QColor& c)
    :GameObject(x,y,w,h),m_color(c){}
void StaticPlatform::draw(QPainter& painter, float camX, float camY) {
    float sx = m_x - camX;
    float sy = m_y - camY;
    QRectF rect(sx, sy, m_w, m_h);
    QLinearGradient grad(rect.topLeft(), rect.bottomLeft());
    grad.setColorAt(0.0, m_color.lighter(120));
    grad.setColorAt(1.0, m_color.darker(140));
    painter.setPen(Qt::NoPen);
    painter.setBrush(grad);
    painter.drawRoundedRect(rect, 6, 6);
    painter.setPen(QPen(m_color.darker(180), 1.5));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(rect, 6, 6);
    painter.setPen(QPen(QColor(255, 255, 255, 40), 2));
    painter.drawLine(rect.topLeft() + QPointF(3, 2), rect.topRight() + QPointF(-3, 2));
    painter.setPen(QPen(QColor(0, 0, 0, 60), 2));
    painter.drawLine(rect.bottomLeft() - QPointF(-3, 2), rect.bottomRight() - QPointF(3, 2));
}