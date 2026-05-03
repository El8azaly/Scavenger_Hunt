#pragma once
#include <QString>
#include <QVector>
#include <QVariantMap>
#include "data/ItemData.h"

struct EntitySpawnData {
    QString     type;

    QString     id;
    float       x, y;
    float       w, h;
    QVariantMap properties;

};

struct CraftingRecipe {
    QString ingredient1;
    QString ingredient2;
    Item    result;
};

struct LevelData {
    int     levelNumber;
    int     timeLimitSec;
    float   worldWidth;
    float   worldHeight;
    float   playerStartX;
    float   playerStartY;

    QVector<EntitySpawnData> entities;
    QStringList              targetIds;
    QVector<CraftingRecipe>  recipes;
    QVector<Item>            itemLibrary;
};

class LevelLoader
{
public:

    static LevelData load(int levelNumber);
    static QVector<int> getRegisteredLevels();
    static LevelData loadFromFile(const QString& filePath);
};