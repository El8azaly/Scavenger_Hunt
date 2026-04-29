#include "entities/Container.h"
#include <QPainter>

Container::Container(float x,float y,float w,float h):InteractiveObject(x,y,w,h){
    m_closedPix.load(":/assets/images/chest_closed.png");
    m_openPix.load(":/assets/images/chest_open.png");
}
void Container::addItem(const Item& i){m_contents.append(i);}
InteractionResult Container::interact(){
    if(m_opened) return {InteractionResult::Type::None};
    if(m_isLocked){
        PuzzleData p; p.type=m_puzzleType; p.data=m_puzzleData; p.objectId=m_id;
        return {InteractionResult::Type::ShowPuzzle, p};
    }
    m_opened=true;
    InteractionResult r; r.type=InteractionResult::Type::RevealItems; r.revealedItems=m_contents;
    return r;
}
void Container::draw(QPainter& painter, float camX, float camY){
    float sx=m_x-camX, sy=m_y-camY;
    QPixmap& pix = m_opened ? m_openPix : m_closedPix;
    if(!pix.isNull()) painter.drawPixmap(QRectF(sx,sy,m_w,m_h), pix, pix.rect());
    else painter.fillRect(QRectF(sx,sy,m_w,m_h), m_opened?QColor(180,120,60):QColor(120,70,30));
    if(m_showHint && !m_opened){
        painter.setPen(Qt::yellow); painter.setFont(QFont("Arial",10,QFont::Bold));
        painter.drawText(QPointF(sx+m_w/2-8,sy-8),"[E]");
    }
}
