# Data Layer — Complete Implementation Guide
**Folder: `data/`**

---

## What You Are Working On

The data layer reads and writes files. It has no game logic, no UI, no drawing. It translates between disk (JSON files) and the data structures the rest of the game uses.

---

## What Is Already Done

### `data/ItemData.h` — COMPLETE, DO NOT MODIFY

The `Item` struct. This is the universal data type for any item in the game. Every other file that deals with items includes this. Fields: `id`, `name`, `description`, `iconPath`, `isQuestTarget`. The `isNull()` method returns true if `id` is empty (used as a "not found" result). `Item::null()` creates an empty item used as a "nothing" return value.

### `data/LevelLoader.h` — COMPLETE, DO NOT MODIFY

Declares three structs that are shared across the entire codebase:

**`EntitySpawnData`** — describes one entity to place in the world:
- `type` — `"platform"`, `"collectible"`, `"container"`, `"hidden_area"`
- `id` — unique string (e.g. `"chest_1"`, `"key_spawn_3"`)
- `x, y, w, h` — world-space position and size in pixels
- `properties` — a `QVariantMap` with extra data depending on type:
  - container: `"locked"` (bool), `"puzzleType"` (string), `"puzzleData"` (stringlist), `"contents"` (stringlist of item ids)
  - collectible: `"itemId"` (string)
  - hidden_area: `"requiredTriggers"` (int), `"itemIds"` (stringlist)

**`CraftingRecipe`** — one crafting combination:
- `ingredient1`, `ingredient2` — item ids of the two ingredients
- `result` — full `Item` struct for what gets crafted

**`LevelData`** — everything needed to set up a level:
- `levelNumber`, `timeLimitSec`, `worldWidth`, `worldHeight`, `playerStartX`, `playerStartY`
- `entities` — list of `EntitySpawnData`
- `targetIds` — list of item ids the player must collect to win
- `recipes` — crafting recipes active in this level
- `itemLibrary` — list of `Item` structs for all items referenced in this level (so the loader can resolve id strings to full Item objects)

---

## Files You Must Implement

---

### `data/LevelLoader.cpp` — NEEDS JSON PARSER

**Current state:** `buildLevel1()` returns a hardcoded level with one platform and one item. The `load()` function always calls this regardless of level number.

**What you must implement — the JSON parser:**

The JSON schema (agreed with the leader) looks like this for each level file:
```json
{
  "levelNumber": 1,
  "timeLimitSec": 300,
  "playerStart": { "x": 100, "y": 580 },
  "worldWidth": 5760,
  "worldHeight": 720,
  "itemLibrary": [
    {
      "id": "brass_key",
      "name": "Brass Key",
      "description": "An old brass key",
      "iconPath": ":/assets/images/brass_key.png",
      "isQuestTarget": true
    }
  ],
  "targets": ["brass_key"],
  "recipes": [
    {
      "ingredient1": "stick",
      "ingredient2": "rope",
      "resultId": "fishing_rod"
    }
  ],
  "entities": [
    { "type": "platform", "id": "floor", "x": 0, "y": 672, "w": 5760, "h": 48 },
    { "type": "collectible", "id": "key1", "x": 400, "y": 490, "itemId": "brass_key" },
    {
      "type": "container",
      "id": "chest1",
      "x": 600, "y": 624, "w": 64, "h": 48,
      "locked": false,
      "contents": ["brass_key"]
    },
    {
      "type": "container",
      "id": "drawer1",
      "x": 800, "y": 624, "w": 64, "h": 48,
      "locked": true,
      "puzzleType": "quiz",
      "puzzleData": ["What color is the sky?", "Red", "Blue", "Green", "Yellow", "B"]
    },
    {
      "type": "hidden_area",
      "id": "secret1",
      "x": 1000, "y": 600, "w": 48, "h": 72,
      "requiredTriggers": 3,
      "itemIds": ["map_piece"]
    }
  ]
}
```

**Full parser implementation:**

