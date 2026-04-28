#pragma once
#include <QString>
#include <QStringList>
#include "data/ItemData.h"

// ─────────────────────────────────────────────────────────────
//  InteractionResult  —  what an InteractiveObject returns when
//  the player interacts with it.
//
//  InteractiveObject::interact() builds and returns one of these.
//  Game::update() reads it and decides what to do:
//    • ShowPuzzle  → emit puzzleRequired signal → UI shows dialog
//    • RevealItems → add items to scene as CollectibleItems
//    • RevealArea  → mark a HiddenArea as visible
//    • ShowMessage → emit messageReady signal → UI shows popup
//    • None        → already opened / nothing to do
//
//  This design keeps entities completely free of UI dependencies.
// ─────────────────────────────────────────────────────────────

struct PuzzleData {
    QString      type;       // "quiz" | "sequence" | "lock"
    QStringList  data;       // quiz: [question, A, B, C, D, answer]
                             // lock: [correctCode]
                             // sequence: [id1, id2, id3, ...]
    QString      objectId;   // which Container/HiddenArea owns this puzzle
};

struct InteractionResult {
    enum class Type {
        None,        // already open, or nothing happened
        ShowPuzzle,  // display a PuzzleDialog
        RevealItems, // spread items in the scene (container opened)
        RevealArea,  // a HiddenArea became visible
        ShowMessage  // display a brief text popup (clue text)
    };

    Type              type = Type::None;
    PuzzleData        puzzle;
    QVector<Item>     revealedItems; // items that should be spawned in world
    QString           message;       // clue text for ShowMessage
};
