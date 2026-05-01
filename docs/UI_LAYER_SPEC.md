# UI Layer — Complete Implementation Guide
**Folder: `ui/` and `ui/puzzles/`**

---

## What You Are Working On

The UI layer is everything the player sees that is NOT the game world itself — menus, buttons, the score display, the inventory grid, the crafting panel, and the puzzle popups. UI files are the only place in the entire project where Qt widgets (`QWidget`, `QDialog`, `QPushButton`, `QLabel`, etc.) are allowed.

**The one rule you must never break:** UI files can READ from Game and systems using their getter methods. UI files emit signals when the player clicks something. UI files must NEVER call game logic directly on their own — they tell Game what the player wants, and Game decides what happens.

---

## What Is Already Done — Understand These Before You Touch Anything

### `ui/MainWindow.h` / `.cpp` — COMPLETE, DO NOT MODIFY UNLESS ADDING CONNECTIONS

MainWindow is the top-level Qt window. It owns the `Game` object, the `GameLoop`, and a `QStackedWidget` that holds all screens. It is the switchboard — when the game state changes, it switches which screen is visible.

**Screens and their stack indexes:**
- Index 0 → `MainMenuScreen` (shown at startup)
- Index 1 → `GameWidget` (the actual game canvas)
- Index 2 → `PauseScreen`
- Index 3 → `GameOverScreen`
- Index 4 → `WinScreen`

**How the GameLoop works:** `GameLoop` has a `QTimer` set to fire every 16ms. Each tick calls `MainWindow::onGameTick()` which calls `game->update()` then `gameWidget->update()` (which triggers a repaint). This is the heartbeat.

**How state switching works:** `Game` emits `stateChanged(oldState, newState)` whenever the state changes. `MainWindow::onStateChanged()` receives it and calls `m_stack->setCurrentIndex(n)`.

**What you may need to add to MainWindow:** When you build InventoryPanel and CraftingPanel, you'll connect their signals through MainWindow. When you implement puzzle handling, you'll add a connection from `Game::puzzleRequired` to `GameWidget`'s puzzle slot. These additions go in `connectSignals()`.

---

### `ui/GameWidget.h` / `.cpp` — COMPLETE, BUT NEEDS PUZZLE SLOT ADDED

GameWidget is the canvas where the game is drawn. It's the Qt widget that receives keyboard events and passes them to Game.

**What it already does:**
- `paintEvent()` — called by Qt automatically when `update()` is called. Fills background, then calls `game->draw(painter)` which draws everything
- `keyPressEvent(e)` — calls `game->handleKeyPress(e->key())`
- `keyReleaseEvent(e)` — calls `game->handleKeyRelease(e->key())`

**What you must add — the puzzle slot:**

Add to `GameWidget.h`:
```cpp
public slots:
    void onPuzzleRequired(const PuzzleData& data);
```

Add to `GameWidget.cpp`:
```cpp
void GameWidget::onPuzzleRequired(const PuzzleData& data) {
    // Create the right puzzle dialog type
    PuzzleDialog* dlg = PuzzleDialog::create(data, this);
    dlg->exec(); // blocks until player closes the dialog

    if (dlg->wasSolved()) {
        m_game->onPuzzleSolved(data.objectId);
    } else {
        m_game->onPuzzleFailed(data.objectId);
    }
    delete dlg;
}
```

Then in `MainWindow::connectSignals()` add:
```cpp
connect(m_game, &Game::puzzleRequired,
        m_gameWidget, &GameWidget::onPuzzleRequired);
```

**Why this matters:** When the player opens a locked container, Container::interact() returns a ShowPuzzle result. Game.cpp emits `puzzleRequired(puzzleData)`. GameWidget receives it, shows the dialog, waits for the player to answer, then calls back to Game with `onPuzzleSolved` or `onPuzzleFailed`. Game then unlocks the container and opens it.

---

### `ui/HUD.h` / `.cpp` — EXISTS, NEEDS MORE CONTENT

HUD is a static class (no instances). Game.cpp calls `HUD::draw(painter, score, inventory, quest)` every frame after drawing all entities. Because it draws last, it appears on top of everything. It does NOT subtract camera offsets — it draws in screen space so it stays fixed.

**What it already draws:**
- Semi-transparent top bar
- "Score: X" on the left
- "Time: MM:SS" in the centre (turns red under 30 seconds)
- "Items: X/Y" on the right

**What you must add:**

