#pragma once
#include <QString>
#include <QStringList>
#include "data/ItemData.h"

struct PuzzleData {
    QString      type;
    QStringList  data;

    QString      objectId;
};

struct InteractionResult {
    enum class Type {
        None,
        ShowPuzzle,
        RevealItems,
        RevealArea,
        ShowMessage
    };

    Type              type = Type::None;
    PuzzleData        puzzle;
    QVector<Item>     revealedItems;
    QString           message;
};