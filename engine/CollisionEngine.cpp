#include "engine/CollisionEngine.h"
#include "entities/GameObject.h"
#include "core/Constants.h"
#include <algorithm>

// ── Solid collision check ─────────────────────────────────────
QVector<CollisionResult> CollisionEngine::checkSolid(
    const QRectF& playerRect,
    const QVector<GameObject*>& allEntities)
{
    QVector<CollisionResult> results;

    for (GameObject* obj : allEntities) {
        if (!obj || !obj->isActive() || !obj->isSolid()) continue;

        QRectF objRect = obj->boundingBox();
        if (overlaps(playerRect, objRect)) {
            results.append(resolveDirection(playerRect, objRect, obj));
        }
    }
    return results;
}

// ── Proximity check (interaction range) ──────────────────────
QVector<GameObject*> CollisionEngine::checkProximity(
    const QRectF& playerRect,
    const QVector<GameObject*>& allEntities,
    int range)
{
    if (range == 0) range = Constants::INTERACT_RANGE;

    QRectF expandedRect = playerRect.adjusted(-range, -range, range, range);
    QVector<GameObject*> results;

    for (GameObject* obj : allEntities) {
        if (!obj || !obj->isActive() || obj->isSolid()) continue;
        if (overlaps(expandedRect, obj->boundingBox()))
            results.append(obj);
    }
    return results;
}

// ── Helpers ───────────────────────────────────────────────────
bool CollisionEngine::overlaps(const QRectF& a, const QRectF& b)
{
    return a.intersects(b);
}

CollisionResult CollisionEngine::resolveDirection(
    const QRectF& mover, const QRectF& other, GameObject* obj)
{
    CollisionResult r;
    r.object = obj;

    // Penetration depths on each axis
    float overlapLeft   = (mover.right()  - other.left());
    float overlapRight  = (other.right()  - mover.left());
    float overlapTop    = (mover.bottom() - other.top());
    float overlapBottom = (other.bottom() - mover.top());

    float minH = std::min(overlapLeft, overlapRight);
    float minV = std::min(overlapTop,  overlapBottom);

    if (minH < minV) {
        // Horizontal collision
        if (overlapLeft < overlapRight) r.fromLeft  = true;
        else                            r.fromRight = true;
    } else {
        // Vertical collision
        if (overlapTop < overlapBottom) r.fromTop    = true;
        else                            r.fromBottom = true;
    }
    return r;
}
