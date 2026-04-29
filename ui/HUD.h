#pragma once
#include <QPainter>
class ScoreManager;
class InventorySystem;
class QuestSystem;

class HUD {
public:
    // Static-only: no instances. Game::draw() calls this directly.
    static void draw(QPainter& painter, ScoreManager* score,
                     InventorySystem* inv, QuestSystem* quest);
};
