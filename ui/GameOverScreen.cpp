#include "ui/GameOverScreen.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

GameOverScreen::GameOverScreen(QWidget* p) : QWidget(p) {
    auto* lay = new QVBoxLayout(this);
    lay->setAlignment(Qt::AlignCenter);

    auto* lbl = new QLabel("GAME OVER", this);
    lbl->setStyleSheet("color:#e05c7a;font-size:36px;font-weight:bold;");
    lbl->setAlignment(Qt::AlignCenter);

    auto* r  = new QPushButton("Try Again", this);
    auto* ex = new QPushButton("Exit to Menu", this);
    for (auto* b : {r, ex}) {
        b->setFixedSize(180, 44);
        b->setStyleSheet("QPushButton{background:#3a3a5c;color:white;font-size:14px;"
                         "border-radius:6px;border:1px solid #e05c7a;}"
                         "QPushButton:hover{background:#e05c7a;}");
    }
    connect(r,  &QPushButton::clicked, this, &GameOverScreen::restartRequested);
    connect(ex, &QPushButton::clicked, this, &GameOverScreen::exitRequested);

    lay->addWidget(lbl);
    lay->addSpacing(20);
    lay->addWidget(r,  0, Qt::AlignCenter);
    lay->addWidget(ex, 0, Qt::AlignCenter);
    setStyleSheet("background:#1a1a2e;");
}

void GameOverScreen::setFinalScore(int /*score*/) {
    // TODO M2: display score on this screen
}
