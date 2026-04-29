#include "ui/HUD.h"
#include "systems/ScoreManager.h"
#include "systems/InventorySystem.h"
#include "systems/QuestSystem.h"
#include "core/Constants.h"
#include <QFont>

void HUD::draw(QPainter& p, ScoreManager* score, InventorySystem* inv, QuestSystem* quest) {
    // Semi-transparent top bar
    p.fillRect(0, 0, Constants::WINDOW_WIDTH, 44, QColor(0,0,0,140));
    p.setPen(Qt::white);
    p.setFont(QFont("Arial", 14, QFont::Bold));

    if(score) {
        // Score
        p.drawText(QPoint(12, 28),
                   QString("Score: %1").arg(score->currentScore()));
        // Timer
        int t = score->timeLeft();
        QString ts = QString("%1:%2").arg(t/60,2,10,QChar('0')).arg(t%60,2,10,QChar('0'));
        QColor tc = t>30 ? Qt::white : QColor(255,80,80);
        p.setPen(tc);
        p.drawText(QPoint(Constants::WINDOW_WIDTH/2 - 30, 28), "Time: " + ts);
        p.setPen(Qt::white);
    }

    if(quest) {
        // Quest progress top-right
        QString qStr = QString("Items: %1/%2")
            .arg(quest->foundTargets()).arg(quest->totalTargets());
        p.drawText(QPoint(Constants::WINDOW_WIDTH - 140, 28), qStr);
    }
}
