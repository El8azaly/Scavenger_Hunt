#include "entities/QuizNpc.h"
#include "ui/sprite/AnimatedSprite.h"
#include <QPainter>

QuizNpc::QuizNpc(float x, float y, float w, float h)
    : InteractiveObject(x, y, w, h) {

    m_sprite = new AnimatedSprite(":/assets/sprites/enemy/crabby.json",
                                  ":/assets/sprites/enemy/crabby.png");
    m_sprite->setState("Idle");
    m_sprite->setLoop(true);

    m_spriteWidth = 236.0f;
    m_spriteHeight = 64.0f;
    m_spriteOffsetX = (m_w - m_spriteWidth) / 2.0f;
    m_spriteOffsetY = m_h - m_spriteHeight +4 ;
}

void QuizNpc::update() {
    if (m_sprite) {
        m_sprite->update(16);
    }
}

void QuizNpc::draw(QPainter& painter, float camX, float camY) {
    drawSprite(painter, camX, camY, false);
}

void QuizNpc::addRewardItem(const Item& item) {
    if (!item.isNull()) {
        m_rewards.append(item);
    }
}

InteractionResult QuizNpc::interact() {
    InteractionResult result;

    if (m_rewardGiven) {
        result.type = InteractionResult::Type::None;
        return result;
    }

    if (m_isLocked) {
        result.type = InteractionResult::Type::ShowPuzzle;
        result.puzzle.type = m_puzzleType;
        result.puzzle.data = m_puzzleData;
        result.puzzle.objectId = id();
        return result;
    }

    m_rewardGiven = true;
    result.type = InteractionResult::Type::RevealItems;
    result.revealedItems = m_rewards;
    m_rewards.clear();
    return result;
}