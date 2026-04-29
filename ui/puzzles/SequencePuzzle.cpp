#include "ui/puzzles/SequencePuzzle.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
SequencePuzzle::SequencePuzzle(const PuzzleData& d, QWidget* p):PuzzleDialog(d,p){
    setWindowTitle("Sequence");
    auto* lay=new QVBoxLayout(this);
    lay->addWidget(new QLabel("Click in the correct order:",this));
    QStringList correct=d.data; // correct order
    QStringList clicked;
    for(const QString& item:correct){
        auto* btn=new QPushButton(item,this);
        connect(btn,&QPushButton::clicked,this,[=,&clicked]()mutable{
            clicked<<item;
            if(clicked.size()==correct.size()){ m_solved=(clicked==correct); accept(); }
        });
        lay->addWidget(btn);
    }
}