#### Quest item checklist (bottom-right corner):
```cpp
// After the existing top bar drawing, add:
if (quest && quest->totalTargets() > 0) {
    int panelX = Constants::WINDOW_WIDTH - 180;
    int panelY = 60;
    int lineH  = 20;

    // Semi-transparent background for the list
    p.fillRect(panelX - 8, panelY - 4,
               175, quest->totalTargets() * lineH + 12,
               QColor(0, 0, 0, 120));

    p.setFont(QFont("Arial", 11));
    QSet<QString> found = quest->foundIds();
    QSet<QString> targets = quest->targetIds();

    int row = 0;
    for (const QString& id : targets) {
        bool collected = found.contains(id);
        p.setPen(collected ? QColor(80, 220, 80) : QColor(200, 200, 200));
        QString prefix = collected ? "✓ " : "□ ";
        p.drawText(QPoint(panelX, panelY + row * lineH), prefix + id);
        row++;
    }
}
```

**Note:** `HUD::draw()` receives raw pointers — `ScoreManager*`, `InventorySystem*`, `QuestSystem*`. You call their getter methods (`score->currentScore()`, `quest->remainingTargets()`, etc.) directly. These pointers come from `Game.cpp` which owns the systems.

---

## Files You Must Build From Scratch (stubs exist but are empty)

---

### `ui/MainMenuScreen.cpp` — EXISTS, IMPROVE THE VISUAL

**Current state:** shows a "SCAVENGER HUNT" title and a "START GAME" button.

**What to improve:**
```cpp
MainMenuScreen::MainMenuScreen(QWidget* p) : QWidget(p) {
    setStyleSheet("background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
                  "stop:0 #1a1a3e, stop:1 #0d0d1f);");

    auto* lay = new QVBoxLayout(this);
    lay->setAlignment(Qt::AlignCenter);
    lay->setSpacing(20);

    auto* title = new QLabel("🔍 SCAVENGER HUNT", this);
    title->setStyleSheet("color:#f0c040;font-size:42px;font-weight:bold;"
                         "letter-spacing:4px;");
    title->setAlignment(Qt::AlignCenter);

    auto* subtitle = new QLabel("Find all items before time runs out", this);
    subtitle->setStyleSheet("color:#888;font-size:14px;");
    subtitle->setAlignment(Qt::AlignCenter);

    auto* startBtn = new QPushButton("▶  START GAME", this);
    startBtn->setFixedSize(220, 55);
    startBtn->setStyleSheet("QPushButton{background:#f0c040;color:#111;font-size:16px;"
                             "font-weight:bold;border-radius:10px;}"
                             "QPushButton:hover{background:#ffd060;}");
    connect(startBtn, &QPushButton::clicked, this, &MainMenuScreen::startRequested);

    // Add controls reminder
    auto* controls = new QLabel("A/D = Move   W/Space = Jump   E = Interact   F = Attack   I = Inventory   ESC = Pause", this);
    controls->setStyleSheet("color:#555;font-size:11px;");
    controls->setAlignment(Qt::AlignCenter);

    lay->addWidget(title);
    lay->addWidget(subtitle);
    lay->addSpacing(30);
    lay->addWidget(startBtn, 0, Qt::AlignCenter);
    lay->addSpacing(40);
    lay->addWidget(controls);
}
```

**The `startRequested` signal:** emitted when Start is clicked. MainWindow receives it and calls `game->startNewGame(1)` and `gameLoop->start()`. You emit the signal, MainWindow does the rest — you don't call Game directly.

---

### `ui/PauseScreen.cpp` — EXISTS, IMPROVE THE VISUAL

**What it already has:** Resume, Restart, Exit buttons with signals.

**What to improve — add a blurred/darkened overlay feel:**
```cpp
PauseScreen::PauseScreen(QWidget* p) : QWidget(p) {
    setStyleSheet("background:rgba(0,0,0,180);");
    // ... rest of layout
}
```

The three signals — `resumeRequested`, `restartRequested`, `exitRequested` — are already connected in MainWindow. Your only job is to make the buttons look good.

---

### `ui/GameOverScreen.cpp` — EXISTS, ADD SCORE DISPLAY

**What to add — `setFinalScore(int score)` method:**

Add a `QLabel* m_scoreLabel` member to the header. In the constructor, create it but leave it empty. In `setFinalScore(score)`:
```cpp
void GameOverScreen::setFinalScore(int score) {
    m_scoreLabel->setText(QString("Final Score: %1").arg(score));
}
```

