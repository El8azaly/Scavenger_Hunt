# Systems Layer — Complete Implementation Guide
**Folder: `systems/`**

---

## What You Are Working On

Systems are the game's rule engine. They track state — what items the player has, what the score is, how much time is left, which targets are found. They never draw anything. They never touch any UI file. They communicate entirely through Qt signals that other parts of the game connect to.

**The one rule you must never break:** Do NOT `#include` any UI file inside any system file. Do NOT call `InventorySystem` from `CraftingSystem` or vice versa — they communicate only through Game.cpp.

---

## What Is Already Done — Verify These Work

All system files exist and compile. The core logic is written. Your job is to verify each one works correctly, and then understand exactly how they are used so you can answer any question about them.

---

### `systems/InventorySystem.h` / `.cpp` — COMPLETE, VERIFY IT WORKS

The player's item bag. This is the most connected system in the project — everything else depends on it.

**What it holds:**
- `m_items` — a `QVector<Item>` of everything the player currently carries

**Methods — all already implemented:**

`addItem(item)` — appends the item to m_items, then emits two signals:
- `itemAdded(item)` — QuestSystem is connected to this. Every time an item is added, QuestSystem automatically checks if it's a quest target
- `inventoryChanged()` — InventoryPanel will be connected to this to refresh its display

`removeItem(itemId)` — removes the first item with that id, emits `itemRemoved` and `inventoryChanged`. Returns `true` if found, `false` if not. Used by Game.cpp after crafting — it removes the two ingredients before adding the crafted result.

`hasItem(itemId)` — returns true if the item is in the bag. Used by Game.cpp and CraftingPanel to check if ingredients exist before crafting.

`getItem(itemId)` — returns the full `Item` struct for an item. Returns `Item::null()` if not found.

`allItems()` — returns a copy of the whole list. InventoryPanel uses this to draw the grid.

`clear()` — empties the bag. Called by Game.cpp at the start of every new game or restart.

**The signals — this is the key part:**
```
itemAdded    → QuestSystem::onItemAdded   (auto-connected in QuestSystem constructor)
inventoryChanged → InventoryPanel::refresh  (you connect this in InventoryPanel)
```

---

### `systems/QuestSystem.h` / `.cpp` — COMPLETE, VERIFY IT WORKS

Watches InventorySystem and tracks which hunt targets have been found.

**What it holds:**
- `m_targetIds` — the set of item IDs the player must collect to win (e.g. `{"brass_key", "map_piece"}`)
- `m_foundIds` — the set of target IDs the player has already collected

**How it's wired:** In its constructor, QuestSystem connects to `InventorySystem::itemAdded`. This means every time an item enters the inventory, QuestSystem's `onItemAdded` slot fires automatically — no manual calling needed.

**`onItemAdded(item)` — the key slot (private, called by signal):**
Checks if `item.id` is in `m_targetIds` and not yet in `m_foundIds`. If yes: adds it to `m_foundIds`, emits `targetFound(item.id)`. Then checks if all targets are found — if yes, emits `allTargetsFound()`.

**The signals:**
```
targetFound(itemId)   → HUD can connect this to show a checkmark on the item list
allTargetsFound()     → Game.cpp is connected to this — it triggers the WIN state
```

**Methods used by HUD:**
- `totalTargets()` — how many items needed total
- `foundTargets()` — how many collected so far
- `remainingTargets()` — list of item IDs not yet collected. HUD uses this to display what's left
- `targetIds()` / `foundIds()` — full sets. Used to draw checkmarks vs unchecked items

---

### `systems/ScoreManager.h` / `.cpp` — COMPLETE, VERIFY IT WORKS

Owns the countdown timer and the score counter.

**What it holds:**
- `m_score` — current score integer
- `m_timeLeft` — seconds remaining
- `m_timer` — a QTimer that fires every 1000ms (1 second)

**Methods — all already implemented:**

`startTimer(seconds)` — sets m_timeLeft and starts the QTimer. Called by Game.cpp at level start.

`stopTimer()` — stops the QTimer. Called by Game.cpp on pause, win, or game over.

`addScore(points)` — adds to m_score, emits `scoreChanged(newScore)`. Called by Game.cpp whenever points are earned (item collected, puzzle solved, etc.).

`reset()` — zeros score and time, stops timer. Called by Game.cpp at start of every new game.

`timeLeft()` — read by Game.cpp when calculating the time bonus on win.

**The signals:**
```
scoreChanged(int)   → HUD connects this to update score display
timeChanged(int)    → HUD connects this to update timer display
timeUp()            → Game.cpp connects this to trigger GAME_OVER state
```

**`onTick()` (private slot):** fires every second. Decrements `m_timeLeft`. If it hits 0, stops the timer and emits `timeUp()`.

---

### `systems/CraftingSystem.h` / `.cpp` — COMPLETE, VERIFY IT WORKS

A pure logic class (no QObject, no signals) that knows which items combine into what.

