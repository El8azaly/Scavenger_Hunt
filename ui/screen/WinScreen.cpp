#include "WinScreen.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

WinScreen::WinScreen(QWidget* p) : QWidget(p) {
    auto* lay = new QVBoxLayout(this);
    lay->setAlignment(Qt::AlignCenter);

    auto* lbl = new QLabel("YOU WIN!", this);
    lbl->setStyleSheet("color:#4ecb8d;font-size:36px;font-weight:bold;");
    lbl->setAlignment(Qt::AlignCenter);

    auto* nx = new QPushButton("Next Level",   this);
    auto* ex = new QPushButton("Exit to Menu", this);
    for (auto* b : {nx, ex}) {
        b->setFixedSize(180, 44);
        b->setStyleSheet("QPushButton{background:#3a3a5c;color:white;font-size:14px;"
                         "border-radius:6px;border:1px solid #4ecb8d;}"
                         "QPushButton:hover{background:#4ecb8d;color:#111;}");
    }
    connect(nx, &QPushButton::clicked, this, &WinScreen::nextLevelRequested);
    connect(ex, &QPushButton::clicked, this, &WinScreen::exitRequested);

    lay->addWidget(lbl);
    lay->addSpacing(20);
    lay->addWidget(nx, 0, Qt::AlignCenter);
    lay->addWidget(ex, 0, Qt::AlignCenter);
    setStyleSheet("background:#1a1a2e;");
}

void WinScreen::setFinalScore(int ) {

}