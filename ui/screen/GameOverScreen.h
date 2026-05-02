#pragma once
#include <QWidget>
class GameOverScreen : public QWidget {
    Q_OBJECT
public:
    explicit GameOverScreen(QWidget* parent = nullptr);
    void setFinalScore(int score);
signals:
    void restartRequested();
    void exitRequested();
};