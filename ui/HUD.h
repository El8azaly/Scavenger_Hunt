#pragma once
#include <QPainter>
class ScoreManager;
class InventorySystem;
class QuestSystem;

class Player;

class HUD {
public:
    static void makeVignette(QPixmap &vignetteCache, bool &isVignetteCached);

    static void draw(QPainter& p, ScoreManager* score, InventorySystem* inv, QuestSystem* quest, Player* player = nullptr);
};