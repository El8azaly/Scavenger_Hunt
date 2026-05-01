# Entity Layer — Complete Implementation Guide
**Folder: `entities/`**

---

## What You Are Working On

You are implementing the things that exist inside the game world — the player, the floor, the chests, the items on the ground, the hidden areas. Every one of these is a class that already has its `.h` and `.cpp` file in the repo. Some are **fully done**, some are **stubs that need real implementation**.

**The one rule you must never break:** Do NOT `#include` any UI file (`QWidget`, `QDialog`, `QLabel`, etc.) inside any entity file. Entities are pure game logic. They paint themselves with `QPainter` but they never open windows or dialogs.

---

## What Is Already Done — Do Not Touch

### `GameObject.h` / `GameObject.cpp` — COMPLETE, DO NOT MODIFY

This is the base class that every entity inherits from. It is fully written. You only read it to understand what your classes have access to.

**What it gives every entity:**
- `m_x, m_y` — world-space position (top-left corner of the entity)
- `m_w, m_h` — width and height in pixels
- `m_id` — a unique string name set by the level loader
- `m_active` — when you set this to `false`, Game.cpp will delete this entity next frame
- `boundingBox()` — returns a `QRectF` of the entity's rectangle, used by CollisionEngine
- `setPosition(x, y)` — moves the entity to a new world position
- `centreX()`, `centreY()` — the centre point of the entity (used by Camera)
- Three virtual flags: `isSolid()`, `isInteractable()`, `isCollectible()` — all return `false` by default, your subclasses override the relevant one to return `true`

### `Entity.h` / `Entity.cpp` — COMPLETE, DO NOT MODIFY

Adds physics on top of `GameObject`. Already implemented. You inherit from this for the Player.

**What it gives the Player class:**
- `m_velX, m_velY` — velocity applied every frame
- `m_onGround` — true when standing on a platform, false otherwise
- `m_facingRight` — use this to flip the sprite when moving left
- `update()` — already applies gravity, clamps fall speed, moves the entity. Calls this in Player::update() with `Entity::update()`
- `land()` — called by Game.cpp when the player touches a platform from above. Sets velY=0 and onGround=true
- `stopVertical()` — called by Game.cpp on ceiling hit. Sets velY=0
- `stopHorizontal()` — called by Game.cpp on wall hit. Sets velX=0
- `jump(velocity)` — sets velY to the given negative value (upward)
- `setVelocityX(vx)` — called by Game.cpp every frame with the movement speed
- `isOnGround()` — Game.cpp checks this before allowing a jump

### `InteractiveObject.h` / `InteractiveObject.cpp` — COMPLETE, DO NOT MODIFY

The base class for Container and HiddenArea. Already implemented.

**What it gives Container and HiddenArea:**
- `m_isLocked` — whether this object requires a puzzle before it can be opened
- `m_puzzleType` — `"quiz"`, `"lock"`, or `"sequence"`
- `m_puzzleData` — the list of strings that define the puzzle (question, choices, code, etc.)
- `m_showHint` — set to true by Game.cpp when the player is close enough. Use this in `draw()` to decide whether to show the `[E]` prompt
- `setLocked(puzzleType, puzzleData)` — called by Game.cpp during level load to configure the lock
- `unlock()` — called by Game.cpp after the player solves the puzzle. Sets m_isLocked to false
- `isInteractable()` returns `true` — CollisionEngine uses this to include this object in proximity checks

### `InteractionResult.h` — COMPLETE, DO NOT MODIFY

This is a data struct, not a class. Container and HiddenArea fill one of these and return it from `interact()`. Game.cpp reads it and decides what to do. You don't implement this — you just create and return instances of it.

