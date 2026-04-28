# Entity Layer — Developer Specification
**Owner: Person 2**
**Folder: `entities/`**

> Read this entire document before writing any code.
> Read `entities/GameObject.h` and `entities/InteractionResult.h` — both are already written. Your classes extend and use them.
> Do NOT include any Qt UI headers (QWidget, QDialog, QLabel, etc.) in any entity file.
> Do NOT call InventorySystem, QuestSystem, or any system directly. Game.cpp handles all of that.

---

## The OOP Hierarchy You Are Building

```
GameObject           (already written — do not modify)
  ├── Entity         ← you write this
  │     └── Player   ← you write this
  ├── InteractiveObject ← you write this (abstract)
  │     ├── Container    ← you write this
  │     └── HiddenArea   ← you write this
  ├── CollectibleItem  ← you write this
  └── StaticPlatform   ← you write this
```

Your professor will look at this hierarchy for the OOP grade (35 pts).
Make sure `virtual`, `override`, and `= 0` are used correctly.

---

## File 1: `entities/Entity.h` and `Entity.cpp`

### What it IS
The base class for anything that MOVES under physics (gravity + velocity).
Currently only Player inherits from it, but future NPCs would too.

### What to include
```cpp
#include "entities/GameObject.h"
#include "core/Constants.h"
```

### Member variables (all protected, so Player can access them)
```cpp
float m_velX = 0;       // horizontal velocity in pixels/frame
float m_velY = 0;       // vertical velocity (positive = downward)
bool  m_onGround = false; // true when standing on a solid platform
bool  m_facingRight = true; // used by draw() to flip sprite
```

### Methods to implement

**`void update() override`**
Call this order every frame:
1. Apply gravity: `m_velY += Constants::GRAVITY;`
2. Clamp fall speed: `if (m_velY > Constants::MAX_FALL_SPEED) m_velY = Constants::MAX_FALL_SPEED;`
3. Apply velocity to position: `m_x += m_velX; m_y += m_velY;`
4. Mark as not on ground at start of each frame: `m_onGround = false;`
   (CollisionEngine in Game.cpp will set it back to true if player lands on something)

**IMPORTANT:** Entity::update() DOES NOT handle collision. Game.cpp calls
CollisionEngine after update() and then calls land()/stopVertical()/stopHorizontal()
to fix the position. Entity just applies physics naively.

**`void draw(QPainter& painter, float camX, float camY) override`**
Entity itself is abstract-ish — draw() can be pure virtual here,
forcing each subclass to implement its own visuals. Add `= 0` if you like.

**`void land()`**
Called by Game.cpp when CollisionEngine reports fromTop collision.
Sets: `m_velY = 0; m_onGround = true;`
Player position correction (pushing out of floor) is done in Game.cpp before calling this.

**`void stopVertical()`**
Called by Game.cpp on ceiling collision. Sets: `m_velY = 0;`

**`void stopHorizontal()`**
Called by Game.cpp on wall collision. Sets: `m_velX = 0;`

**`void setVelocityX(float vx)`** — sets m_velX
**`void setVelocityY(float vy)`** — sets m_velY
**`float velX() const`** — returns m_velX
**`float velY() const`** — returns m_velY
**`bool isOnGround() const`** — returns m_onGround

---

## File 2: `entities/Player.h` and `Player.cpp`

### What it IS
The character the user controls. Reads from InputHandler, applies movement,
draws the player sprite or a colored rectangle placeholder.

### What to include
```cpp
#include "entities/Entity.h"
#include "engine/InputHandler.h"
#include <QPixmap>
```

### Constructor
```cpp
Player(float startX, float startY, InputHandler* input);
```
Store the InputHandler pointer as `m_input`. Never own it (no delete).

### Member variables
```cpp
InputHandler* m_input;          // non-owning pointer, set in constructor
QPixmap       m_spriteSheet;    // loaded from :/assets/images/player.png
int           m_animFrame = 0;  // current sprite frame index
int           m_frameTimer = 0; // counts frames to switch animation
```

### Methods to implement

**`void update() override`**
Player's update() DOES NOT read input. That's done in Game::processInput().
Why? Because Game sets m_velX and calls jump() BEFORE calling update().
So Player::update() just:
1. Calls `Entity::update()` (applies physics — always call parent)
2. Advances animation frame: increment m_frameTimer, every 8 frames increment m_animFrame

**`void draw(QPainter& painter, float camX, float camY) override`**
Screen position = world position minus camera offset:
```cpp
float sx = m_x - camX;
float sy = m_y - camY;
```
Draw the player at (sx, sy). If you have a sprite sheet, draw the correct frame.
If no sprite yet, draw a colored rectangle as placeholder:
```cpp
painter.fillRect(QRectF(sx, sy, m_w, m_h), QColor(60, 120, 200));
```
If m_facingRight is false, use painter.scale(-1, 1) to flip (remember to save/restore).

**`void jump(float velocity)`**
Sets `m_velY = velocity;` and `m_onGround = false;`
Only call this if m_onGround is true — Game.cpp already checks that.

