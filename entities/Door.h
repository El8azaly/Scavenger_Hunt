#pragma once
#include "entities/InteractiveObject.h"
#include "ui/sprite/AnimatedSprite.h"
#include <QPointF>
#include <QString>

class Door : public InteractiveObject
{
public:
    explicit Door(float x, float y, float w, float h, int fadeTimeMs = 500);
    ~Door();

    void    setPartner(Door* partner);
    Door*   partner() const { return m_partner; }

    void    setGroupId(const QString& g) { m_groupId = g; }
    QString groupId()  const             { return m_groupId; }

    int  fadeTimeMs() const { return m_fadeTimeMs; }

    bool isInteractable() const override;

    InteractionResult interact() override;

    void notifyDelta(int deltaTimeMs) { m_pendingDelta = deltaTimeMs; }
    void update() override;
    void draw(QPainter& painter, float camX, float camY) override;

    void onFadeOutComplete();
    void onFadeInComplete();
    bool isOpenAnimDone() const;

    QPointF teleportDestination() const;
    void reset();

    enum class DoorState { Idle, Opening, HoldOpen, Closing };
    DoorState doorState() const { return m_doorState; }

private:
    void transitionTo(DoorState next);

    AnimatedSprite* m_sprite      = nullptr;
    Door*           m_partner     = nullptr;
    QString         m_groupId;

    DoorState m_doorState    = DoorState::Idle;
    int       m_pendingDelta = 16;
    int       m_fadeTimeMs;

    float m_spriteW = 0.f;
    float m_spriteH = 0.f;
};