#pragma once
#include <QString>
class Game;

class SaveManager {
public:
    static void save(Game* game, const QString& filePath);
    static void load(Game* game, const QString& filePath);
};