**Animation frames (optional but recommended):**
A simple 4-frame walk cycle. If player has a sprite sheet, extract frames using:
```cpp
QRect frameRect(m_animFrame * FRAME_WIDTH, 0, FRAME_WIDTH, FRAME_HEIGHT);
painter.drawPixmap(sx, sy, m_spriteSheet, frameRect.x(), frameRect.y(),
                   frameRect.width(), frameRect.height());
```
States: idle (frame 0), walking (frames 1-3), jump (frame 4)

---

## File 3: `entities/InteractiveObject.h` and `InteractiveObject.cpp`

### What it IS
Abstract base class for anything the player presses E near.
Container and HiddenArea both inherit from it.

### What to include
```cpp
#include "entities/GameObject.h"
#include "entities/InteractionResult.h"
```

### Member variables (protected)
```cpp
bool        m_isLocked = false;
QString     m_puzzleType;   // "quiz" | "lock" | "sequence" — set if locked
QStringList m_puzzleData;   // the puzzle parameters
bool        m_showHint = false; // Game sets this when player is nearby
```

### Methods to implement

**`virtual InteractionResult interact() = 0`**
Pure virtual. Each subclass decides what happens when the player interacts.
Return an InteractionResult struct (see entities/InteractionResult.h).

**`bool isInteractable() const override { return true; }`**
Override from GameObject so CollisionEngine knows this is interactable.

**`bool isSolid() const override { return false; }`**
Players walk in front of containers, not into them.

**`void setLocked(const QString& puzzleType, const QStringList& puzzleData)`**
Sets m_isLocked = true, stores puzzle info.

**`void unlock()`**
Sets m_isLocked = false. Game.cpp calls this after puzzle is solved.

**`bool isLocked() const`** — returns m_isLocked

**`void setShowHint(bool show)`** — Game.cpp calls this each frame.
Used in draw() to show/hide the interaction prompt icon above the object.

**`void draw(QPainter& painter, float camX, float camY) override`**
InteractiveObject's draw() is abstract — subclasses must implement their own visuals.
But they should call a shared helper to draw the hint icon if m_showHint is true:
Draw a small "!" or a key icon 30px above the object centre.

---

## File 4: `entities/Container.h` and `Container.cpp`

### What it IS
A chest, drawer, cabinet, or box. Holds items that are revealed when opened.
Can be locked (requires solving a puzzle first).

### Inherits from: `InteractiveObject`

### What to include
```cpp
#include "entities/InteractiveObject.h"
#include "data/ItemData.h"
#include <QVector>
#include <QPixmap>
```

### Member variables
```cpp
QVector<Item> m_contents;   // items stored inside
bool          m_opened = false;
QPixmap       m_closedSprite; // :/assets/images/chest_closed.png
QPixmap       m_openSprite;   // :/assets/images/chest_open.png
```

### Methods to implement

**`InteractionResult interact() override`**
Logic:
```
if (m_opened):
    return InteractionResult{Type::None}
if (m_isLocked):
    PuzzleData p;
    p.type     = m_puzzleType;
    p.data     = m_puzzleData;
    p.objectId = m_id;
    return InteractionResult{Type::ShowPuzzle, puzzle=p}
else:
    m_opened = true;
    return InteractionResult{Type::RevealItems, revealedItems=m_contents}
```

**`void addItem(const Item& item)`**
Appends to m_contents. Called by Game.cpp during level loading.

**`void draw(QPainter& painter, float camX, float camY) override`**
Draw m_openSprite if m_opened, else m_closedSprite.
If sprites are not loaded yet, draw a brown rectangle.
Screen position: `float sx = m_x - camX; float sy = m_y - camY;`
If m_showHint and !m_opened, call the hint icon helper from InteractiveObject.

---

## File 5: `entities/CollectibleItem.h` and `CollectibleItem.cpp`

### What it IS
An item lying in the world. When the player walks over it, it gets collected.
This class is simpler than Container — no interaction needed, just overlap detection.

### Inherits from: `GameObject` (NOT Entity — it doesn't move)

### What to include
```cpp
#include "entities/GameObject.h"
#include "data/ItemData.h"
#include <QPixmap>
```

### Constructor
```cpp
CollectibleItem(float x, float y, const Item& item);
```
Store item data. Set m_w = 40, m_h = 40 (default item size).
Load icon: `m_icon.load(item.iconPath);`

### Member variables
```cpp
Item    m_item;
bool    m_collected = false;
QPixmap m_icon;
float   m_bobOffset = 0;    // for floating animation
int     m_bobTimer  = 0;
```

### Methods to implement

**`void update() override`**
Animate a gentle floating bob:
```cpp
m_bobTimer++;
m_bobOffset = 5.0f * std::sin(m_bobTimer * 0.08f);
```
No physics here — item doesn't fall.

**`void draw(QPainter& painter, float camX, float camY) override`**
Draw icon at screen position with bobOffset applied to Y:
```cpp
float sx = m_x - camX;
float sy = m_y - camY + m_bobOffset;
if (!m_icon.isNull())
    painter.drawPixmap(QPointF(sx, sy), m_icon.scaled(m_w, m_h));
else
    painter.fillRect(QRectF(sx, sy, m_w, m_h), QColor(255, 220, 0)); // gold
```