**The five result types:**
- `Type::None` — nothing happened (already open, or not enough triggers yet)
- `Type::ShowPuzzle` — the object is locked. Game.cpp will show a puzzle dialog. Fill in `result.puzzle` with type, data, and objectId
- `Type::RevealItems` — the object is now open. Fill `result.revealedItems` with the items inside. Game.cpp will spawn them as CollectibleItems in the world
- `Type::RevealArea` — a HiddenArea triggered. Fill `result.revealedItems` with hidden items. Game.cpp spawns them
- `Type::ShowMessage` — put a clue string in `result.message`. Game.cpp will show it as a text overlay

---

## Files You Must Implement

---

### `entities/Player.cpp` — NEEDS IMPLEMENTATION

**Current state:** exists as a stub. Draws a blue rectangle. Physics works because Entity::update() is called.

**What you must add:**

#### 1. Sprite loading in the constructor

```cpp
Player::Player(float x, float y, InputHandler* input)
    : Entity(x, y, 40, 60), m_input(input)
{
    m_sprite.load(":/assets/images/player.png");
    // If the image has multiple frames side by side, that's the sprite sheet
}
```

The `InputHandler* input` pointer is passed in by Game.cpp. Store it but never delete it — Game.cpp owns it.

#### 2. `update()` — animation logic

```cpp
void Player::update() {
    Entity::update(); // ALWAYS call this first — it does the physics

    // Update facing direction based on velocity
    if (m_velX > 0) m_facingRight = true;
    if (m_velX < 0) m_facingRight = false;

    // Advance animation frame every 8 game frames
    if (++m_frameTimer >= 8) {
        m_frameTimer = 0;
        m_animFrame = (m_animFrame + 1) % 4;
    }
}
```

**Important:** Do NOT read input in `update()`. Game.cpp reads input before calling update and calls `setVelocityX()` and `jump()` directly. Your update() only does physics (via Entity::update()) and animation.

#### 3. `draw()` — sprite rendering

```cpp
void Player::draw(QPainter& painter, float camX, float camY) {
    float sx = m_x - camX;  // screen X = world X minus camera offset
    float sy = m_y - camY;  // screen Y = world Y minus camera offset

    if (!m_sprite.isNull()) {
        painter.save();
        if (!m_facingRight) {
            // Flip horizontally: translate to centre, scale -1 on X, translate back
            painter.translate(sx + m_w / 2, sy + m_h / 2);
            painter.scale(-1, 1);
            painter.translate(-m_w / 2, -m_h / 2);
            painter.drawPixmap(QRectF(0, 0, m_w, m_h), m_sprite, m_sprite.rect());
        } else {
            painter.drawPixmap(QRectF(sx, sy, m_w, m_h), m_sprite, m_sprite.rect());
        }
        painter.restore();
    } else {
        // Fallback if image not loaded
        painter.fillRect(QRectF(sx, sy, m_w, m_h), QColor(60, 120, 200));
    }
}
```

**How this is called:** Game.cpp calls `obj->draw(painter, camX, camY)` every frame for every entity. `camX` and `camY` are the Camera's current scroll offsets. You always subtract them to go from world position to screen position.

---

### `entities/Container.cpp` — NEEDS IMPROVEMENT

**Current state:** The logic in `interact()` is correct. The `draw()` is minimal. `addItem()` works.

**What to improve:**

#### `draw()` — make it visually clear

```cpp
void Container::draw(QPainter& painter, float camX, float camY) {
    float sx = m_x - camX;
    float sy = m_y - camY;

    // Draw the correct sprite based on open/closed state
    QPixmap& pix = m_opened ? m_openPix : m_closedPix;
    if (!pix.isNull()) {
        painter.drawPixmap(QRectF(sx, sy, m_w, m_h), pix, pix.rect());
    } else {
        // Fallback rectangles if sprites not loaded
        painter.fillRect(QRectF(sx, sy, m_w, m_h),
                         m_opened ? QColor(180, 120, 60) : QColor(120, 70, 30));
    }

    // If locked, draw a small padlock indicator
    if (m_isLocked && !m_opened) {
        painter.setPen(QColor(255, 60, 60));
        painter.setFont(QFont("Arial", 9, QFont::Bold));
        painter.drawText(QPointF(sx + m_w/2 - 4, sy - 2), "🔒");
        // Or just draw a red square as a "locked" marker
    }

    // If the player is nearby and the container is not yet opened,
    // show the [E] interaction hint above it
    if (m_showHint && !m_opened) {
        painter.setPen(Qt::yellow);
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.drawText(QPointF(sx + m_w/2 - 8, sy - 16), "[E]");
    }
}
```

