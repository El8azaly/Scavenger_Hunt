#pragma once
#include <QString>

// ─────────────────────────────────────────────────────────────
//  Item  —  pure data struct representing any in-game item
//
//  Used by: InventorySystem, CollectibleItem, Container,
//           CraftingSystem, QuestSystem, SaveManager.
//
//  This header has NO includes other than QString.
//  Do not add Qt or game logic here.
// ─────────────────────────────────────────────────────────────
struct Item {
    QString id;           // unique key e.g. "brass_key", "map_piece_1"
    QString name;         // display name e.g. "Brass Key"
    QString description;  // shown in InventoryPanel tooltip
    QString iconPath;     // Qt resource path e.g. ":/assets/images/brass_key.png"
    bool    isQuestTarget = false; // true = QuestSystem watches for this

    bool isNull() const { return id.isEmpty(); }

    bool operator==(const Item& o) const { return id == o.id; }
    bool operator!=(const Item& o) const { return !(*this == o); }

    // Returns an empty item — used as a "not found" / "craft failed" sentinel
    static Item null() { return {}; }
};
