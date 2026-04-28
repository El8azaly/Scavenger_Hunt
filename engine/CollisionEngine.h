#pragma once
#include <QVector>
#include <QRectF>

// Forward declarations — CollisionEngine does not own these
class GameObject;
class Player;

// ─────────────────────────────────────────────────────────────
//  CollisionResult — what Game::update() receives per collision
// ─────────────────────────────────────────────────────────────
struct CollisionResult {
    GameObject* object      = nullptr; // which entity was collided with
    bool        fromTop     = false;   // player landed on top (should stop falling)
    bool        fromBottom  = false;   // player hit ceiling (should stop rising)
    bool        fromLeft    = false;   // player moving right hit a wall
    bool        fromRight   = false;   // player moving left hit a wall
};

// ─────────────────────────────────────────────────────────────
//  CollisionEngine — pure spatial logic, no game rules here
//
//  Game calls checkSolid() and checkProximity() each frame.
//  Game then decides what to do with the results (resolve
//  physics, trigger interaction, pick up item, etc.).
//
//  This class has NO knowledge of items, inventory, or puzzles.
// ─────────────────────────────────────────────────────────────
class CollisionEngine
{
public:
    // Returns solid-object collisions for the player rect.
    // "Solid" means entity->isSolid() == true (platforms, walls).
    // Game uses these results to push the player out of geometry.
    QVector<CollisionResult> checkSolid(const QRectF& playerRect,
                                         const QVector<GameObject*>& allEntities);

    // Returns all non-solid entities whose bounding box overlaps
    // playerRect expanded by INTERACT_RANGE on all sides.
    // Game uses this list to decide what the player can interact with
    // or automatically collect.
    QVector<GameObject*> checkProximity(const QRectF& playerRect,
                                         const QVector<GameObject*>& allEntities,
                                         int range = 0);

    // Static helpers — used by the above methods, also available to tests
    static bool overlaps(const QRectF& a, const QRectF& b);
    static CollisionResult resolveDirection(const QRectF& mover,
                                             const QRectF& other,
                                             GameObject*   obj);
};
