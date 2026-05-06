#include "data/LevelLoader.h"
#include "core/Constants.h"
#include "data/ItemRegistry.h"
#include "data/EntityRegistry.h"
#include "levels/Level0.h"
#include "levels/Level1.h"
#include "core/Game.h"
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

static LevelData buildLevel1() {
    LevelData d;
    d.levelNumber  = 1;
    d.timeLimitSec = Constants::LEVEL1_TIME_SEC;
    d.playerStartX = 71;
    d.playerStartY = 193;

    d.itemLibrary = ItemRegistry::allItems().values().toVector();
    d.entities.append(EntityRegistry::spawnCaptainStar("capnstar",158, 193 ));
    d.entities.append(EntityRegistry::spawnCollectible("sword_1", "sword", 286, 196));

    // Existing fierce tooth enemy
    d.entities.append(EntityRegistry::spawnEnemy("fiercetooth_1", "fiercetooth", 668, 104, "skull"));

    // NEW: Spawn the Cannon on the right side of the level to shoot horizontally at the player
    // It uses the default parameters for hitboxes and projectile spawns
    d.entities.append(EntityRegistry::spawnCannon("cannon_1", 493, 151));

    d.entities.append(EntityRegistry::spawnContainer(
        "chest_1", "chest", 558, 193,
        {"blue_potion"}
    ));

    d.entities.append(EntityRegistry::spawnContainer(
        "barrel_1", "barrel", 92, 22,
        {"ruby"}
    ));

    d.targetIds.append({"blue_potion", "ruby", "skull"});

    return d;
}
const std::map<int, LevelRegistryEntry>& LevelLoader::getRegistry() {
    static const std::map<int, LevelRegistryEntry> registry = {
        {0, { buildLevel0, [](Game* g) { return std::make_unique<Level0>(g); } }},
        {1, { buildLevel1, [](Game* g) { return std::make_unique<Level1>(g); } }}
    };
    return registry;
}

QVector<int> LevelLoader::getRegisteredLevels() {
    QVector<int> levels;
    for (auto const& [id, entry] : getRegistry()) {
        levels.append(id);
    }
    return levels;
}

LevelData LevelLoader::load(int levelNumber) {
    auto& reg = getRegistry();
    if (reg.count(levelNumber)) {
        return reg.at(levelNumber).dataBuilder();
    }
    return {};
}

std::unique_ptr<Level> LevelLoader::createLevelInstance(int levelNumber, Game* game) {
    auto& reg = getRegistry();
    if (reg.count(levelNumber)) {
        return reg.at(levelNumber).instanceCreator(game);
    }
    return nullptr;
}