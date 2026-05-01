#include "data/LevelLoader.h"
#include "core/Constants.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

// ── Level 0 (Uses JSON map collision) ────────────────────────
static LevelData buildLevel0() {
    LevelData d;
    d.levelNumber  = 0;
    d.timeLimitSec = Constants::LEVEL1_TIME_SEC;
    d.playerStartX = 50;  // Adjust based on your Level 0 layout
    d.playerStartY = 50;

    // NOTE: We do not add "floor" or "platforms" here!
    // Game.cpp will automatically generate them from level0_collision_data.json.

    Item key;
    key.id="brass_key";
    key.name="Brass Key";
    key.description="A shiny brass key. Wonder what it opens?";
    key.spriteJsonPath=":/assets/sprites/items/key.json";
    key.spriteImagePath=":/assets/sprites/items/key.png";
    key.isQuestTarget=true;
    key.spriteWidth = 16.0f;
    key.spriteHeight = 30.0f;
    d.itemLibrary.append(key);

    EntitySpawnData itm;
    itm.type="collectible";
    itm.id="key1";
    itm.x=400; // Put it somewhere reachable on your map
    itm.y=180;
    itm.w=36;
    itm.h=36;
    itm.properties["itemId"] = "brass_key";
    d.entities.append(itm);

    d.targetIds.append("brass_key");
    return d;
}

LevelData LevelLoader::load(int levelNumber) {
    if (levelNumber == 0) return buildLevel0();
    return {};
}

LevelData LevelLoader::loadFromFile(const QString& filePath) {
    return {};
}