#pragma once
#include <QVector>
#include <QRectF>

class GameObject;
class Player;

struct CollisionResult {
    GameObject* object      = nullptr;
    bool        fromTop     = false;
    bool        fromBottom  = false;
    bool        fromLeft    = false;
    bool        fromRight   = false;
};

class CollisionEngine
{
public:

    QVector<CollisionResult> checkSolid(const QRectF& playerRect,
                                         const QVector<GameObject*>& allEntities);

    QVector<GameObject*> checkProximity(const QRectF& playerRect,
                                         const QVector<GameObject*>& allEntities,
                                         int range = 0);

    static bool overlaps(const QRectF& a, const QRectF& b);
    static CollisionResult resolveDirection(const QRectF& mover,
                                             const QRectF& other,
                                             GameObject*   obj);
};