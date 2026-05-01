#include "ui/HUD.h"
#include "ui/PixelFont.h"
#include "systems/ScoreManager.h"
#include "systems/InventorySystem.h"
#include "systems/QuestSystem.h"
#include "core/Constants.h"

void HUD::draw(QPainter& p, ScoreManager* score, InventorySystem* inv, QuestSystem* quest) {
    // Instantiate without arguments (uses hardcoded path)
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
}