In MainWindow, after connecting the game-over state change, call `m_gameOverScreen->setFinalScore(m_game->score()->currentScore())` when transitioning to GAME_OVER state.

---

### `ui/WinScreen.cpp` — EXISTS, ADD SCORE DISPLAY

Same pattern as GameOverScreen — add a `QLabel` that shows the final score. Also the "Next Level" button should advance to the next level. In `MainWindow::connectSignals()` the current TODO stub calls `startNewGame(1)`. Replace it with proper level advancement when you're ready.

---

### `ui/InventoryPanel.h` / `.cpp` — STUB, NEEDS FULL IMPLEMENTATION

**What this widget does:** A pop-up panel showing all items the player currently carries, displayed as a grid of icon buttons. The player can click an item to select it for crafting.

**Size and position:** This is a floating overlay on top of GameWidget. Create it as a child of GameWidget, sized 300×400px, positioned in the top-right corner.

**Full implementation:**

In `InventoryPanel.h`, add:
```cpp
private:
    InventorySystem* m_inv;
    QGridLayout*     m_grid;
    QWidget*         m_gridContainer;
```

In `InventoryPanel.cpp`:
```cpp
InventoryPanel::InventoryPanel(InventorySystem* inv, QWidget* p)
    : QWidget(p), m_inv(inv)
{
    setFixedSize(300, 400);
    setStyleSheet("background:rgba(20,20,40,220);border:1px solid #444;border-radius:8px;");

    auto* mainLay = new QVBoxLayout(this);

    auto* title = new QLabel("INVENTORY", this);
    title->setStyleSheet("color:#f0c040;font-size:14px;font-weight:bold;padding:6px;");
    title->setAlignment(Qt::AlignCenter);

    m_gridContainer = new QWidget(this);
    m_grid = new QGridLayout(m_gridContainer);
    m_grid->setSpacing(6);

    mainLay->addWidget(title);
    mainLay->addWidget(m_gridContainer);
    mainLay->addStretch();

    // Connect InventorySystem's signal so we refresh when inventory changes
    connect(m_inv, &InventorySystem::inventoryChanged, this, &InventoryPanel::refresh);

    hide(); // hidden by default, shown when player presses I
}

void InventoryPanel::refresh() {
    // Delete all existing buttons in the grid
    QLayoutItem* item;
    while ((item = m_grid->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Rebuild grid from current inventory
    QVector<Item> items = m_inv->allItems();
    for (int i = 0; i < items.size(); i++) {
        auto* btn = new QPushButton(m_gridContainer);
        btn->setFixedSize(60, 60);
        btn->setToolTip(items[i].name + "\n" + items[i].description);

        QPixmap icon(items[i].iconPath);
        if (!icon.isNull()) {
            btn->setIcon(QIcon(icon));
            btn->setIconSize(QSize(40, 40));
        } else {
            btn->setText(items[i].name.left(3));
        }
        btn->setStyleSheet("QPushButton{background:#2a2a4a;border:1px solid #555;border-radius:4px;color:white;}"
                           "QPushButton:hover{background:#3a3a6a;border-color:#f0c040;}");

        // When clicked, emit signal so CraftingPanel can receive the item
        QString itemId = items[i].id;
        connect(btn, &QPushButton::clicked, this, [=]() {
            emit itemSelectedForCraft(itemId);
        });

        m_grid->addWidget(btn, i / 4, i % 4); // 4 columns
    }

    // Fill remaining slots with empty placeholders up to at least 8 slots
    int totalSlots = std::max(8, (int)items.size() + 4);
    for (int i = items.size(); i < totalSlots; i++) {
        auto* placeholder = new QWidget(m_gridContainer);
        placeholder->setFixedSize(60, 60);
        placeholder->setStyleSheet("background:#1a1a2a;border:1px solid #333;border-radius:4px;");
        m_grid->addWidget(placeholder, i / 4, i % 4);
    }
}
```

**How it's shown/hidden:** GameWidget adds it as a child, positions it, and calls `show()`/`hide()` when the player presses I. You must add this logic to GameWidget — in `oneShotAction` handling or by connecting `Game::inventoryToggled` signal (which you'll add to Game.h).

**What `itemSelectedForCraft(itemId)` does:** emitted when the player clicks an item. CraftingPanel connects to this signal. When it receives it, it puts that item into one of its crafting slots.

---

### `ui/CraftingPanel.h` / `.cpp` — STUB, NEEDS FULL IMPLEMENTATION

