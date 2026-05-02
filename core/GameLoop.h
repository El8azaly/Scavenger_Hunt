#pragma once
#include <QObject>
#include <QTimer>
#include "core/Constants.h"

// GameLoop owns the QTimer that drives the entire game.
// It emits tick() every ~16 ms (60 fps).
// GameWidget connects tick() -> repaint(), and Game::update() is called inside paintEvent.
// To pause the game: call stop(). To resume: call start().
class GameLoop : public QObject
{
    Q_OBJECT
public:
    explicit GameLoop(QObject* parent = nullptr);

    void start();    // begins ticking at FRAME_RATE_MS intervals
    void stop();     // halts ticking (used for PAUSED / GAME_OVER states)
    void resume();   // alias for start(); named for clarity at call sites

    bool isRunning() const;

signals:
    void tick(int deltaTimeMs); // emitted every frame; GameWidget connects this to update()

private:
    QTimer m_timer;
};
