#include "ui/HUD.h"
#include "sprite/PixelFont.h"
#include "systems/ScoreManager.h"
#include "systems/InventorySystem.h"
#include "core/Constants.h"
#include "entities/Player.h"
#include "sprite/SlicedSprite.h"
#include "sprite/AnimatedSprite.h"
#include <cmath>
#include <algorithm>
#include <QMap>
#include <QSet>
#include <QVector>
#include <QDateTime>

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

void HUD::drawPlayerHud(QPainter &p, ScoreManager *score, Player *player, PixelFont &pixelFont) {
    int scale = Constants::UI_SCALE;
    int hp = player->getHealth();
    int maxHp = player->getMaxHealth();

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

void HUD::drawHotbar(QPainter &p, InventorySystem *inv, QuestSystem* quest) {
    int scale = Constants::UI_SCALE;

    const auto& allItems = inv->allItems();
    QVector<Item> items;

    int slotCount = 0;
    if (quest) {
        slotCount = std::max(quest->totalTargets(), 1);
        for (const auto& item : allItems) {

            if (quest->isTarget(item.id)) {
                items.append(item);
            }
        }
    } else {
        items = allItems;
    }

    int itemCount = items.size();

    QString hotbarName = QString("item_hotbar_%1").arg(slotCount);
    SlicedSprite hotbar(hotbarName);

    int hotbarW = 76 * scale;
    int hotbarH = 20 * scale;

    int hotbarX = Constants::WINDOW_WIDTH - hotbarW - 24;
    int hotbarY = (Constants::WINDOW_HEIGHT - hotbarH - 12) - (5 * scale);

    hotbar.draw(p, hotbarX, hotbarY, hotbarW, hotbarH);

    static QMap<QString, AnimatedSprite*> spriteCache;
    QSet<QString> updatedThisFrame;
    for (int i = 0; i < slotCount; i++) {
        float boxCenterX = (hotbarX + hotbarW) - (7.0f * scale) - (i * 13.0f * scale) - (5.0f * scale);
        float boxCenterY = hotbarY + (4.5f * scale) + (5.0f * scale);

        if (i < itemCount) {
            const auto& item = items[i];

            if (!spriteCache.contains(item.id)) {
                AnimatedSprite* newSprite = new AnimatedSprite(item.spriteJsonPath, item.spriteImagePath);
                newSprite->setState("Idle");
                spriteCache.insert(item.id, newSprite);
            }

            AnimatedSprite* m_sprite = spriteCache[item.id];

            if (m_sprite) {
                if (!updatedThisFrame.contains(item.id)) {
                    m_sprite->update(16);
                    updatedThisFrame.insert(item.id);
                }

                float itemDrawnW = item.spriteWidth / 2.0f;
                float itemDrawnH = item.spriteHeight / 2.0f;

                float itemX = boxCenterX - (itemDrawnW / 2.0f);
                float itemY = boxCenterY - (itemDrawnH / 2.0f);

                QRectF dest(itemX, itemY, itemDrawnW, itemDrawnH);
                m_sprite->draw(p, dest, false);
            }
        }
    }
    static bool wasComplete = false;
    static qint64 completionTime = 0;
    if (quest) {
        bool isComplete = quest->isComplete();
        if (isComplete && !wasComplete) {
            completionTime = QDateTime::currentMSecsSinceEpoch();
        }
        wasComplete = isComplete;
        if (isComplete) {
            qint64 now = QDateTime::currentMSecsSinceEpoch();
            qint64 elapsed = now - completionTime;
            if (elapsed < 8000) {
                float opacity = 1.0f;
                if (elapsed < 1000) {
                    opacity = elapsed / 1000.0f;
                }
                else if (elapsed > 7000) {
                    opacity = (8000.0f - elapsed) / 1000.0f;
                }
                int targetW = 160 * scale;
                int targetH = 64 * scale;

                int dialogX = Constants::WINDOW_WIDTH - targetW - 24;
                int dialogY = hotbarY - targetH - (10 * scale);

                p.save();
                p.setOpacity(opacity);
                SlicedSprite successDialog("captain_star_sucess_dialog_1x1");
                successDialog.draw(p, dialogX, dialogY, targetW, targetH);
                p.restore();
            }
        }
    }
}

void HUD::draw(QPainter& p, ScoreManager* score, InventorySystem* inv, QuestSystem* quest, Player* player) {
    static QPixmap vignetteCache;
    static bool isVignetteCached = false;
    if (!isVignetteCached) makeVignette(vignetteCache, isVignetteCached);
    p.drawPixmap(0, 0, vignetteCache);
    static PixelFont pixelFont;

    if (player) drawPlayerHud(p, score, player, pixelFont);
    if (inv) drawHotbar(p, inv, quest);
}