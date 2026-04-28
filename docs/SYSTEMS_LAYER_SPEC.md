# Systems Layer — Developer Specification
**Owner: Person 3**
**Folder: `systems/`**

> Read `data/ItemData.h` and `data/LevelLoader.h` before starting — these define the data structs your systems work with.
> Read `core/Game.h` to understand how Game constructs and connects your systems.
> Systems are QObject subclasses (where noted). They hold state and emit Qt signals.
> Systems do NOT render anything (no QPainter, no QWidget).
> Systems do NOT call each other directly — communication is through Qt signals or through Game.

---

## File 1: `systems/InventorySystem.h` and `InventorySystem.cpp`

### What it IS
The player's item bag. The single place in the entire codebase that stores
what the player currently holds. Everyone who needs to know "does the player
have X?" asks InventorySystem.

### Inherits from: `QObject`

### What to include
```cpp
#include <QObject>
#include <QVector>
#include "data/ItemData.h"
```

### Signals — these are the most important part
```cpp
signals:
    // Emitted every time an item is added. QuestSystem connects to this.
    void itemAdded(const Item& item);

    // Emitted every time an item is removed.
    void itemRemoved(const QString& itemId);

    // Emitted after any change. InventoryPanel and HUD connect to this
    // to repaint themselves without caring what changed.
    void inventoryChanged();
```

### Methods to implement

**`void addItem(const Item& item)`**
- Append to m_items
- Emit `itemAdded(item)`
- Emit `inventoryChanged()`
- If item already exists (same id) AND you want stackable items: increment a
  quantity counter instead of appending duplicate. For now, just append — no stacking.

**`bool removeItem(const QString& itemId)`**
- Find first item in m_items with matching id
- If found: remove it, emit `itemRemoved(itemId)`, emit `inventoryChanged()`, return true
- If not found: return false

**`bool hasItem(const QString& itemId) const`**
- Return true if any Item in m_items has matching id

**`Item getItem(const QString& itemId) const`**
- Return the Item with matching id, or Item::null() if not found

**`QVector<Item> allItems() const`**
- Return a copy of m_items (InventoryPanel uses this to draw the grid)

**`int itemCount() const`**
- Return m_items.size()

**`void clear()`**
- Clear m_items, emit inventoryChanged()
- Called by Game.cpp at the start of each new game/restart

### Member variables
```cpp
private:
    QVector<Item> m_items;
```

---

## File 2: `systems/CraftingSystem.h` and `CraftingSystem.cpp`

### What it IS
A pure logic class (no QObject, no signals) that knows how to combine
two items into a new one. It owns the recipe table for the current level.

### Does NOT inherit from QObject (no signals needed)

### What to include
```cpp
#include <QMap>
#include <QPair>
#include "data/ItemData.h"
#include "data/LevelLoader.h"  // for CraftingRecipe struct
```

### Data structures

