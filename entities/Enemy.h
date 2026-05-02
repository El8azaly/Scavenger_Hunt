#pragma once
#include "entities/AttackableEntity.h"
#include <QVector>
#include <QRectF>

class Player;
class CollectibleItem;
struct Item;
class AnimatedSprite;

class Enemy : public AttackableEntity {
public:
    Enemy(float x, float y, float w, float h, int maxHealth, Player* player);
    virtual ~Enemy();

    void setEnvironment(const QVector<QRectF>& platforms) { m_platforms = platforms; }

    void setHeldItem(const Item& item);
    CollectibleItem* getBeltItem() const;
    CollectibleItem* takeDroppedItem();

    void update() override;

    void drawDialog(QPainter& painter, float camX, float camY);

    void showDialog(const QString& type);

protected:
    virtual void updateBehavior();
    virtual void updateAnimation() {}

    void onHit() override;

    Player* m_player;
    QVector<QRectF> m_platforms;

    CollectibleItem* m_beltItem = nullptr;
    CollectibleItem* m_droppedItemToTransfer = nullptr;
    bool m_itemDropped = false;
    float m_prevX = 0.0f;
    float m_prevY = 0.0f;

    AnimatedSprite* m_dialogSprite = nullptr;
    QString m_dialogState = "None";
    int m_dialogTimer = 0;
    bool m_hasSeenPlayer = false;
    bool m_hasBeenAttacked = false;

    int m_attackTimer;
    int m_landTimer;
    int m_jumpCooldown;
    int m_reactionTimer;
    bool m_isAggroed;

    float m_attackRange;
    float m_triggerRange;
    float m_loseAggroRange;
    float m_speed;
    float m_jumpForce;
    int m_attackDuration;

    float m_alpha;
    int m_deathFadeDelay;
    float m_fadeSpeed;
};