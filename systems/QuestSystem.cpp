#include "systems/QuestSystem.h"
QuestSystem::QuestSystem(InventorySystem* inv, QObject* p):QObject(p){
    connect(inv,&InventorySystem::itemAdded,this,&QuestSystem::onItemAdded);
}
void QuestSystem::setTargets(const QStringList& ids){
    m_targetIds=QSet<QString>(ids.begin(),ids.end()); m_foundIds.clear();
}
QStringList QuestSystem::remainingTargets() const{
    QStringList r; for(auto& id:m_targetIds) if(!m_foundIds.contains(id)) r<<id; return r;
}
void QuestSystem::onItemAdded(const Item& item){
    if(m_targetIds.contains(item.id)&&!m_foundIds.contains(item.id)){
        m_foundIds.insert(item.id); emit targetFound(item.id);
        if(isComplete()) emit allTargetsFound();
    }
}