#pragma once
#include <QString>

struct Item {
    QString id;
    QString name;
    QString description;

    QString spriteJsonPath;
    QString spriteImagePath;

    float   spriteWidth  = 36.0f;
    float   spriteHeight = 36.0f;
    QString spriteState;
    bool    isQuestTarget = false;

    bool isNull() const { return id.isEmpty(); }

    bool operator==(const Item& o) const { return id == o.id; }
    bool operator!=(const Item& o) const { return !(*this == o); }

    static Item null() { return {}; }
};