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


    Item sword;
    sword.id = "sword";
    sword.name = "Captain's Sword";
    sword.description = "A trusty blade for slaying enemies.";
    sword.spriteJsonPath = ":/assets/sprites/items/sword.json";
    sword.spriteImagePath = ":/assets/sprites/items/sword.png";
    sword.spriteWidth = 14.0f;
    sword.spriteHeight = 19*2;
    d.itemLibrary.append(sword);

    EntitySpawnData swd;
    swd.type = "collectible";
    swd.id = "sword_1";
    swd.x = 200;
    swd.y = 175;
    swd.w = 36;
    swd.h = 36;
    swd.properties["itemId"] = "sword";
    d.entities.append(swd);


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

    EntitySpawnData enemy;
    enemy.type = "enemy";
    enemy.id = "fiercetooth_1";
    enemy.x = 390;
    enemy.y = 175;
    enemy.w = 32;
    enemy.h = 32;
    enemy.properties["enemyType"] = "fiercetooth";
    d.entities.append(enemy);

    return d;
}

LevelData LevelLoader::load(int levelNumber) {
    if (levelNumber == 0) return buildLevel0();
    return {};
}

LevelData LevelLoader::loadFromFile(const QString& filePath) {
    return {};
}