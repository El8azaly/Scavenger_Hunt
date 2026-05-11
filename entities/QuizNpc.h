#pragma once

#include "entities/InteractiveObject.h"
#include "data/ItemData.h"
#include <QVector>

class QuizNpc : public InteractiveObject {
public:
    QuizNpc(float x, float y, float w = 40.0f, float h = 60.0f);

    void update() override;
    void draw(QPainter& painter, float camX, float camY) override;
    InteractionResult interact() override;

    void addRewardItem(const Item& item);
    bool isInteractable() const override { return !m_rewardGiven; }

private:
    QVector<Item> m_rewards;
    bool m_rewardGiven = false;
};
