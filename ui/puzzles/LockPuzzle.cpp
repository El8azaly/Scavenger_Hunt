#include "ui/puzzles/LockPuzzle.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
LockPuzzle::LockPuzzle(const PuzzleData& d, QWidget* p):PuzzleDialog(d,p){
    setWindowTitle("Lock");
    auto* lay=new QVBoxLayout(this);
    lay->addWidget(new QLabel("Enter the code:",this));
    auto* input=new QLineEdit(this); input->setMaxLength(6);
    auto* btn=new QPushButton("Unlock",this);
    QString correct=d.data.size()>0?d.data[0]:"0000";
    connect(btn,&QPushButton::clicked,this,[=](){
        m_solved=(input->text()==correct); accept();
    });
    lay->addWidget(input); lay->addWidget(btn);
}
