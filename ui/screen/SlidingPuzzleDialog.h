#pragma once
#include <QDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <array>

class SlidingPuzzleDialog : public QDialog {
    Q_OBJECT

public:
    explicit SlidingPuzzleDialog(QWidget* parent = nullptr);

signals:
    void puzzleSolved();

private:
    static constexpr int SIZE  = 3;   // 3x3 = 8-puzzle (easy)
    static constexpr int EMPTY = 0;

    std::array<std::array<int, SIZE>, SIZE> grid_;
    QPushButton* buttons_[SIZE][SIZE];
    QLabel*      statusLabel_;

    void setupUI();
    void shufflePuzzle();
    void updateButtons();
    void handleTileClick(int row, int col);
    bool isSolved() const;
    bool isAdjacentToEmpty(int row, int col) const;
    std::pair<int,int> findEmpty() const;
};
