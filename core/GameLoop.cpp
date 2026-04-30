#include "core/GameLoop.h"

GameLoop::GameLoop(QObject* parent)
    : QObject(parent)
{
    m_timer.setTimerType(Qt::PreciseTimer);
    connect(&m_timer, &QTimer::timeout, this, &GameLoop::tick);
}

void GameLoop::start()
{
    if (!m_timer.isActive())
        m_timer.start(Constants::FRAME_RATE_MS);
}

void GameLoop::stop()
{
    m_timer.stop();
}

void GameLoop::resume()
{
    start();
}

bool GameLoop::isRunning() const
{
    return m_timer.isActive();
}
