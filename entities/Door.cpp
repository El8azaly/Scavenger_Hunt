#include "entities/Door.h"
#include "entities/InteractionResult.h"
#include <QPainter>
#include <QTimer>

static constexpr const char* DOOR_JSON = ":/assets/sprites/objects/door.json";
static constexpr const char* DOOR_PNG  = ":/assets/sprites/objects/door.png";

static constexpr float DOOR_DRAW_W = 41.0f * 2.0f;
static constexpr float DOOR_DRAW_H = 49.0f * 2.0f;

Door::Door(float x, float y, float w, float h, int fadeTimeMs)
    : InteractiveObject(x, y, w, h)
    , m_fadeTimeMs(fadeTimeMs) {
    m_sprite = new AnimatedSprite(DOOR_JSON, DOOR_PNG);
    transitionTo(DoorState::Idle);
}

Door::~Door() {
    delete m_sprite;
}

void Door::setPartner(Door* partner) {
    m_partner = partner;
}

bool Door::isInteractable() const {
    if (m_doorState != DoorState::Idle) return false;
    if (m_partner && m_partner->doorState() != DoorState::Idle) return false;
    return true;
}

InteractionResult Door::interact() {
    if (!isInteractable()) return InteractionResult{InteractionResult::Type::None};
    if (m_partner) m_partner->transitionTo(DoorState::HoldOpen);
    transitionTo(DoorState::Opening);
    return InteractionResult{InteractionResult::Type::None};
}

QPointF Door::teleportDestination() const {
    if (!m_partner) return QPointF(m_x, m_y);
    return QPointF(m_partner->x()+18, m_partner->y()+36);
}

bool Door::isOpenAnimDone() const {
    return m_doorState == DoorState::Opening && m_sprite && m_sprite->isFinished();
}

void Door::onFadeOutComplete() {

}

void Door::onFadeInComplete() {
    transitionTo(DoorState::Closing);
    if (m_partner) m_partner->transitionTo(DoorState::Closing);
}

void Door::reset() {
    transitionTo(DoorState::Closing);

    if (m_partner &&
        m_partner->doorState() != DoorState::Closing &&
        m_partner->doorState() != DoorState::Idle) {
        m_partner->transitionTo(DoorState::Closing);
    }
}

void Door::update() {
    if (!m_sprite) return;

    switch (m_doorState) {

        case DoorState::Idle:
            m_sprite->update(m_pendingDelta);
            break;

        case DoorState::Opening:
            if (!m_sprite->isFinished())
                m_sprite->update(m_pendingDelta);
            break;

        case DoorState::HoldOpen:
            break;

        case DoorState::Closing:
            if (!m_sprite->isFinished()) {
                m_sprite->update(m_pendingDelta);
            }
            if (m_sprite->isFinished()) {
                transitionTo(DoorState::Idle);
            }
            break;
    }
}

void Door::draw(QPainter& painter, float camX, float camY) {
    if (!m_sprite) return;
    QRectF dest(m_x - camX, m_y - camY, DOOR_DRAW_W, DOOR_DRAW_H);
    m_sprite->draw(painter, dest, false);
}

void Door::transitionTo(DoorState next) {
    m_doorState = next;
    if (!m_sprite) return;

    switch (next) {
        case DoorState::Idle:
            m_sprite->setLoop(true);
            m_sprite->forceState("Idle");
            break;

        case DoorState::Opening:
            m_sprite->setLoop(false);
            m_sprite->forceState("Opening");
            break;

        case DoorState::HoldOpen:
            m_sprite->setLoop(false);
            m_sprite->forceState("Opening");
            m_sprite->update(999999);
            break;

        case DoorState::Closing:
            m_sprite->setLoop(false);
            m_sprite->forceState("Closing");
            break;
    }
}