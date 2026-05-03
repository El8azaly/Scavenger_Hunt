#pragma once
#include <QPainter>
#include "sprite/PixelFont.h"
class ScoreManager;
class InventorySystem;
class QuestSystem;
class Game;
class Player;

class HUD {
public:
    static void makeVignette(QPixmap &vignetteCache, bool &isVignetteCached);

    static void drawPlayerHud(QPainter &p, ScoreManager *score, Player *player, PixelFont &pixelFont);

    static void drawHotbar(QPainter &p, InventorySystem *inv, QuestSystem* quest);
    static void drawLevelHud(QPainter &p, ScoreManager *score, PixelFont &pixelFont, Game* game);
    static void draw(QPainter& p, ScoreManager* score, InventorySystem* inv, QuestSystem* quest, Player* player = nullptr, Game* game = nullptr);
};