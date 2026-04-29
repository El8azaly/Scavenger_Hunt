#include "systems/CraftingSystem.h"
QPair<QString,QString> CraftingSystem::makeKey(const QString& a,const QString& b){
    return a<b ? QPair<QString,QString>{a,b} : QPair<QString,QString>{b,a};
}
void CraftingSystem::loadRecipes(const QVector<CraftingRecipe>& recipes){
    m_recipes.clear();
    for(auto& r:recipes) m_recipes[makeKey(r.ingredient1,r.ingredient2)]=r.result;
}
bool CraftingSystem::canCraft(const QString& a,const QString& b) const{
    return m_recipes.contains(makeKey(a,b));
}
Item CraftingSystem::craft(const QString& a,const QString& b) const{
    return m_recipes.value(makeKey(a,b), Item::null());
}
