#include "systems/InventorySystem.h"
InventorySystem::InventorySystem(QObject* p):QObject(p){}
void InventorySystem::addItem(const Item& item){
    m_items.append(item); emit itemAdded(item); emit inventoryChanged();
}
bool InventorySystem::removeItem(const QString& id){
    for(int i=0;i<m_items.size();i++) if(m_items[i].id==id){
        m_items.removeAt(i); emit itemRemoved(id); emit inventoryChanged(); return true;
    } return false;
}
bool InventorySystem::hasItem(const QString& id) const{
    for(auto& it:m_items) if(it.id==id) return true; return false;
}
Item InventorySystem::getItem(const QString& id) const{
    for(auto& it:m_items) if(it.id==id) return it; return Item::null();
}
void InventorySystem::clear(){ m_items.clear(); emit inventoryChanged(); }
