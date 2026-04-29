#pragma once
#include <QWidget>
class GameOverScreen : public QWidget {
    Q_OBJECT
public:
    explicit GameOverScreen(QWidget* parent=nullptr);
signals:
    void startRequested();    // MainMenu
    void resumeRequested();   // Pause
    void restartRequested();  // Pause + GameOver
    void exitRequested();     // Pause + GameOver + Win
    void nextLevelRequested();// Win
};
