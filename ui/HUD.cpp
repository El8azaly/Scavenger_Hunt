#include "ui/HUD.h"
#include "ui/PixelFont.h"
#include "systems/ScoreManager.h"
#include "systems/InventorySystem.h"
#include "systems/QuestSystem.h"
#include "core/Constants.h"
#include "entities/Player.h"

void HUD::draw(QPainter& p, ScoreManager* score, InventorySystem* inv, QuestSystem* quest, Player* player) {
    static PixelFont pixelFont;

    // Semi-transparent top bar
    p.fillRect(0, 0, Constants::WINDOW_WIDTH, 38, QColor(0,0,0,140));

    if(score) {
        // Draw Light Score text at 3x scale (Default color)
        QString scoreText = QString("Score: %1").arg(score->currentScore());
        pixelFont.drawText(p, scoreText, 12, 12, 3);

        int t = score->timeLeft();
        QString ts = QString("%1:%2").arg(t/60,2,10,QChar('0')).arg(t%60,2,10,QChar('0'));
        QString timeText = "Time: " + ts;

        // Example of using a completely custom warning color if time is low
        QColor timeColor = (t > 30) ? PixelFont::Light : QColor("#ff5555");
        pixelFont.drawText(p, timeText, Constants::WINDOW_WIDTH/2 - 90, 12, 3, timeColor);
    }

    if(quest) {
        QString qStr = QString("Items: %1/%2")
            .arg(quest->foundTargets())
            .arg(quest->totalTargets());

        // Example: Draw the dark color variant
        pixelFont.drawText(p, qStr, Constants::WINDOW_WIDTH - 190, 12, 3, PixelFont::Light);

    }
    if (player) {
        int hp = player->getHealth();
        int maxHp = player->getMaxHealth();

        // Draw dark background for the bar
        p.fillRect(12, 45, 104, 14, QColor(30, 30, 30, 200));

        // Calculate health percentage and draw red fill
        float hpRatio = static_cast<float>(hp) / maxHp;
        p.fillRect(14, 47, static_cast<int>(100 * hpRatio), 10, QColor(220, 50, 50));

        // Draw 'HP' text using your PixelFont
        static PixelFont pixelFont;
        pixelFont.drawText(p, "HP", 14, 44, 1, PixelFont::Light);
    }

}