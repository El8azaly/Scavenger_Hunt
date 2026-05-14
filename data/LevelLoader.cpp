#include "data/LevelLoader.h"
#include "core/Constants.h"
#include "data/ItemRegistry.h"
#include "data/EntityRegistry.h"
#include "levels/Level0.h"
#include "levels/Level1.h"
#include "core/Game.h"
#include "levels/Level2.h"
#include "levels/Level3.h"

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
    d.playerStartX = 111;
    d.playerStartY = 193;

    d.itemLibrary = ItemRegistry::allItems().values().toVector();
    d.entities.append(EntityRegistry::spawnCaptainStar("capnstar",158, 193 ));
    d.entities.append(EntityRegistry::spawnCollectible("sword_1", "sword", 286, 196));

    d.entities.append(EntityRegistry::spawnEnemy("fiercetooth_1", "fiercetooth", 668, 104, "skull"));

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
static LevelData buildLevel2() {
    LevelData d;
    d.levelNumber  = 1;
    d.timeLimitSec = Constants::LEVEL1_TIME_SEC;
    d.playerStartX = 116;
    d.playerStartY = 182;

    d.itemLibrary = ItemRegistry::allItems().values().toVector();
    d.entities.append(EntityRegistry::spawnCaptainStar("capnstar",66, 214 ));
    d.entities.append(EntityRegistry::spawnCollectible("sword_1", "sword", 167, 176));
    d.entities.append(EntityRegistry::spawnCannon("cannon_1", 411, 112, 11.0f, 200.0f, true, 353, 110));
    d.entities.append(EntityRegistry::spawnEnemy("fiercetooth_1", "fiercetooth", 723, 195, "green_potion"));
    d.entities.append(EntityRegistry::spawnQuizNpc(
        "quiz_keeper_2", 280, 66, "emerald",
        {
            3, {
                {"What does a pointer store in C++?", {"A value", "A memory address", "A file", "A class"}, 2},
                {"Which data structure uses FIFO order?", {"Stack", "Queue", "Tree", "Graph"}, 2},
                {"Who is the best doctor at AUC?", {"Dr Howaida", "Dr Howaida", "Dr Howaida", "Dr Howaida"}, 1}
            }
        }
    ));
    d.entities.append(EntityRegistry::spawnCollectible("item_2", "diamond", 292, 154));

    d.entities.append(EntityRegistry::spawnContainer(
        "chest_1", "chest", 62, 22,
        {"silver_coin", "brass_key"}
        ));

    d.targetIds.append({"green_potion", "emerald", "diamond", "silver_coin", "brass_key"});

    return d;
}

static LevelData buildLevel3() {
    LevelData d;
    d.levelNumber  = 1;
    d.timeLimitSec = Constants::LEVEL1_TIME_SEC;
    d.playerStartX = 61;
    d.playerStartY = 88;

    d.itemLibrary = ItemRegistry::allItems().values().toVector();
    d.entities.append(EntityRegistry::spawnCaptainStar("capnstar",115, 87 ));
    d.entities.append(EntityRegistry::spawnCollectible("item_1", "emerald", 409, 93));
    d.entities.append(EntityRegistry::spawnCollectible("item_2", "diamond", 349, 177));
    d.entities.append(EntityRegistry::spawnCollectible("item_1", "map", 327, 574));
    d.entities.append(EntityRegistry::spawnCollectible("item_1", "skull", 371, 390));
    d.entities.append(EntityRegistry::spawnCollectible("item_2", "green_potion", 173, 645));
    d.entities.append(EntityRegistry::spawnDoor("door_1a", "a", 51, 671));
    d.entities.append(EntityRegistry::spawnDoor("door_1b", "a", 50, 74));
    d.entities.append(EntityRegistry::spawnDoor("door_2a", "b", 44, 565));
    d.entities.append(EntityRegistry::spawnDoor("door_2b", "b", 289, 563));
    d.entities.append(EntityRegistry::spawnDoor("door_3a", "c", 157, 330));
    d.entities.append(EntityRegistry::spawnDoor("door_3b", "c", 288, 330));
    d.entities.append(EntityRegistry::spawnDoor("door_4a", "d", 296, 96));
    d.entities.append(EntityRegistry::spawnDoor("door_4b", "d", 44, 202));

    d.targetIds.append({"emerald", "diamond", "map", "skull", "green_potion"});

    return d;
}
const std::map<int, LevelRegistryEntry>& LevelLoader::getRegistry() {
    static const std::map<int, LevelRegistryEntry> registry = {
        {0, { buildLevel0, [](Game* g) { return std::make_unique<Level0>(g); } }},
        {1, { buildLevel1, [](Game* g) { return std::make_unique<Level1>(g); } }},
        {2, { buildLevel2, [](Game* g) { return std::make_unique<Level2>(g); } }},
        {3, { buildLevel3, [](Game* g) { return std::make_unique<Level3>(g); } }}
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