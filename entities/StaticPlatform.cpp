#include "entities/StaticPlatform.h"
#include <QPainter>
StaticPlatform::StaticPlatform(float x,float y,float w,float h,const QColor& c)
    :GameObject(x,y,w,h),m_color(c){}
void StaticPlatform::draw(QPainter& painter,float camX,float camY){
    float sx=m_x-camX,sy=m_y-camY;
    painter.fillRect(QRectF(sx,sy,m_w,m_h),m_color);
    painter.setPen(QColor(60,40,20)); painter.drawRect(QRectF(sx,sy,m_w,m_h));
}
