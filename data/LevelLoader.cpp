#include "data/LevelLoader.h"
#include "core/Constants.h"
#include "data/ItemRegistry.h"
#include "data/EntityRegistry.h"

static LevelData buildLevel0() {
    LevelData d;
    d.levelNumber  = 0;
    d.timeLimitSec = Constants::LEVEL1_TIME_SEC;
    d.playerStartX = 71;
    d.playerStartY = 193;

    d.itemLibrary = ItemRegistry::allItems().values().toVector();
    d.entities.append(EntityRegistry::spawnCaptainStar("capnstar",38, 193));
    d.entities.append(EntityRegistry::spawnCollectible("sword_1", "sword", 200, 175));
    d.entities.append(EntityRegistry::spawnEnemy("fiercetooth_1", "fiercetooth", 588, 150, "brass_key"));
    d.entities.append(EntityRegistry::spawnContainer(
        "chest_1", "chest", 280, 150,
        {"map"}
    ));

    d.entities.append(EntityRegistry::spawnContainer(
        "box_1", "box", 210, 150,
        {"diamond"}
    ));

    d.targetIds.append({"diamond", "map", "brass_key"});

    return d;
}

LevelData LevelLoader::load(int levelNumber) {
    if (levelNumber == 0) return buildLevel0();
    return {};
}

LevelData LevelLoader::loadFromFile(const QString& filePath) {
    return {};
}