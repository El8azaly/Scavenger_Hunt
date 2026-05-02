#include "ui/HUD.h"
#include "sprite/PixelFont.h"
#include "systems/ScoreManager.h"
#include "systems/InventorySystem.h"
#include "core/Constants.h"
#include "entities/Player.h"
#include "sprite/SlicedSprite.h"
#include <cmath>

#include "systems/QuestSystem.h"

void HUD::makeVignette(QPixmap &vignetteCache, bool &isVignetteCached) {
    vignetteCache = QPixmap(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT);
    vignetteCache.fill(Qt::transparent);
    QPainter cachePainter(&vignetteCache);

    float cx = Constants::WINDOW_WIDTH / 2.0f;
    float cy = Constants::WINDOW_HEIGHT / 2.0f;
    float radius = std::sqrt(cx * cx + cy * cy);
    QRadialGradient gradient(cx, cy, radius);
    gradient.setColorAt(0.0, QColor(0, 0, 0, 0));
    gradient.setColorAt(0.6, QColor(0, 0, 0, 0));
    gradient.setColorAt(1.0, QColor(0, 0, 0, 45));

    cachePainter.fillRect(0, 0, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT, gradient);
    isVignetteCached = true;
}

void HUD::draw(QPainter& p, ScoreManager* score, InventorySystem* inv, QuestSystem* quest, Player* player) {
    static QPixmap vignetteCache;
    static bool isVignetteCached = false;

    if (!isVignetteCached) {
        makeVignette(vignetteCache, isVignetteCached);
    }
    p.drawPixmap(0, 0, vignetteCache);

    static PixelFont pixelFont;

    if(quest) {
        QString qStr = QString("Items: %1/%2")
            .arg(quest->foundTargets())
            .arg(quest->totalTargets());
        pixelFont.drawText(p, qStr, Constants::WINDOW_WIDTH - 190, 12, 3, PixelFont::Light);
    }
    if (player) {
        int hp = player->getHealth();
        int maxHp = player->getMaxHealth();
        int scale = Constants::UI_SCALE;

        int hudW = 80 * scale;
        int hudH = 48 * scale;
        int hudX = 12;
        int hudY = (Constants::WINDOW_HEIGHT - hudH - 12) - (5 * scale);

        SlicedSprite healthHud("health_hud");
        healthHud.draw(p, hudX, hudY, hudW, hudH);

        int startX = hudX + (26 * scale);
        int startY = hudY + (19 * scale);
        int heartW = 8 * scale;
        int heartH = 7 * scale;
        int gap = 3 * scale;
        int heartsToDraw = hp / (maxHp / 3);

        SlicedSprite heart("heart");
        for (int i = 0; i < heartsToDraw; i++) {
            int currentX = startX + i * (heartW + gap);
            heart.draw(p, currentX, startY, heartW, heartH);
        }

        SlicedSprite timeHud("time_hud");
        timeHud.draw(p, hudX + (17 * scale), hudY + (46 * scale), 15 * scale, 8 * scale);

        int numX = hudX + (35 * scale);
        int numY = hudY + (36 * scale);
        int currentScore = 0;
        int time  = 0;
        if (score) {
            currentScore = score->currentScore();
            time = score->timeLeft();
        }
        pixelFont.drawText(p, QString("%1").arg(currentScore), numX, numY, scale, PixelFont::Bright);
        QString ts = QString("%1:%2").arg(time/60,2,10,QChar('0')).arg(time%60,2,10,QChar('0'));
        int timeTextY = numY + (11 * scale);
        QColor timeColor = (time > 30) ? PixelFont::Bright : QColor("#ff5555");
        pixelFont.drawText(p, ts, numX, timeTextY, scale, timeColor);
    }
}