#pragma once
#include <QWidget>
class MainMenuScreen : public QWidget {
    Q_OBJECT
public:
    explicit MainMenuScreen(QWidget* parent = nullptr);
signals:
    void startRequested();
};