**How `interact()` is called:** When the player presses E near this container, Game.cpp calls `container->interact()`. Your function must return an `InteractionResult`. Game.cpp reads the result and acts on it — you never call Game directly.

**What `interact()` returns and why:**
- If already open → return `{InteractionResult::Type::None}` — Game.cpp does nothing
- If locked → return `{Type::ShowPuzzle, puzzle}` where `puzzle.objectId = m_id` — Game.cpp emits a signal to show the puzzle dialog. After the player solves it, Game.cpp calls `unlock()` on this container and then calls `interact()` again, which will now return RevealItems
- If unlocked → set `m_opened = true`, return `{Type::RevealItems, revealedItems = m_contents}` — Game.cpp spawns each item in `revealedItems` as a CollectibleItem in the world near this container's position

---

### `entities/CollectibleItem.cpp` — NEEDS IMPROVEMENT

**Current state:** Bob animation works. Icon loading works. Fallback gold rectangle works.

**What to improve:**

#### `draw()` — scale icon correctly

```cpp
void CollectibleItem::draw(QPainter& painter, float camX, float camY) {
    float sx = m_x - camX;
    float sy = m_y - camY + m_bobOffset; // bobbing up and down

    if (!m_icon.isNull()) {
        painter.drawPixmap(QRectF(sx, sy, m_w, m_h), m_icon, m_icon.rect());
    } else {
        // Fallback: gold square with item name initial
        painter.fillRect(QRectF(sx, sy, m_w, m_h), QColor(255, 220, 0));
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.drawText(QRectF(sx, sy, m_w, m_h), Qt::AlignCenter,
                         m_item.name.isEmpty() ? "?" : m_item.name.left(1));
    }
}
```

**How `markCollected()` is called:** When the player walks over this item, Game.cpp detects the overlap and calls `item->markCollected()`. Your implementation sets `m_collected = true` and calls `setActive(false)`. Once active is false, Game.cpp deletes this object next frame. You never delete yourself.

**How `item()` is called:** Game.cpp reads `item->item()` to get the `Item` struct before calling `markCollected()`. It then passes that Item to `InventorySystem::addItem()`. Your `item()` just returns `m_item`.

**How `isCollectible()` is used:** CollisionEngine calls `obj->isCollectible()` on every entity near the player. Only this class returns `true`. That's how Game.cpp knows to try to pick this object up.

---

### `entities/HiddenArea.cpp` — NEEDS IMPROVEMENT

**Current state:** `interact()` logic is correct. `draw()` only draws when revealed.

**What to improve:**

#### Add a hit-flash visual effect

Add a private member `int m_flashTimer = 0;` to the header.

```cpp
// In interact(), after incrementing m_triggerCount:
m_flashTimer = 8; // flash for 8 frames

// In update() — currently empty, change it to:
void HiddenArea::update() {
    if (m_flashTimer > 0) m_flashTimer--;
}

// In draw():
void HiddenArea::draw(QPainter& painter, float camX, float camY) {
    float sx = m_x - camX;
    float sy = m_y - camY;

    if (m_revealed) {
        // Show a glow or open area after revealing
        painter.fillRect(QRectF(sx, sy, m_w, m_h), QColor(80, 200, 120, 80));
    } else {
        // Subtle visual hint — slightly different wall texture
        // Only draw something very faint so sharp players can notice
        painter.fillRect(QRectF(sx, sy, m_w, m_h), QColor(100, 90, 80, 30));
    }

    // Flash white when hit (player attacked this area)
    if (m_flashTimer > 0) {
        painter.fillRect(QRectF(sx, sy, m_w, m_h), QColor(255, 255, 255, 100));
    }
}
```