**What it holds:**
- `m_recipes` — a `QMap` where the key is a sorted pair of item IDs, and the value is the resulting Item

**Methods — all already implemented:**

`loadRecipes(recipes)` — clears m_recipes and loads a new set. Called by Game.cpp at the start of every level. Level 1 might have 1 recipe, level 2 might add 3 more — the JSON defines this.

`canCraft(id1, id2)` — returns true if a recipe exists for this combination. Order doesn't matter: `canCraft("rope", "stick")` == `canCraft("stick", "rope")`.

`craft(id1, id2)` — returns the resulting `Item`. Returns `Item::null()` if no recipe exists. **This does NOT touch the inventory.** Removing the ingredients and adding the result is done by Game.cpp after calling this.

**How CraftingPanel uses it:** CraftingPanel calls `canCraft(slot1Id, slot2Id)` to decide whether to enable the Craft button. When the player clicks Craft, CraftingPanel emits a signal. Game.cpp receives that signal and then calls: `craft(id1, id2)` to get result, `inventory->removeItem(id1)`, `inventory->removeItem(id2)`, `inventory->addItem(result)`, `score->addScore(SCORE_CRAFTED)`.

---

### `systems/LevelManager.h` / `.cpp` — COMPLETE, VERIFY IT WORKS

Tracks which level the game is on. Mostly informational.

**Methods:**
- `setLevel(n)` — sets current level, emits `levelChanged(n)`
- `nextLevel()` — calls `setLevel(n+1)`. If past the last level, calls `setLevel(-1)` as a signal that all levels are done
- `getLevelData()` — calls `LevelLoader::load(m_current)` and returns the full level data struct

**Signal:**
```
levelChanged(int)  → Game.cpp can connect this to handle level transitions
```

---

## What Is NOT Yet Done — You Must Add This

### Connect ScoreManager and QuestSystem signals to HUD

Right now the HUD calls `score->currentScore()` and `score->timeLeft()` directly every frame (by reading pointers passed to it). This works but is polling. For the InventoryPanel to update automatically, you need to connect `inventoryChanged` to `InventoryPanel::refresh`. This connection is made in the UI layer (not here), but you need to make sure `inventoryChanged` always emits when items change — which it already does.

### Verify: QuestSystem correctly detects completion

After Game.cpp calls `quest->setTargets({"brass_key"})` at level start, and then the player picks up the brass_key (which causes `inventory->addItem(brassKey)` which emits `itemAdded(brassKey)` which triggers `quest->onItemAdded(brassKey)`), the QuestSystem must:
1. Check `m_targetIds.contains("brass_key")` → true
2. Insert into m_foundIds
3. Emit `targetFound("brass_key")`
4. Check `isComplete()` → true
5. Emit `allTargetsFound()`
6. Game.cpp receives `allTargetsFound` → sets WIN state

Test this manually: run the game, collect the brass key, confirm the win screen appears.

---

## Summary — What Each Function Does and Who Calls It

| Function | Who calls it | What you must return/do |
|---|---|---|
| `InventorySystem::addItem(item)` | Game.cpp when player walks over CollectibleItem | Add to m_items, emit itemAdded + inventoryChanged |
| `InventorySystem::removeItem(id)` | Game.cpp after crafting (twice — once per ingredient) | Remove item, emit itemRemoved + inventoryChanged, return bool |
| `InventorySystem::hasItem(id)` | CraftingPanel before allowing craft | Return bool, no side effects |
| `InventorySystem::allItems()` | InventoryPanel::refresh() | Return copy of m_items |
| `InventorySystem::clear()` | Game.cpp on new game / restart | Clear m_items, emit inventoryChanged |
| `CraftingSystem::loadRecipes(list)` | Game.cpp at level start | Replace m_recipes with new recipe table |
| `CraftingSystem::canCraft(a, b)` | CraftingPanel to enable/disable Craft button | Return bool, no side effects |
| `CraftingSystem::craft(a, b)` | Game.cpp after player confirms craft | Return result Item (or Item::null() if no recipe) |
| `QuestSystem::setTargets(ids)` | Game.cpp at level start | Replace m_targetIds, clear m_foundIds |
| `QuestSystem::remainingTargets()` | HUD to display what's left to find | Return QStringList of unfound target IDs |
| `QuestSystem::foundTargets()` | HUD for "X/Y items" display | Return m_foundIds.size() |
| `QuestSystem::totalTargets()` | HUD for "X/Y items" display | Return m_targetIds.size() |
| `ScoreManager::startTimer(secs)` | Game.cpp at level start | Set m_timeLeft, start QTimer |
| `ScoreManager::stopTimer()` | Game.cpp on pause/win/gameover | Stop QTimer |
| `ScoreManager::addScore(pts)` | Game.cpp on item collected, puzzle solved, craft | Add to m_score, emit scoreChanged |
| `ScoreManager::timeLeft()` | Game.cpp at win (for time bonus) | Return m_timeLeft |
| `ScoreManager::currentScore()` | HUD every frame (via pointer) | Return m_score |
