#pragma once
#include "data/LevelLoader.h"
#include <QString>
#include <QStringList>

class EntityRegistry {
public:

    static EntitySpawnData spawnEnemy(const QString& id, const QString& enemyType, float x, float y, const QString& heldItem = "") {
        EntitySpawnData e;
        e.type = "enemy";
        e.id = id;
        e.x = x;
        e.y = y;

        if (enemyType == "fiercetooth") {
            e.w = 32.0f;
            e.h = 32.0f;
        } else {
            e.w = 32.0f;
            e.h = 32.0f;
        }

        e.properties["enemyType"] = enemyType;
        if (!heldItem.isEmpty()) {
            e.properties["heldItemId"] = heldItem;
        }
        return e;
    }

    static EntitySpawnData spawnCaptainStar(const QString& id, float x, float y) {
        EntitySpawnData e;
        e.type = "npc_star";
        e.id = id;
        e.x = x;
        e.y = y;
        e.w = 64.0f;
        e.h = 64.0f;
        return e;
    }

    static EntitySpawnData spawnCannon(const QString& id, float x, float y,
                                       float shotPower = 6.0f, float shotRange = 600.0f,
                                       float hitboxOffsetX = 0.0f, float hitboxOffsetY = 0.0f,
                                       float spawnOffsetX = -10.0f, float spawnOffsetY = 20.0f) {
        EntitySpawnData e;
        e.type = "enemy";
        e.id = id;
        e.x = x;
        e.y = y;
        e.w = 64.0f;
        e.h = 64.0f;

        e.properties["enemyType"] = "cannon";
        e.properties["shotPower"] = shotPower;
        e.properties["shotRange"] = shotRange;
        e.properties["hitboxOffsetX"] = hitboxOffsetX;
        e.properties["hitboxOffsetY"] = hitboxOffsetY;
        e.properties["spawnOffsetX"] = spawnOffsetX;
        e.properties["spawnOffsetY"] = spawnOffsetY;

        return e;
    }

    static EntitySpawnData spawnContainer(const QString& id, const QString& containerType, float x, float y, const QStringList& contents = {}) {
        EntitySpawnData e;
        e.type = "container";
        e.id = id;
        e.x = x;
        e.y = y;

        if (containerType == "chest" || containerType == "barrel" || containerType == "box") {
            e.w = 39.0f;
            e.h = 64.0f;
        } else {
            e.w = 32.0f;
            e.h = 32.0f;
        }

        e.properties["containerType"] = containerType;
        if (!contents.isEmpty()) {
            e.properties["contents"] = contents;
        }
        return e;
    }

    static EntitySpawnData spawnCollectible(const QString& id, const QString& itemId, float x, float y, float w = 36.0f, float h = 36.0f) {
        EntitySpawnData e;
        e.type = "collectible";
        e.id = id;
        e.x = x;
        e.y = y;
        e.w = w;
        e.h = h;
        e.properties["itemId"] = itemId;
        return e;
    }
};