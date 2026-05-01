#pragma once
#include <QString>

// ─────────────────────────────────────────────────────────────
//  Item  —  pure data struct representing any in-game item
// ─────────────────────────────────────────────────────────────
struct Item {
    QString id;           // unique key e.g. "brass_key", "map_piece_1"
    QString name;         // display name e.g. "Brass Key"
    QString description;  // shown in InventoryPanel tooltip

    // World & UI Representation (Animated Sprite)
    QString spriteJsonPath;   // e.g. ":/assets/sprites/items/coin.json"
    QString spriteImagePath;  // e.g. ":/assets/sprites/items/coin.png"

    // NEW: Custom Sprite Sizes (Defaults to 36x36 if not overridden)
    float   spriteWidth  = 36.0f;
    float   spriteHeight = 36.0f;

    bool    isQuestTarget = false; // true = QuestSystem watches for this

    bool isNull() const { return id.isEmpty(); }

    bool operator==(const Item& o) const { return id == o.id; }
    bool operator!=(const Item& o) const { return !(*this == o); }

    // Returns an empty item — used as a "not found" / "craft failed" sentinel
    static Item null() { return {}; }
};