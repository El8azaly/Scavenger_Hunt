#include "SlidingPuzzleDialog.h"
#include <QVBoxLayout>
#include <algorithm>
#include <random>

SlidingPuzzleDialog::SlidingPuzzleDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUI();
    shufflePuzzle();
    updateButtons();
}

void SlidingPuzzleDialog::setupUI() {
    setWindowTitle("Unlock the Chest");
    setFixedSize(320, 400);
    setStyleSheet("background-color: #1a1a2e;");

    auto* layout = new QVBoxLayout(this);

    statusLabel_ = new QLabel("Arrange tiles in order to unlock the chest!", this);
    statusLabel_->setAlignment(Qt::AlignCenter);
    statusLabel_->setWordWrap(true);
    statusLabel_->setStyleSheet("color: #e0c97f; font-size: 13px; padding: 8px;");
    layout->addWidget(statusLabel_);

    auto* gridWidget  = new QWidget(this);
    auto* gridLayout  = new QGridLayout(gridWidget);
    gridLayout->setSpacing(6);
    gridLayout->setContentsMargins(20, 10, 20, 10);

    for (int r = 0; r < SIZE; ++r) {
        for (int c = 0; c < SIZE; ++c) {
            buttons_[r][c] = new QPushButton(gridWidget);
            buttons_[r][c]->setFixedSize(80, 80);
            buttons_[r][c]->setFont(QFont("Arial", 22, QFont::Bold));
            connect(buttons_[r][c], &QPushButton::clicked,
                    this, [this, r, c]() { handleTileClick(r, c); });
            gridLayout->addWidget(buttons_[r][c], r, c);
        }
    }
    layout->addWidget(gridWidget);

    auto* closeBtn = new QPushButton("Give Up", this);
    closeBtn->setFixedHeight(36);
    closeBtn->setStyleSheet(
        "QPushButton { background-color:#8b2020; color:white;"
        " border-radius:6px; font-size:13px; }"
        "QPushButton:hover { background-color:#a52828; }");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);
    layout->addWidget(closeBtn);
}

void SlidingPuzzleDialog::shufflePuzzle() {
    // Start from solved state, make 200 random valid moves → always solvable
    for (int r = 0; r < SIZE; ++r)
        for (int c = 0; c < SIZE; ++c)
            grid_[r][c] = r * SIZE + c + 1;   // 1..8
    grid_[SIZE-1][SIZE-1] = EMPTY;             // bottom-right is blank

    std::mt19937 rng(std::random_device{}());
    int er = SIZE-1, ec = SIZE-1;
    const int dr[] = {-1,1,0,0};
    const int dc[] = {0,0,-1,1};

    for (int i = 0; i < 200; ++i) {
        std::vector<std::pair<int,int>> moves;
        for (int d = 0; d < 4; ++d) {
            int nr = er+dr[d], nc = ec+dc[d];
            if (nr>=0 && nr<SIZE && nc>=0 && nc<SIZE)
                moves.push_back({nr, nc});
        }
        auto [mr, mc] = moves[rng() % moves.size()];
        std::swap(grid_[er][ec], grid_[mr][mc]);
        er = mr; ec = mc;
    }
}

void SlidingPuzzleDialog::updateButtons() {
    const QString tileStyle =
        "QPushButton { background-color:#16213e; color:#e0c97f;"
        " border:2px solid #4a3f7a; border-radius:8px; }"
        "QPushButton:hover { background-color:#0f3460; }";
    const QString emptyStyle =
        "QPushButton { background-color:#0a0a1a;"
        " border:2px dashed #333; border-radius:8px; }";

    for (int r = 0; r < SIZE; ++r) {
        for (int c = 0; c < SIZE; ++c) {
            int val = grid_[r][c];
            if (val == EMPTY) {
                buttons_[r][c]->setText("");
                buttons_[r][c]->setStyleSheet(emptyStyle);
                buttons_[r][c]->setEnabled(false);
            } else {
                buttons_[r][c]->setText(QString::number(val));
                buttons_[r][c]->setStyleSheet(tileStyle);
                buttons_[r][c]->setEnabled(true);
            }
        }
    }
}

void SlidingPuzzleDialog::handleTileClick(int row, int col) {
    if (!isAdjacentToEmpty(row, col)) return;

    auto [er, ec] = findEmpty();
    std::swap(grid_[row][col], grid_[er][ec]);
    updateButtons();

    if (isSolved()) {
        statusLabel_->setText("✓ Puzzle solved! The chest opens...");
        statusLabel_->setStyleSheet(
            "color:#4caf50; font-size:14px; font-weight:bold; padding:8px;");
        // Short delay so player can see the success message
        QTimer::singleShot(1200, this, [this]() {
            emit puzzleSolved();
            accept();
        });
    }
}

bool SlidingPuzzleDialog::isAdjacentToEmpty(int row, int col) const {
    auto [er, ec] = findEmpty();
    return (std::abs(row - er) + std::abs(col - ec)) == 1;
}

std::pair<int,int> SlidingPuzzleDialog::findEmpty() const {
    for (int r = 0; r < SIZE; ++r)
        for (int c = 0; c < SIZE; ++c)
            if (grid_[r][c] == EMPTY) return {r, c};
    return {-1, -1};
}

bool SlidingPuzzleDialog::isSolved() const {
    int expected = 1;
    for (int r = 0; r < SIZE; ++r)
        for (int c = 0; c < SIZE; ++c) {
            if (r == SIZE-1 && c == SIZE-1) return grid_[r][c] == EMPTY;
            if (grid_[r][c] != expected++) return false;
        }
    return true;
}