**`bool isCollectible() const override { return true; }`**
Overrides from GameObject so Game.cpp routes it to the pickup logic.

**`const Item& item() const`** — returns m_item

**`bool isCollected() const`** — returns m_collected

**`void markCollected()`**
Sets m_collected = true AND calls setActive(false).
Game.cpp then removes it from m_entities next frame.

---

## File 6: `entities/HiddenArea.h` and `HiddenArea.cpp`

### What it IS
An invisible trigger zone that the player "hits" (presses F near it) multiple
times. After enough hits, it reveals hidden items.
Example: hitting a cracked wall 3 times opens a secret passage.

### Inherits from: `InteractiveObject`

### What to include
```cpp
#include "entities/InteractiveObject.h"
#include "data/ItemData.h"
```

### Constructor
```cpp
HiddenArea(float x, float y, float w, float h, int requiredTriggers);
```

### Member variables
```cpp
int           m_triggerCount    = 0;
int           m_requiredTriggers;
bool          m_revealed        = false;
QVector<Item> m_hiddenItems;    // items that appear after reveal
```

### Methods to implement

**`InteractionResult interact() override`**
This is called when the player presses ATTACK (F key) near this area.
Logic:
```
m_triggerCount++
if (m_triggerCount >= m_requiredTriggers AND !m_revealed):
    m_revealed = true
    return InteractionResult{Type::RevealArea, revealedItems=m_hiddenItems}
else:
    return InteractionResult{Type::None}
```
After revealing, subsequent presses do nothing (m_revealed check).

**`void addHiddenItem(const Item& item)`**
Appends to m_hiddenItems. Called by Game.cpp during level loading.

**`void draw(QPainter& painter, float camX, float camY) override`**
If NOT revealed: draw nothing OR draw a very subtle cracked texture so player
knows there might be something here (optional visual hint).
If revealed: draw an open passage, a glow, or a distinct visual.
Debug mode: draw a semi-transparent red rectangle to verify placement.

---

## File 7: `entities/StaticPlatform.h` and `StaticPlatform.cpp`

### What it IS
Floors, walls, ceilings — solid tiles the player stands on and collides with.
This is the simplest entity in the whole project.

### Inherits from: `GameObject`

### Constructor
```cpp
StaticPlatform(float x, float y, float w, float h,
               const QColor& color = QColor(100, 80, 60));
```

### Methods to implement

**`bool isSolid() const override { return true; }`**
This is the ONLY important override. CollisionEngine checks this flag.

**`void update() override {}`**
Empty — static objects don't do anything each frame.

**`void draw(QPainter& painter, float camX, float camY) override`**
Draw a filled rectangle at screen position:
```cpp
float sx = m_x - camX;
float sy = m_y - camY;
painter.fillRect(QRectF(sx, sy, m_w, m_h), m_color);
// Optional: draw a 1px dark border for a tile look
painter.setPen(QColor(60, 40, 20));
painter.drawRect(QRectF(sx, sy, m_w, m_h));
```
Later replace with a tile sprite for the house/jungle theme.

---

## What Game.cpp Expects From You (Contract)

| Method | Called from | What must happen |
|--------|-------------|-----------------|
| `obj->update()` | Game::updateEntities() | Advances state, does NOT do collision |
| `obj->draw(p, cx, cy)` | Game::draw() | Paints at (m_x-cx, m_y-cy) |
| `obj->boundingBox()` | CollisionEngine | Returns correct QRectF |
| `obj->isSolid()` | CollisionEngine::checkSolid() | true only for StaticPlatform |
| `obj->isInteractable()` | CollisionEngine::checkProximity() | true for Container, HiddenArea |
| `obj->isCollectible()` | Game::resolveProximity() | true only for CollectibleItem |
| `item->markCollected()` | Game::resolveProximity() | sets active=false |
| `container->interact()` | Game::triggerInteraction() | returns InteractionResult |
| `container->unlock()` | Game::onPuzzleSolved() | sets m_isLocked=false |
| `entity->land()` | Game::resolveSolidCollision() | zeroes velY, sets onGround |
| `entity->stopVertical()` | Game::resolveSolidCollision() | zeroes velY |
| `entity->stopHorizontal()` | Game::resolveSolidCollision() | zeroes velX |
| `player->jump(v)` | Game::processInput() | sets velY = v |
| `player->setVelocityX(v)` | Game::processInput() | sets velX |

---

## Common Mistakes to Avoid

1. **Don't call `m_inventory->addItem()` inside any entity** — Game.cpp does that.
2. **Don't read input inside `update()`** — Game::processInput() calls setVelocityX/jump BEFORE update().
3. **Don't store camera offset in entities** — receive camX/camY in draw() parameters.
4. **Don't call `delete this`** — Game.cpp manages entity lifetimes.
5. **Don't override isSolid() to return true unless you're StaticPlatform** — walls only.
