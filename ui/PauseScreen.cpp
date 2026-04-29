#include "ui/PauseScreen.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
PauseScreen::PauseScreen(QWidget* p):QWidget(p){
    auto* lay=new QVBoxLayout(this); lay->setAlignment(Qt::AlignCenter);
    auto* lbl=new QLabel("PAUSED",this);
    lbl->setStyleSheet("color:white;font-size:32px;font-weight:bold;"); lbl->setAlignment(Qt::AlignCenter);
    auto mkBtn=[&](const QString& t)->QPushButton*{
        auto* b=new QPushButton(t,this); b->setFixedSize(180,44);
        b->setStyleSheet("QPushButton{background:#3a3a5c;color:white;font-size:14px;border-radius:6px;border:1px solid #5b8aff;}"
                         "QPushButton:hover{background:#5b8aff;}"); return b;
    };
    auto* r=mkBtn("Resume"); auto* rs=mkBtn("Restart"); auto* ex=mkBtn("Exit to Menu");
    connect(r,&QPushButton::clicked,this,&PauseScreen::resumeRequested);
    connect(rs,&QPushButton::clicked,this,&PauseScreen::restartRequested);
    connect(ex,&QPushButton::clicked,this,&PauseScreen::exitRequested);
    lay->addWidget(lbl); lay->addSpacing(20);
    lay->addWidget(r,0,Qt::AlignCenter); lay->addWidget(rs,0,Qt::AlignCenter); lay->addWidget(ex,0,Qt::AlignCenter);
    setStyleSheet("background:rgba(10,10,20,220);");
}
