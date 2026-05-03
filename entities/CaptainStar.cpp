#include "CaptainStar.h"
#include "core/Constants.h"

CaptainStar::CaptainStar(float x, float y)
    : InteractiveObject(x, y, 40, 60) {
    m_sprite = new AnimatedSprite(":/assets/sprites/npc/captain_star.json",
                                  ":/assets/sprites/npc/captain_star.png");
    m_spriteWidth = 64;
    m_spriteHeight = 64;
    m_spriteOffsetX = (m_w - m_spriteWidth) / 2.0f;
    m_spriteOffsetY = (m_h - m_spriteHeight);

    m_sprite->setState("Idle");
    m_dialogBox = new SlicedSprite("captain_star_dialog_1x1");
}

CaptainStar::~CaptainStar() {
    delete m_sprite;
    delete m_dialogBox;
}

void CaptainStar::update() {
    if (m_animState != AnimState::Idle) {
        m_animTimer--;

        if (m_animTimer <= 0) {
            switch (m_animState) {
                case AnimState::Anticipation:
                    m_animState = AnimState::Roll;
                    m_sprite->setState("Roll");
                    m_animTimer = 25;
                    break;
                case AnimState::Roll:
                    m_animState = AnimState::PostRoll;
                    m_sprite->setState("PostRoll");
                    m_animTimer = 20;
                    break;
                case AnimState::PostRoll:
                    m_animState = AnimState::Idle;
                    m_sprite->setState("Idle");
                    break;
                default:
                    break;
            }
        }
    }
    if (m_showDialog) {
        m_dialogTimer--;
        if (m_dialogTimer <= 0) {
            m_showDialog = false;
        }
    }
    if (m_sprite) {
        m_sprite->update(16);
    }
}

void CaptainStar::draw(QPainter& painter, float camX, float camY) {

    if (m_sprite) {
        float centerX = m_x + (m_w / 2.0f);
        float centerY = m_y + (m_h / 2.0f);

        QRectF destRect(centerX - (m_spriteWidth / 2.0f) - camX,
                        centerY - (m_spriteHeight / 2.0f) - camY,
                        m_spriteWidth, m_spriteHeight);

        m_sprite->draw(painter, destRect, false);
    }
}

InteractionResult CaptainStar::interact() {
    InteractionResult result;
    result.type = InteractionResult::Type::None;

    if (m_animState == AnimState::Idle) {
        m_animState = AnimState::Anticipation;
        m_sprite->setState("Anticipation");
        m_animTimer = 15;
    }

    m_showDialog = true;
    m_dialogTimer = 180;

    return result;
}