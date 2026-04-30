#include "ui/puzzles/SequencePuzzle.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSharedPointer>

SequencePuzzle::SequencePuzzle(const PuzzleData& d, QWidget* p)
    : PuzzleDialog(d, p)
{
    setWindowTitle("Sequence Puzzle");
    auto* lay = new QVBoxLayout(this);
    lay->addWidget(new QLabel("Click the items in the correct order:", this));

    QStringList correct = d.data;

    // Store clicked list on heap so lambda captures are safe
    auto clicked = QSharedPointer<QStringList>::create();

    for (const QString& item : correct) {
        auto* btn = new QPushButton(item, this);
        connect(btn, &QPushButton::clicked, this, [=]() mutable {
            *clicked << item;
            if (clicked->size() == correct.size()) {
                m_solved = (*clicked == correct);
                accept();
            }
        });
        lay->addWidget(btn);
    }
}
