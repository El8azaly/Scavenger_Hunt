#pragma once
#include "entities/InteractiveObject.h"
#include "ui/sprite/AnimatedSprite.h"
#include "ui/sprite/SlicedSprite.h"

class CaptainStar : public InteractiveObject {
public:
    CaptainStar(float x, float y);
    ~CaptainStar();

    void update() override;
    void draw(QPainter& painter, float camX, float camY) override;
    InteractionResult interact() override;

private:
    AnimatedSprite* m_sprite;
    SlicedSprite* m_dialogBox;

    enum class AnimState {
        Idle,
        Anticipation,
        Roll,
        PostRoll
    };

    AnimState m_animState = AnimState::Idle;
    int m_animTimer = 0;

    bool m_showDialog = false;
    int m_dialogTimer = 0;
};