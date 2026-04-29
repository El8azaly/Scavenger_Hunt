#pragma once
#include <QDialog>
#include "entities/InteractionResult.h"

class PuzzleDialog : public QDialog {
    Q_OBJECT
public:
    explicit PuzzleDialog(const PuzzleData& data, QWidget* parent = nullptr);
    // Factory: creates the right subclass based on data.type
    static PuzzleDialog* create(const PuzzleData& data, QWidget* parent = nullptr);
    bool wasSolved() const { return m_solved; }
protected:
    PuzzleData m_data;
    bool       m_solved = false;
};
