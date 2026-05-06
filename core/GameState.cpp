#include "core/GameState.h"
#include <QDebug>

GameStateManager::GameStateManager(QObject* parent)
    : QObject(parent)
{}

void GameStateManager::setState(GameState newState) {
    if (newState == m_state) return;

    GameState old = m_state;
    m_state = newState;

    emit stateChanged(old, newState);
}