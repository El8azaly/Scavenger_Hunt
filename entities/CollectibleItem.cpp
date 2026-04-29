#include "entities/CollectibleItem.h"
#include <QPainter>
#include <cmath>

CollectibleItem::CollectibleItem(float x,float y,const Item& item)
    :GameObject(x,y,36,36), m_item(item){ m_icon.load(item.iconPath); }

void CollectibleItem::update(){
    m_bobOffset = 5.0f * std::sin(m_bobTimer++ * 0.08f);
}
void CollectibleItem::markCollected(){ m_collected=true; setActive(false); }
void CollectibleItem::draw(QPainter& painter,float camX,float camY){
    float sx=m_x-camX, sy=m_y-camY+m_bobOffset;
    if(!m_icon.isNull()) painter.drawPixmap(QRectF(sx,sy,m_w,m_h),m_icon,m_icon.rect());
    else { painter.fillRect(QRectF(sx,sy,m_w,m_h),QColor(255,220,0));
           painter.setPen(Qt::black); painter.drawRect(QRectF(sx,sy,m_w,m_h)); }
}
