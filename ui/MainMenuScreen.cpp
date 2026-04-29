#include "ui/MainMenuScreen.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
MainMenuScreen::MainMenuScreen(QWidget* p):QWidget(p){
    auto* lay=new QVBoxLayout(this);
    lay->setAlignment(Qt::AlignCenter);
    auto* title=new QLabel("SCAVENGER HUNT",this);
    title->setStyleSheet("color:white;font-size:36px;font-weight:bold;");
    title->setAlignment(Qt::AlignCenter);
    auto* btn=new QPushButton("START GAME",this);
    btn->setFixedSize(200,50);
    btn->setStyleSheet("QPushButton{background:#f0c040;color:#111;font-size:16px;font-weight:bold;border-radius:8px;}"
                       "QPushButton:hover{background:#ffd060;}");
    connect(btn,&QPushButton::clicked,this,&MainMenuScreen::startRequested);
    lay->addWidget(title); lay->addSpacing(30); lay->addWidget(btn,0,Qt::AlignCenter);
    setStyleSheet("background:#1a1a2e;");
}
