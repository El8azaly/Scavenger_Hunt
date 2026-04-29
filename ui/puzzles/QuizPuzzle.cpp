#include "ui/puzzles/QuizPuzzle.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
QuizPuzzle::QuizPuzzle(const PuzzleData& d, QWidget* p):PuzzleDialog(d,p){
    setWindowTitle("Puzzle");
    auto* lay=new QVBoxLayout(this);
    QString question = d.data.size()>0 ? d.data[0] : "Answer the question:";
    lay->addWidget(new QLabel(question,this));
    QStringList choices={"A","B","C","D"};
    QString correct = d.data.size()>5 ? d.data[5] : "A";
    for(int i=0;i<4&&i+1<d.data.size();i++){
        auto* btn=new QPushButton(d.data[i+1],this);
        QString ans=choices[i];
        connect(btn,&QPushButton::clicked,this,[=](){
            m_solved=(ans==correct); accept();
        });
        lay->addWidget(btn);
    }
}
