#pragma once
#include "ui/puzzles/PuzzleDialog.h"
class QuizPuzzle : public PuzzleDialog {
    Q_OBJECT
public:
    explicit QuizPuzzle(const PuzzleData& data, QWidget* parent = nullptr);
};
