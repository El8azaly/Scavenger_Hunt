#pragma once
#include <QMap>
#include <QPair>
#include <QVector>
#include "data/ItemData.h"
#include "data/LevelLoader.h"

class CraftingSystem {
public:
    void loadRecipes(const QVector<CraftingRecipe>& recipes);
    bool canCraft(const QString& id1, const QString& id2) const;
    Item craft(const QString& id1, const QString& id2) const;
    static QPair<QString,QString> makeKey(const QString& a, const QString& b);
private:
    QMap<QPair<QString,QString>, Item> m_recipes;
};