Define this struct inside CraftingSystem.h (or reuse LevelLoader's CraftingRecipe):
Internally store recipes in a QMap for fast lookup:
```cpp
private:
    // Key: canonical pair (smaller id first alphabetically)
    // Value: the resulting Item
    QMap<QPair<QString,QString>, Item> m_recipes;

    // Helper to always put ids in alphabetical order
    // so craft("key","wood") == craft("wood","key")
    static QPair<QString,QString> makeKey(const QString& id1, const QString& id2);
```

### Methods to implement

**`void loadRecipes(const QVector<CraftingRecipe>& recipes)`**
Clear m_recipes. For each recipe in the list, insert:
```cpp
m_recipes[makeKey(recipe.ingredient1, recipe.ingredient2)] = recipe.result;
```
Game.cpp calls this when a level starts, so recipes can differ per level.

**`bool canCraft(const QString& id1, const QString& id2) const`**
Return m_recipes.contains(makeKey(id1, id2));
CraftingPanel calls this to show/hide the preview result.

**`Item craft(const QString& id1, const QString& id2) const`**
Return m_recipes.value(makeKey(id1, id2), Item::null());
Returns Item::null() if no recipe exists.
NOTE: CraftingSystem does NOT remove items from inventory. The caller
(Game.cpp or CraftingPanel) must call InventorySystem::removeItem() on both
ingredients and then InventorySystem::addItem() on the result.

**`QVector<QPair<QString,QString>> availableRecipesFor(const QString& id) const`**
(Optional but helpful for CraftingPanel) Returns all pairs where id1 OR id2
matches the given id, so CraftingPanel can show "combinable with..." hints.

**`static QPair<QString,QString> makeKey(const QString& id1, const QString& id2)`**
Returns {min(id1,id2), max(id1,id2)} to ensure order-independence.

---

## File 3: `systems/QuestSystem.h` and `QuestSystem.cpp`

### What it IS
Tracks the scavenger hunt target list. Knows which items the player must
collect and how many have been found. Listens to InventorySystem and emits
signals when the quest progresses.

### Inherits from: `QObject`

### What to include
```cpp
#include <QObject>
#include <QSet>
#include <QStringList>
#include "systems/InventorySystem.h"
```

### Constructor
```cpp
explicit QuestSystem(InventorySystem* inventory, QObject* parent = nullptr);
```
In the constructor body, connect InventorySystem's itemAdded signal to
QuestSystem's onItemAdded slot:
```cpp
connect(inventory, &InventorySystem::itemAdded,
        this,      &QuestSystem::onItemAdded);
```

### Signals
```cpp
signals:
    // Fired when a target item is found. HUD connects to show a checkmark.
    void targetFound(const QString& itemId);

    // Fired when ALL targets are found. Game.cpp connects to trigger WIN.
    void allTargetsFound();
```

### Methods to implement

**`void setTargets(const QStringList& targetItemIds)`**
Called by Game.cpp when a level starts.
Sets m_targetIds = QSet(targetItemIds.begin(), targetItemIds.end());
Clears m_foundIds.

**`int totalTargets() const`** — returns m_targetIds.size()

**`int foundTargets() const`** — returns m_foundIds.size()

**`bool isTarget(const QString& itemId) const`** — returns m_targetIds.contains(itemId)

**`bool isFound(const QString& itemId) const`** — returns m_foundIds.contains(itemId)

**`bool isComplete() const`** — returns m_foundIds.size() == m_targetIds.size()

**`QStringList remainingTargets() const`**
Return list of target ids not yet in m_foundIds.
HUD uses this to show the remaining item list.

**Private slot `void onItemAdded(const Item& item)`**
```cpp
void QuestSystem::onItemAdded(const Item& item) {
    if (m_targetIds.contains(item.id) && !m_foundIds.contains(item.id)) {
        m_foundIds.insert(item.id);
        emit targetFound(item.id);
        if (isComplete())
            emit allTargetsFound();
    }
}
```

### Member variables
```cpp
private:
    QSet<QString> m_targetIds;  // all ids the player must find
    QSet<QString> m_foundIds;   // ids already in inventory
```

---

## File 4: `systems/LevelManager.h` and `LevelManager.cpp`

### What it IS
Tracks which level the game is on. Provides level metadata.
Does NOT directly create or destroy entities — Game.cpp does that.

### Inherits from: `QObject`

### What to include
```cpp
#include <QObject>
#include "data/LevelLoader.h"
```

### Signals
```cpp
signals:
    // Game.cpp connects to this to reload the world when level changes
    void levelChanged(int newLevel);
```

### Methods to implement

**`void setLevel(int n)`**
Sets m_currentLevel = n, emit levelChanged(n).

**`void nextLevel()`**
If m_currentLevel < totalLevels(): setLevel(m_currentLevel + 1);
Else: emit levelChanged(-1) as a "no more levels" signal — Game.cpp
treats this as all levels complete (ultimate WIN).

**`int currentLevel() const`** — returns m_currentLevel

**`int totalLevels() const`** — return 3 (hardcoded for now; can read from a config later)

**`LevelData getLevelData() const`**
Returns LevelLoader::load(m_currentLevel);
Game.cpp calls this when building the entity list.

**`int timeLimitForLevel(int n) const`**
Switch on n:
- 1 → Constants::LEVEL1_TIME_SEC
- 2 → Constants::LEVEL2_TIME_SEC
- 3 → Constants::LEVEL3_TIME_SEC
- default → 240

---

## File 5: `systems/ScoreManager.h` and `ScoreManager.cpp`

### What it IS
Owns the countdown timer and the score counter. Emits signals when
these values change so the HUD can update without polling.

### Inherits from: `QObject`

### What to include
```cpp
#include <QObject>
#include <QTimer>
```

### Signals
```cpp
signals:
    void scoreChanged(int newScore);     // emitted whenever score changes
    void timeChanged(int secondsLeft);   // emitted every second
    void timeUp();                        // emitted when timer hits 0
```

### Member variables
```cpp
private:
    int    m_score    = 0;
    int    m_timeLeft = 0;
    QTimer m_timer;         // fires every 1000 ms (1 second)
```

### Methods to implement

**Constructor**
```cpp
ScoreManager::ScoreManager(QObject* parent) : QObject(parent) {
    m_timer.setInterval(1000); // 1 second
    connect(&m_timer, &QTimer::timeout, this, &ScoreManager::onTimerTick);
}
```

**`void startTimer(int seconds)`**
Sets m_timeLeft = seconds, starts m_timer.

**`void stopTimer()`**
Calls m_timer.stop(). Called on pause, game over, win.

**`void addScore(int points)`**
m_score += points; emit scoreChanged(m_score);

**`int currentScore() const`** — returns m_score

**`int timeLeft() const`** — returns m_timeLeft

**`void reset()`**
m_score = 0; m_timeLeft = 0; m_timer.stop();
emit scoreChanged(0); emit timeChanged(0);

**Private slot `void onTimerTick()`**
```cpp
void ScoreManager::onTimerTick() {
    if (m_timeLeft <= 0) {
        m_timer.stop();
        emit timeUp();
        return;
    }
    m_timeLeft--;
    emit timeChanged(m_timeLeft);
}
```

---

## How Systems Connect to Each Other

```
InventorySystem::itemAdded  ──────→  QuestSystem::onItemAdded
                                          │
                                          ├─ emit targetFound(id)
                                          └─ emit allTargetsFound() ──→ Game.cpp WIN
InventorySystem::inventoryChanged ──→  InventoryPanel::refresh()
ScoreManager::timeChanged ──────────→  HUD::updateTimer()
ScoreManager::timeUp ───────────────→  Game.cpp GAME_OVER
ScoreManager::scoreChanged ─────────→  HUD::updateScore()
```

These connections are made in Game.cpp's constructor. Your systems just need
to emit the right signals at the right time — the wiring is already written.

---

## What Game.cpp Calls on Your Systems

| Call | When | What must happen |
|------|------|-----------------|
| `m_inventory->clear()` | new game / restart | Wipes all items, emits inventoryChanged |
| `m_inventory->addItem(item)` | item collected | Adds item, emits itemAdded + inventoryChanged |
| `m_inventory->removeItem(id)` | crafting | Removes item, emits itemRemoved + inventoryChanged |
| `m_inventory->hasItem(id)` | crafting check | Returns bool (no side effects) |
| `m_crafting->loadRecipes(list)` | level start | Loads recipe table for this level |
| `m_crafting->canCraft(a,b)` | CraftingPanel | Returns bool (no side effects) |
| `m_crafting->craft(a,b)` | CraftingPanel submit | Returns result Item |
| `m_quest->setTargets(ids)` | level start | Sets target list, clears found set |
| `m_quest->isComplete()` | Win check | Returns bool |
| `m_score->reset()` | new game / restart | Zeroes score and time |
| `m_score->startTimer(secs)` | level start | Begins countdown |
| `m_score->stopTimer()` | pause / win / gameover | Halts countdown |
| `m_score->addScore(n)` | item collected, puzzle solved | Adds points |
| `m_score->timeLeft()` | win (bonus calc) | Returns seconds remaining |
