#pragma once
#include <QObject>
#include <QSet>
#include <optional>

enum class GameAction {
    MOVE_LEFT,
    MOVE_RIGHT,
    JUMP,
    INTERACT,
    OPEN_INVENTORY,
    OPEN_CRAFTING,
    PAUSE,
    ATTACK
};

class InputHandler : public QObject
{
    Q_OBJECT
public:
    explicit InputHandler(QObject* parent = nullptr);

    void keyPressEvent(int qtKey);
    void keyReleaseEvent(int qtKey);

    bool isHeld(GameAction action) const;

    bool wasJustPressed(GameAction action) const;

    void endFrame();

signals:

    void oneShotAction(GameAction action);

private:
    QSet<int> m_heldKeys;
    QSet<int> m_justPressedKeys;

    std::optional<GameAction> keyToAction(int qtKey) const;

    static bool isOneShot(GameAction action);
};