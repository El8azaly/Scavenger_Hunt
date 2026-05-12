#pragma once
#include <QObject>
#include <QTimer>
#include "core/Constants.h"

class GameLoop : public QObject {
    Q_OBJECT
public:
    explicit GameLoop(QObject* parent = nullptr);

    void start();
    void stop();
    void resume();

    bool isRunning() const;

signals:
    void tick(int deltaTimeMs);

private:
    QTimer m_timer;
};