#pragma once
#include <QObject>
#include "data/LevelLoader.h"

class LevelManager : public QObject {
    Q_OBJECT
public:
    explicit LevelManager(QObject* parent = nullptr);
    void setLevel(int n);
    void nextLevel();
    int  currentLevel()  const { return m_current; }
    int  totalLevels()   const { return 3; }
    LevelData getLevelData() const;
signals:
    void levelChanged(int newLevel);
private:
    int m_current = 1;
};