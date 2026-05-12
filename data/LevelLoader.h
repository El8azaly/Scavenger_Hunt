#pragma once
#include <QString>
#include <QVector>
#include <QVariantMap>
#include "data/ItemData.h"
class Level;
class Game;
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
struct LevelRegistryEntry {
    std::function<LevelData()> dataBuilder;
    std::function<std::unique_ptr<Level>(Game*)> instanceCreator;
};
class LevelLoader {
public:

    static LevelData load(int levelNumber);
    static QVector<int> getRegisteredLevels();
    static std::unique_ptr<Level> createLevelInstance(int levelNumber, Game* game);
private:
    static const std::map<int, LevelRegistryEntry>& getRegistry();
};