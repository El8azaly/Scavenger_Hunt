#include "PuzzleChest.h"
#include "Player.h"
#include "../ui/screen/SlidingPuzzleDialog.h"
#include "../data/ItemRegistry.h"   // however your project looks up items

PuzzleChest::PuzzleChest(float x, float y,
                         const std::string& rewardItemId,
                         QWidget* parentWidget)
    : InteractiveObject(x, y),
    solved_(false),
    rewardItemId_(rewardItemId),
    parentWidget_(parentWidget)
{}

InteractionResult PuzzleChest::interact(Player& player) {
    if (solved_) {
        return InteractionResult::AlreadyUsed;   // adjust to your enum
    }

    SlidingPuzzleDialog dlg(parentWidget_);

    // Give the player the reward when they solve it
    connect(&dlg, &SlidingPuzzleDialog::puzzleSolved, [&]() {
        solved_ = true;
        player.getInventory().addItem(rewardItemId_);
    });

    dlg.exec();   // blocks until solved or dismissed
    return solved_ ? InteractionResult::Success : InteractionResult::Cancelled;
}

void PuzzleChest::draw(QPainter& painter) {
    // Reuse your existing chest sprite, or draw a locked/unlocked version
    // based on solved_ flag — same pattern as Container::draw()
}
