#pragma once
#include <QWidget>
class PauseScreen : public QWidget {
    Q_OBJECT
public:
    explicit PauseScreen(QWidget* parent = nullptr);
signals:
    void resumeRequested();
    void restartRequested();
    void exitRequested();
};