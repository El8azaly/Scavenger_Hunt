#include "ui/puzzles/PuzzleDialog.h"
#include "ui/puzzles/QuizPuzzle.h"
#include "ui/puzzles/LockPuzzle.h"
#include "ui/puzzles/SequencePuzzle.h"
PuzzleDialog::PuzzleDialog(const PuzzleData& d, QWidget* p):QDialog(p),m_data(d){}
PuzzleDialog* PuzzleDialog::create(const PuzzleData& data, QWidget* parent){
    if(data.type=="quiz")     return new QuizPuzzle(data,parent);
    if(data.type=="lock")     return new LockPuzzle(data,parent);
    if(data.type=="sequence") return new SequencePuzzle(data,parent);
    return new QuizPuzzle(data,parent); // fallback
}