**What this widget does:** Two item slots side by side, a "CRAFT" button, and a preview area showing what the result will be. The player drags/clicks items from InventoryPanel into the slots, then clicks Craft.

**Full implementation:**

In `CraftingPanel.h`, add:
```cpp
public slots:
    void receiveItem(const QString& itemId); // called when InventoryPanel emits itemSelectedForCraft

private:
    InventorySystem* m_inv;
    CraftingSystem*  m_craft;
    QString          m_slot1Id, m_slot2Id;
    QLabel*          m_slot1Label;
    QLabel*          m_slot2Label;
    QLabel*          m_resultLabel;
    QPushButton*     m_craftBtn;
    void updatePreview();
```

In `CraftingPanel.cpp`:
```cpp
CraftingPanel::CraftingPanel(InventorySystem* inv, CraftingSystem* craft, QWidget* p)
    : QWidget(p), m_inv(inv), m_craft(craft)
{
    setFixedSize(320, 200);
    setStyleSheet("background:rgba(20,20,40,220);border:1px solid #444;border-radius:8px;");

    auto* lay = new QVBoxLayout(this);

    auto* title = new QLabel("CRAFTING", this);
    title->setStyleSheet("color:#a580f0;font-size:14px;font-weight:bold;");
    title->setAlignment(Qt::AlignCenter);

    // Two slots + plus sign + result
    auto* slotsRow = new QHBoxLayout();
    m_slot1Label = new QLabel("[ Empty ]", this);
    m_slot2Label = new QLabel("[ Empty ]", this);
    m_resultLabel = new QLabel("?", this);
    for (auto* l : {m_slot1Label, m_slot2Label, m_resultLabel}) {
        l->setFixedSize(70, 70);
        l->setAlignment(Qt::AlignCenter);
        l->setStyleSheet("background:#1a1a3a;border:1px solid #555;border-radius:4px;color:white;font-size:10px;");
    }

    slotsRow->addWidget(m_slot1Label);
    slotsRow->addWidget(new QLabel("+", this));
    slotsRow->addWidget(m_slot2Label);
    slotsRow->addWidget(new QLabel("=", this));
    slotsRow->addWidget(m_resultLabel);

    m_craftBtn = new QPushButton("CRAFT", this);
    m_craftBtn->setEnabled(false);
    m_craftBtn->setStyleSheet("QPushButton{background:#a580f0;color:white;font-weight:bold;"
                               "border-radius:6px;padding:6px;}"
                               "QPushButton:disabled{background:#444;color:#777;}"
                               "QPushButton:hover:enabled{background:#c0a0ff;}");
    connect(m_craftBtn, &QPushButton::clicked, this, [this]() {
        // Tell Game to perform the craft — emit signal, don't touch inventory directly
        emit craftRequested(m_slot1Id, m_slot2Id);
        // Clear slots
        m_slot1Id.clear(); m_slot2Id.clear();
        m_slot1Label->setText("[ Empty ]");
        m_slot2Label->setText("[ Empty ]");
        m_resultLabel->setText("?");
        m_craftBtn->setEnabled(false);
    });

    lay->addWidget(title);
    lay->addLayout(slotsRow);
    lay->addWidget(m_craftBtn, 0, Qt::AlignCenter);

    hide();
}

void CraftingPanel::receiveItem(const QString& itemId) {
    // Fill first empty slot, then second
    if (m_slot1Id.isEmpty()) {
        m_slot1Id = itemId;
        Item it = m_inv->getItem(itemId);
        m_slot1Label->setText(it.name);
    } else if (m_slot2Id.isEmpty()) {
        m_slot2Id = itemId;
        Item it = m_inv->getItem(itemId);
        m_slot2Label->setText(it.name);
    }
    updatePreview();
}

void CraftingPanel::updatePreview() {
    if (!m_slot1Id.isEmpty() && !m_slot2Id.isEmpty()) {
        if (m_craft->canCraft(m_slot1Id, m_slot2Id)) {
            Item result = m_craft->craft(m_slot1Id, m_slot2Id);
            m_resultLabel->setText(result.name);
            m_craftBtn->setEnabled(true);
        } else {
            m_resultLabel->setText("✗");
            m_craftBtn->setEnabled(false);
        }
    }
}
```

Add to `CraftingPanel.h` signals section:
```cpp
signals:
    void craftRequested(const QString& id1, const QString& id2);
```

