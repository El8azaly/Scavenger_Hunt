#include "data/LevelLoader.h"
#include "core/Constants.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

// ── Hardcoded Level 1 (Milestone 1 bootstrap) ─────────────────
// Replace with JSON parsing in Milestone 2.
static LevelData buildLevel1() {
    LevelData d;
    d.levelNumber  = 1;
    d.timeLimitSec = Constants::LEVEL1_TIME_SEC;
    d.worldWidth   = Constants::WORLD_WIDTH;
    d.worldHeight  = Constants::WORLD_HEIGHT;
    d.playerStartX = 100;
    d.playerStartY = 580;

    // Floor
    EntitySpawnData floor;
    floor.type="platform"; floor.id="floor";
    floor.x=0; floor.y=672; floor.w=5760; floor.h=48;
    d.entities.append(floor);

    // A platform to jump to
    EntitySpawnData plat;
    plat.type="platform"; plat.id="plat1";
    plat.x=300; plat.y=530; plat.w=200; plat.h=24;
    d.entities.append(plat);

    // One collectible item (Updated for AnimatedSprites!)
    Item key;
    key.id="brass_key";
    key.name="Brass Key";
    key.description="A shiny brass key. Wonder what it opens?";
    // UI & World representation
    key.spriteJsonPath=":/assets/sprites/items/key.json";
    key.spriteImagePath=":/assets/sprites/items/key.png";
    key.isQuestTarget=true;
    key.spriteWidth = 16.0f;   // Larger width
    key.spriteHeight = 30.0f;  // Larger height
    d.itemLibrary.append(key);

    // Spawn the collectible in the world
    EntitySpawnData itm;
    itm.type="collectible";
    itm.id="key1";
    itm.x=400;
    itm.y=480;
    itm.w=36;
    itm.h=36;
    itm.properties["itemId"] = "brass_key";
    d.entities.append(itm);

    // Target required to win the level
    d.targetIds.append("brass_key");

    return d;
}

LevelData LevelLoader::load(int levelNumber) {
    if (levelNumber == 1) {
        return buildLevel1();
    }
    return {};
}

LevelData LevelLoader::loadFromFile(const QString& filePath) {
    // Milestone 2 implementation
    return {};
}