**How `interact()` is called:** The player presses F (Attack key) near this area. Game.cpp calls `hiddenArea->interact()`. You increment `m_triggerCount`. When it reaches `m_requiredTriggers`, set `m_revealed = true` and return `{Type::RevealArea, revealedItems = m_hiddenItems}`. Game.cpp then spawns those items. Until the count is reached, return `{Type::None}`.

---

### `entities/StaticPlatform.cpp` — NEEDS IMPROVEMENT

**Current state:** draws a brown rectangle.

**What to improve — add a tile texture look:**

```cpp
void StaticPlatform::draw(QPainter& painter, float camX, float camY) {
    float sx = m_x - camX;
    float sy = m_y - camY;

    // Fill base color
    painter.fillRect(QRectF(sx, sy, m_w, m_h), m_color);

    // Draw tile grid lines — divides the platform into TILE_SIZE segments
    painter.setPen(QPen(m_color.darker(140), 1));
    for (float tx = sx; tx < sx + m_w; tx += 48) {
        painter.drawLine(QPointF(tx, sy), QPointF(tx, sy + m_h));
    }

    // Highlight top edge (gives a "surface" feel)
    painter.setPen(QPen(m_color.lighter(150), 2));
    painter.drawLine(QPointF(sx, sy + 1), QPointF(sx + m_w, sy + 1));

    // Dark border around the whole platform
    painter.setPen(QPen(QColor(40, 25, 10), 1));
    painter.drawRect(QRectF(sx, sy, m_w, m_h));
}
```

**How `isSolid()` is used:** CollisionEngine calls `obj->isSolid()` on every entity. Only StaticPlatform returns `true`. That's the only thing that tells the physics system "the player cannot pass through this". Platforms don't need any other special logic.

---

## Summary — What Each Function Does and Who Calls It

| Function | Who calls it | What you must do |
|---|---|---|
| `Player::update()` | Game.cpp every frame | Call `Entity::update()`, then update animation frame |
| `Player::draw(painter, camX, camY)` | Game.cpp every frame | Draw sprite at `(m_x - camX, m_y - camY)` |
| `Player::jump(velocity)` | Game.cpp when jump key held + on ground | Already in Entity — do not re-implement |
| `Player::setVelocityX(vx)` | Game.cpp every frame | Already in Entity — do not re-implement |
| `Container::interact()` | Game.cpp when player presses E nearby | Return InteractionResult describing what happened |
| `Container::addItem(item)` | Game.cpp during level loading | Append item to m_contents |
| `Container::unlock()` | Game.cpp after puzzle solved | Already in InteractiveObject — do not re-implement |
| `Container::draw(painter, camX, camY)` | Game.cpp every frame | Draw chest sprite, lock indicator, [E] hint |
| `CollectibleItem::item()` | Game.cpp on player overlap | Return m_item (the Item struct) |
| `CollectibleItem::isCollected()` | Game.cpp on player overlap | Return m_collected |
| `CollectibleItem::markCollected()` | Game.cpp after picking up | Set m_collected=true, setActive(false) |
| `CollectibleItem::draw(painter, camX, camY)` | Game.cpp every frame | Draw icon at bobbing position |
| `HiddenArea::interact()` | Game.cpp when player presses F nearby | Increment trigger count, return RevealArea when count reached |
| `HiddenArea::addHiddenItem(item)` | Game.cpp during level loading | Append to m_hiddenItems |
| `HiddenArea::update()` | Game.cpp every frame | Count down flash timer |
| `HiddenArea::draw(painter, camX, camY)` | Game.cpp every frame | Draw revealed glow or subtle hint, flash on hit |
| `StaticPlatform::isSolid()` | CollisionEngine every frame | Returns true — already done, do not change |
| `StaticPlatform::draw(painter, camX, camY)` | Game.cpp every frame | Draw tiled platform rectangle |
