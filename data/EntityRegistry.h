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
                                       float maxSpeed = 10.0f, float shotRange = 1000.0f,
                                       bool hasTarget = false, float targetX = 0.0f, float targetY = 0.0f) {
        EntitySpawnData e;
        e.type = "enemy";
        e.id = id;
        e.x = x;
        e.y = y;
        e.w = 64.0f;
        e.h = 64.0f;

        e.properties["enemyType"] = "cannon";
        e.properties["maxSpeed"] = maxSpeed;
        e.properties["shotRange"] = shotRange;

        e.properties["hasTarget"] = hasTarget;
        e.properties["targetX"] = targetX;
        e.properties["targetY"] = targetY;

        return e;
    }

    static EntitySpawnData spawnDoor(const QString& id, const QString& groupId,
                                      float x, float y,
                                      float w = 82.0f, float h = 98.0f,
                                      int fadeTimeMs = 500) {
        EntitySpawnData e;
        e.type = "door";
        e.id   = id;
        e.x    = x;
        e.y    = y;
        e.w    = w;
        e.h    = h;
        e.properties["groupId"]    = groupId;
        e.properties["fadeTimeMs"] = fadeTimeMs;
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