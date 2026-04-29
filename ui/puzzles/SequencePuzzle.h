#pragma once
#include "ui/puzzles/PuzzleDialog.h"
class SequencePuzzle : public PuzzleDialog {
    Q_OBJECT
public:
    explicit SequencePuzzle(const PuzzleData& data, QWidget* parent = nullptr);
};
