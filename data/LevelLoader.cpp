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

    // One collectible item
    Item key; key.id="brass_key"; key.name="Brass Key";
    key.iconPath=":/assets/images/brass_key.png";
    key.isQuestTarget=true;
    d.itemLibrary.append(key);

    EntitySpawnData itm;
    itm.type="collectible"; itm.id="key1";
    itm.x=400; itm.y=490; itm.w=36; itm.h=36;
    itm.properties["itemId"]="brass_key";
    d.entities.append(itm);

    d.targetIds << "brass_key";
    return d;
}

LevelData LevelLoader::load(int levelNumber) {
    // TODO (Milestone 2): read from ":/data/levels/levelN.json"
    if(levelNumber==1) return buildLevel1();
    return buildLevel1(); // fallback
}

LevelData LevelLoader::loadFromFile(const QString& path) {
    Q_UNUSED(path)
    return buildLevel1();
}
