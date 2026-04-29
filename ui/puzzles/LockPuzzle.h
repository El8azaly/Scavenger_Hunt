#pragma once
#include "ui/puzzles/PuzzleDialog.h"
class LockPuzzle : public PuzzleDialog {
    Q_OBJECT
public:
    explicit LockPuzzle(const PuzzleData& data, QWidget* parent = nullptr);
};
