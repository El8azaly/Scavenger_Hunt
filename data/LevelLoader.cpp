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
    d.playerStartX = 50;
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

    d.targetIds.append("brass_key");

    EntitySpawnData enemy;
    enemy.type = "enemy";
    enemy.id = "fiercetooth_1";
    enemy.x = 390;
    enemy.y = 175;
    enemy.w = 32;
    enemy.h = 32;
    enemy.properties["enemyType"] = "fiercetooth";
    enemy.properties["heldItemId"] = "brass_key";
    d.entities.append(enemy);
    // Add a coin to our library so things can drop it
    Item coin;
    coin.id = "gold_coin";
    coin.name = "Gold Coin";
    coin.description = "A shiny gold coin.";
    coin.spriteJsonPath = ":/assets/sprites/items/gold_coin.json";
    coin.spriteImagePath = ":/assets/sprites/items/gold_coin.png";
    coin.spriteWidth = 16.0f*2;
    coin.spriteHeight = 16.0f*2;
    d.itemLibrary.append(coin);

    // 1. Spawning a Chest
    EntitySpawnData chest;
    chest.type = "container";
    chest.id = "chest_1";
    chest.x = 460;
    chest.y = 150;
    chest.w = 39;
    chest.h =  64;
    chest.properties["containerType"] = "chest";
    // Define items the chest will drop
    chest.properties["contents"] = QStringList() << "sword" << "gold_coin" << "gold_coin";
    d.entities.append(chest);

    // 2. Spawning a Barrel
    EntitySpawnData barrel;
    barrel.type = "container";
    barrel.id = "barrel_1";
    barrel.x = 280;
    barrel.y = 150;
    barrel.w = 39;
    barrel.h =  64;
    barrel.properties["containerType"] = "barrel";
    // Define items the barrel will drop
    barrel.properties["contents"] = QStringList() << "brass_key" << "gold_coin";
    d.entities.append(barrel);

    // 3. Spawning a Box
    EntitySpawnData box;
    box.type = "container";
    box.id = "box_1";
    box.x = 210;
    box.y = 150;
    box.w = 39;
    box.h =  64;
    box.properties["containerType"] = "box";
    // Define items the box will drop
    box.properties["contents"] = QStringList() << "gold_coin" << "gold_coin" << "gold_coin";
    d.entities.append(box);

    return d;
}
LevelData LevelLoader::load(int levelNumber) {
    if (levelNumber == 0) return buildLevel0();
    return {};
}

LevelData LevelLoader::loadFromFile(const QString& filePath) {
    return {};
}