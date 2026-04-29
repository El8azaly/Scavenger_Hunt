#include "entities/HiddenArea.h"
#include <QPainter>
HiddenArea::HiddenArea(float x,float y,float w,float h,int req)
    :InteractiveObject(x,y,w,h),m_requiredTriggers(req){}
void HiddenArea::addHiddenItem(const Item& i){m_hiddenItems.append(i);}
InteractionResult HiddenArea::interact(){
    if(m_revealed) return {InteractionResult::Type::None};
    if(++m_triggerCount>=m_requiredTriggers){
        m_revealed=true;
        InteractionResult r; r.type=InteractionResult::Type::RevealArea; r.revealedItems=m_hiddenItems; return r;
    }
    return {InteractionResult::Type::None};
}
void HiddenArea::draw(QPainter& painter,float camX,float camY){
    if(!m_revealed) return;
    float sx=m_x-camX,sy=m_y-camY;
    painter.fillRect(QRectF(sx,sy,m_w,m_h),QColor(80,200,120,80));
}
