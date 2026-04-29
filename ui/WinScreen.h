#pragma once
#include <QWidget>
class WinScreen : public QWidget {
    Q_OBJECT
public:
    explicit WinScreen(QWidget* parent=nullptr);
signals:
    void startRequested();    // MainMenu
    void resumeRequested();   // Pause
    void restartRequested();  // Pause + GameOver
    void exitRequested();     // Pause + GameOver + Win
    void nextLevelRequested();// Win
};
