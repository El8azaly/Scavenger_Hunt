#pragma once
#include <QWidget>
class PauseScreen : public QWidget {
    Q_OBJECT
public:
    explicit PauseScreen(QWidget* parent=nullptr);
signals:
    void startRequested();    // MainMenu
    void resumeRequested();   // Pause
    void restartRequested();  // Pause + GameOver
    void exitRequested();     // Pause + GameOver + Win
    void nextLevelRequested();// Win
};