Then in Game.cpp add:
```cpp
void Game::performCraft(const QString& id1, const QString& id2) {
    Item result = m_crafting->craft(id1, id2);
    if (result.isNull()) return;
    m_inventory->removeItem(id1);
    m_inventory->removeItem(id2);
    m_inventory->addItem(result);
    m_score->addScore(Constants::SCORE_CRAFTED);
}
```

And in MainWindow connect:
```cpp
connect(m_craftingPanel, &CraftingPanel::craftRequested,
        this, [this](const QString& a, const QString& b) {
            m_game->performCraft(a, b);
        });
connect(m_inventoryPanel, &InventoryPanel::itemSelectedForCraft,
        m_craftingPanel, &CraftingPanel::receiveItem);
```

---

## Puzzle Files — `ui/puzzles/`

### `ui/puzzles/PuzzleDialog.h` / `.cpp` — COMPLETE

The base class for all puzzle dialogs. The important function is the static factory:

`PuzzleDialog::create(data, parent)` — reads `data.type` and creates the right subclass. Returns a `PuzzleDialog*` that GameWidget can call `exec()` on (blocking) and then check `wasSolved()`.

**GameWidget calls it like this:**
```cpp
PuzzleDialog* dlg = PuzzleDialog::create(data, this);
dlg->exec();                            // blocks until player answers
bool solved = dlg->wasSolved();
delete dlg;
if (solved) game->onPuzzleSolved(data.objectId);
else        game->onPuzzleFailed(data.objectId);
```

---

### `ui/puzzles/QuizPuzzle.cpp` — EXISTS, IMPROVE IT

**Current state:** displays question and 4 answer buttons. Marks solved if correct button clicked.

**What to improve — add retry feedback:**
```cpp
// When wrong answer is clicked, instead of immediately accept()-ing:
connect(btn, &QPushButton::clicked, this, [=]() {
    if (ans == correct) {
        m_solved = true;
        accept();
    } else {
        btn->setStyleSheet("QPushButton{background:#cc3333;color:white;}");
        // Show "Wrong!" message briefly, don't close
        auto* wrongLabel = new QLabel("Wrong answer! Try again.", this);
        wrongLabel->setStyleSheet("color:red;");
        lay->addWidget(wrongLabel);
    }
});
```

**Data format — what `d.data` contains:**
- `d.data[0]` = the question text
- `d.data[1]` = choice A text
- `d.data[2]` = choice B text
- `d.data[3]` = choice C text
- `d.data[4]` = choice D text
- `d.data[5]` = correct answer letter: `"A"`, `"B"`, `"C"`, or `"D"`

---

### `ui/puzzles/LockPuzzle.cpp` — EXISTS, IMPROVE IT

**Current state:** shows a text input, checks if entered code matches `d.data[0]`.

**What to improve:**
- Style the input field to look like a lock keypad
- If a hint is in `d.data[1]`, show it: `"Hint: " + d.data[1]`
- On wrong code, flash the input border red and don't close
- Allow multiple attempts

---

### `ui/puzzles/SequencePuzzle.cpp` — COMPLETE

This one is fully implemented and correct. The buttons are shown, the player clicks them in order, if the order matches `d.data` the puzzle is solved. No changes needed.

---

## Summary — What Each Function Does and Who Calls It

| Function | Who calls it | What you must do |
|---|---|---|
| `GameWidget::onPuzzleRequired(data)` | Game.cpp via Qt signal | Create PuzzleDialog::create(data), exec(), call onPuzzleSolved or onPuzzleFailed |
| `HUD::draw(painter, score, inv, quest)` | Game.cpp every frame | Draw top bar with score/time, draw quest checklist in corner |
| `InventoryPanel::refresh()` | Connected to InventorySystem::inventoryChanged | Rebuild the grid of item buttons from m_inv->allItems() |
| `InventoryPanel::itemSelectedForCraft(id)` | Emitted when player clicks an item | CraftingPanel::receiveItem connects to this |
| `CraftingPanel::receiveItem(id)` | InventoryPanel signal | Fill slot1 or slot2, call updatePreview() |
| `CraftingPanel::craftRequested(id1, id2)` | Emitted when player clicks CRAFT | MainWindow connects this to Game::performCraft |
| `GameOverScreen::setFinalScore(n)` | MainWindow on GAME_OVER transition | Update the score label display |
| `WinScreen::setFinalScore(n)` | MainWindow on WIN transition | Update the score label display |
| `PuzzleDialog::create(data, parent)` | GameWidget | Create and return correct subclass based on data.type |
| `PuzzleDialog::wasSolved()` | GameWidget after exec() | Return m_solved bool |
