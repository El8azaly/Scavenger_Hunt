#pragma once
#include <QObject>
#include <QSet>

// ─────────────────────────────────────────────────────────────
//  InputHandler  —  translates raw Qt key codes into game actions
//
//  WHY THIS EXISTS: Qt gives us raw key codes (Qt::Key_Left, etc.)
//  but game logic should work with named actions (MOVE_LEFT) so
//  re-mapping keys never requires touching gameplay code.
// ─────────────────────────────────────────────────────────────

// All actions the game recognizes. Game code ONLY uses these;
// it never references Qt::Key_* directly.
enum class GameAction {
    MOVE_LEFT,       // A or Left arrow — held action
    MOVE_RIGHT,      // D or Right arrow — held action
    JUMP,            // W, Up arrow, or Space — held (re-jump while held)
    INTERACT,        // E — one-shot (open container / trigger hidden area)
    OPEN_INVENTORY,  // I — one-shot toggle
    OPEN_CRAFTING,   // C — one-shot toggle
    PAUSE,           // Escape — one-shot
    ATTACK           // F — one-shot (used to hit walls for hidden areas)
};

class InputHandler : public QObject
{
    Q_OBJECT
public:
    explicit InputHandler(QObject* parent = nullptr);

    // Called by GameWidget on every Qt key event
    void keyPressEvent(int qtKey);
    void keyReleaseEvent(int qtKey);

    // Query state — Game calls these inside update()
    // isHeld: true while key is held down (movement, jump)
    bool isHeld(GameAction action) const;
    // wasJustPressed: true only for ONE frame after key first goes down
    bool wasJustPressed(GameAction action) const;

    // Must be called at the END of Game::update() each frame.
    // Moves "just pressed" keys to the "held" set.
    void endFrame();

signals:
    // Fires immediately when a one-shot action key is pressed.
    // Game connects this for actions that must not be missed
    // even if update() is delayed (e.g. PAUSE during heavy frames).
    void oneShotAction(GameAction action);

private:
    QSet<int> m_heldKeys;        // currently held Qt key codes
    QSet<int> m_justPressedKeys; // pressed THIS frame only

    // Maps a Qt key code to a GameAction (returns nullopt if unmapped)
    std::optional<GameAction> keyToAction(int qtKey) const;

    // One-shot actions fire their signal and are NOT stored as held
    static bool isOneShot(GameAction action);
};
