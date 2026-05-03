#include "engine/InputHandler.h"
#include <Qt>
#include <optional>

InputHandler::InputHandler(QObject* parent)
    : QObject(parent)
{}

std::optional<GameAction> InputHandler::keyToAction(int k) const
{
    switch (k) {
    case Qt::Key_A:     case Qt::Key_Left:  return GameAction::MOVE_LEFT;
    case Qt::Key_D:     case Qt::Key_Right: return GameAction::MOVE_RIGHT;
    case Qt::Key_W:     case Qt::Key_Up:
    case Qt::Key_Space:                     return GameAction::JUMP;
    case Qt::Key_E:                         return GameAction::INTERACT;
    case Qt::Key_I:                         return GameAction::OPEN_INVENTORY;
    case Qt::Key_C:                         return GameAction::OPEN_CRAFTING;
    case Qt::Key_Escape:                    return GameAction::PAUSE;
    case Qt::Key_F:                         return GameAction::ATTACK;
    default:                                return std::nullopt;
    }
}

bool InputHandler::isOneShot(GameAction action) {

    switch (action) {
    case GameAction::INTERACT:
    case GameAction::OPEN_INVENTORY:
    case GameAction::OPEN_CRAFTING:
    case GameAction::PAUSE:
    case GameAction::ATTACK:
        return true;
    default:
        return false;
    }
}

void InputHandler::keyPressEvent(int qtKey) {
    auto action = keyToAction(qtKey);
    if (!action) return;

    if (isOneShot(*action)) {

        emit oneShotAction(*action);
        m_justPressedKeys.insert(qtKey);
    } else {
        if (!m_heldKeys.contains(qtKey))
            m_justPressedKeys.insert(qtKey);
        m_heldKeys.insert(qtKey);
    }
}

void InputHandler::keyReleaseEvent(int qtKey) {
    m_heldKeys.remove(qtKey);
    m_justPressedKeys.remove(qtKey);
}

bool InputHandler::isHeld(GameAction action) const
{

    switch (action) {
    case GameAction::MOVE_LEFT:   return m_heldKeys.contains(Qt::Key_A)
                                      || m_heldKeys.contains(Qt::Key_Left);
    case GameAction::MOVE_RIGHT:  return m_heldKeys.contains(Qt::Key_D)
                                      || m_heldKeys.contains(Qt::Key_Right);
    case GameAction::JUMP:        return m_heldKeys.contains(Qt::Key_W)
                                      || m_heldKeys.contains(Qt::Key_Up)
                                      || m_heldKeys.contains(Qt::Key_Space);
    default: return false;
    }
}

bool InputHandler::wasJustPressed(GameAction action) const
{
    switch (action) {
    case GameAction::MOVE_LEFT:       return m_justPressedKeys.contains(Qt::Key_A)
                                          || m_justPressedKeys.contains(Qt::Key_Left);
    case GameAction::MOVE_RIGHT:      return m_justPressedKeys.contains(Qt::Key_D)
                                          || m_justPressedKeys.contains(Qt::Key_Right);
    case GameAction::JUMP:            return m_justPressedKeys.contains(Qt::Key_W)
                                          || m_justPressedKeys.contains(Qt::Key_Up)
                                          || m_justPressedKeys.contains(Qt::Key_Space);
    case GameAction::INTERACT:        return m_justPressedKeys.contains(Qt::Key_E);
    case GameAction::OPEN_INVENTORY:  return m_justPressedKeys.contains(Qt::Key_I);
    case GameAction::OPEN_CRAFTING:   return m_justPressedKeys.contains(Qt::Key_C);
    case GameAction::ATTACK:          return m_justPressedKeys.contains(Qt::Key_F);
    case GameAction::PAUSE:           return m_justPressedKeys.contains(Qt::Key_Escape);
    default: return false;
    }
}
void InputHandler::reset() {
    m_heldKeys.clear();
    m_justPressedKeys.clear();
}
void InputHandler::endFrame() {

    m_justPressedKeys.clear();
}