```cpp
LevelData LevelLoader::load(int levelNumber) {
    QString path = QString(":/data/levels/level%1.json").arg(levelNumber);
    return loadFromFile(path);
}

LevelData LevelLoader::loadFromFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "LevelLoader: cannot open" << filePath << "- using hardcoded fallback";
        return buildLevel1(); // keep the hardcoded fallback as emergency
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject root  = doc.object();

    LevelData d;
    d.levelNumber  = root["levelNumber"].toInt(1);
    d.timeLimitSec = root["timeLimitSec"].toInt(300);
    d.worldWidth   = (float)root["worldWidth"].toDouble(Constants::WORLD_WIDTH);
    d.worldHeight  = (float)root["worldHeight"].toDouble(Constants::WORLD_HEIGHT);

    QJsonObject ps = root["playerStart"].toObject();
    d.playerStartX = (float)ps["x"].toDouble(100);
    d.playerStartY = (float)ps["y"].toDouble(580);

    // Parse item library first so we can look up items by id
    QMap<QString, Item> itemMap;
    for (const QJsonValue& v : root["itemLibrary"].toArray()) {
        QJsonObject o = v.toObject();
        Item item;
        item.id            = o["id"].toString();
        item.name          = o["name"].toString();
        item.description   = o["description"].toString();
        item.iconPath      = o["iconPath"].toString();
        item.isQuestTarget = o["isQuestTarget"].toBool(false);
        d.itemLibrary.append(item);
        itemMap[item.id] = item;
    }

    // Parse targets
    for (const QJsonValue& v : root["targets"].toArray())
        d.targetIds << v.toString();

    // Parse recipes
    for (const QJsonValue& v : root["recipes"].toArray()) {
        QJsonObject o = v.toObject();
        CraftingRecipe recipe;
        recipe.ingredient1 = o["ingredient1"].toString();
        recipe.ingredient2 = o["ingredient2"].toString();
        QString resultId   = o["resultId"].toString();
        recipe.result      = itemMap.value(resultId, Item::null());
        if (!recipe.result.isNull())
            d.recipes.append(recipe);
    }

    // Parse entities
    for (const QJsonValue& v : root["entities"].toArray()) {
        QJsonObject o = v.toObject();
        EntitySpawnData e;
        e.type = o["type"].toString();
        e.id   = o["id"].toString();
        e.x    = (float)o["x"].toDouble(0);
        e.y    = (float)o["y"].toDouble(0);
        e.w    = (float)o["w"].toDouble(48);
        e.h    = (float)o["h"].toDouble(48);

        // Copy all other fields into properties for Game.cpp to read
        if (o.contains("locked"))         e.properties["locked"]          = o["locked"].toBool();
        if (o.contains("puzzleType"))     e.properties["puzzleType"]      = o["puzzleType"].toString();
        if (o.contains("itemId"))         e.properties["itemId"]          = o["itemId"].toString();
        if (o.contains("requiredTriggers")) e.properties["requiredTriggers"] = o["requiredTriggers"].toInt();

        // Parse puzzleData array
        if (o.contains("puzzleData")) {
            QStringList pdata;
            for (const QJsonValue& pv : o["puzzleData"].toArray())
                pdata << pv.toString();
            e.properties["puzzleData"] = pdata;
        }

        // Parse contents array (for containers)
        if (o.contains("contents")) {
            QStringList contents;
            for (const QJsonValue& cv : o["contents"].toArray())
                contents << cv.toString();
            e.properties["contents"] = contents;
        }

        // Parse itemIds array (for hidden areas)
        if (o.contains("itemIds")) {
            QStringList ids;
            for (const QJsonValue& iv : o["itemIds"].toArray())
                ids << iv.toString();
            e.properties["itemIds"] = ids;
        }

        d.entities.append(e);
    }

    return d;
}
```

**Who calls this and when:** `Game::startNewGame(n)` calls `LevelLoader::load(n)`. The returned `LevelData` is passed to `Game::spawnEntities(data)` which creates all the entity objects. The loader is also called by `LevelManager::getLevelData()`.

---

### `data/SaveManager.cpp` — NEEDS FULL IMPLEMENTATION

**Current state:** both `save()` and `load()` are empty functions.

**What save must do:** read the current game state and write it to a JSON file.

```cpp
void SaveManager::save(Game* game, const QString& filePath) {
    // Create saves/ directory if it doesn't exist
    QDir().mkpath(QFileInfo(filePath).absolutePath());

    QJsonObject root;
    root["levelNumber"] = game->score()->currentScore(); // wait — wrong
    // Actually: Game needs to expose currentLevel()
    // Add:  int currentLevel() const { return m_currentLevel; }  to Game.h
    root["score"]       = game->score()->currentScore();
    root["timeLeft"]    = game->score()->timeLeft();

    // Save inventory
    QJsonArray items;
    for (const Item& item : game->inventory()->allItems()) {
        QJsonObject o;
        o["id"] = item.id;
        items.append(o);
    }
    root["inventory"] = items;

    QJsonDocument doc(root);
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly))
        file.write(doc.toJson());
}
```

**What load must do:**
```cpp
void SaveManager::load(Game* game, const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return; // no save file — do nothing

    QJsonObject root = QJsonDocument::fromJson(file.readAll()).object();

    // Restore inventory (items are loaded by id — need to look up full Item from level)
    // Note: load() is called AFTER spawnEntities, so the level's itemLibrary is available
    // For now, restore only basic item data
    // Full implementation requires Game to expose a way to look up items by id from the level
}
```

**Important note for SaveManager:** You need Game to expose `m_currentLevel` via a public getter. Add `int currentLevel() const { return m_currentLevel; }` to `Game.h`. SaveManager is the only external class that needs this.

---

### Level JSON Files — `data/levels/level1.json`, `level2.json`, `level3.json`

**Current state:** All three are stub files with one line each.

**What you must write:** Real level definitions following the schema above.

**Level 1 guidelines:**
- 5 quest targets
- 300 seconds (5 minutes)
- Floor + 3-4 raised platforms
- 3 collectible items scattered around
- 1 unlocked container with 2 items inside
- 1 locked container with a quiz puzzle
- No hidden areas yet (save for level 2)
- 1 crafting recipe (combines 2 collectible items into a 3rd quest target)

**Level 2 guidelines:**
- 7 quest targets
- 240 seconds (4 minutes)
- More complex platform layout (higher jumps, gaps)
- 1 hidden area (hit 3 times to reveal)
- 2 locked containers (one quiz, one lock puzzle)
- 2 crafting recipes

**Level 3 guidelines:**
- 9 quest targets
- 180 seconds (3 minutes)
- All features: 1 quiz, 1 lock, 1 sequence puzzle, 2 hidden areas
- 3 crafting recipes (one chain: A+B→C, then C+D→E which is a quest target)

---

## Summary — What Each Function Does and Who Calls It

| Function | Who calls it | What you must return/do |
|---|---|---|
| `LevelLoader::load(n)` | Game::startNewGame(n), LevelManager::getLevelData() | Parse level JSON file, return filled LevelData struct |
| `LevelLoader::loadFromFile(path)` | LevelLoader::load() internally, also useful for testing | Same as above but from explicit path |
| `SaveManager::save(game, path)` | Game::saveGame() | Read game state, write JSON to disk |
| `SaveManager::load(game, path)` | Game::loadGame() (called at startNewGame) | Read JSON from disk, restore game state |
