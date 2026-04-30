#include "core/GameState.h"
#include <QDebug>

GameStateManager::GameStateManager(QObject* parent)
    : QObject(parent)
{}

void GameStateManager::setState(GameState newState)
{
    if (newState == m_state) return; // no redundant transitions

    GameState old = m_state;
    m_state = newState;

    qDebug() << "[GameState] Transition:"
             << static_cast<int>(old) << "->"
             << static_cast<int>(newState);

    emit stateChanged(old, newState);
}
