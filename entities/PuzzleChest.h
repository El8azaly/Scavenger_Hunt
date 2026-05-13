#pragma once
#include "InteractiveObject.h"
#include <string>

class PuzzleChest : public InteractiveObject {
    Q_OBJECT
public:
    PuzzleChest(float x, float y, const std::string& rewardItemId,
                QWidget* parentWidget = nullptr);

    InteractionResult interact(Player& player) override;
    void draw(QPainter& painter) override;   // match your base signature

private:
    bool          solved_;
    std::string   rewardItemId_;
    QWidget*      parentWidget_;
};
