#include "entities/InteractiveObject.h"
InteractiveObject::InteractiveObject(float x,float y,float w,float h):GameObject(x,y,w,h){}
void InteractiveObject::setLocked(const QString& t,const QStringList& d){m_isLocked=true;m_puzzleType=t;m_puzzleData=d;}
void InteractiveObject::unlock(){m_isLocked=false;}
