#pragma once
#include <QWidget>
class WinScreen : public QWidget {
    Q_OBJECT
public:
    explicit WinScreen(QWidget* parent = nullptr);
    void setFinalScore(int score);
signals:
    void nextLevelRequested();
    void exitRequested();
};