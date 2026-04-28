#pragma once
#include <QString>
#include <QVector>
#include <QVariantMap>
#include "data/ItemData.h"

// ─────────────────────────────────────────────────────────────
//  LevelLoader structs — shared between core/Game.h and
//  data/LevelLoader.cpp. Defined here so Game.h can include
//  them without depending on the full LevelLoader implementation.
// ─────────────────────────────────────────────────────────────

// Describes one entity to spawn when a level is loaded.
// LevelLoader parses the JSON file and fills one of these per entity.
struct EntitySpawnData {
    QString     type;        // "platform" | "container" | "collectible"
                             // | "hidden_area"
    QString     id;          // unique id within the level
    float       x, y;        // world-space top-left position
    float       w, h;        // size in pixels
    QVariantMap properties;  // flexible key-value store:
                             //   container: locked, puzzleType, puzzleData,
                             //              contents (list of item ids)
                             //   collectible: itemId
                             //   hidden_area: requiredTriggers, itemIds
                             //   platform: solid=true (always)
};

// Crafting recipe stored per-level (level 2 might add new recipes)
struct CraftingRecipe {
    QString ingredient1; // item id
    QString ingredient2; // item id
    Item    result;      // full Item struct for the crafted product
};

// Complete data for one level — returned by LevelLoader::load()
struct LevelData {
    int     levelNumber;
    int     timeLimitSec;
    float   worldWidth;
    float   worldHeight;
    float   playerStartX;
    float   playerStartY;

    QVector<EntitySpawnData> entities;    // everything to spawn
    QStringList              targetIds;   // item ids player must collect to win
    QVector<CraftingRecipe>  recipes;     // crafting recipes active in this level
    QVector<Item>            itemLibrary; // all items referenced in this level
                                          // (id, name, icon etc.) for LevelLoader
                                          // to resolve ids to full Item structs
};

// ─────────────────────────────────────────────────────────────
//  LevelLoader — reads a level JSON file and returns LevelData
//  Implementation is in data/LevelLoader.cpp (Person 5's file)
// ─────────────────────────────────────────────────────────────
class LevelLoader
{
public:
    // Loads ":/data/levels/levelN.json" where N = levelNumber
    static LevelData load(int levelNumber);

    // Load from an explicit file path (useful for testing)
    static LevelData loadFromFile(const QString& filePath);
};
