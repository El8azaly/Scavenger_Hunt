#include "systems/LevelManager.h"
LevelManager::LevelManager(QObject* p):QObject(p){}
void LevelManager::setLevel(int n){ m_current=n; emit levelChanged(n); }
void LevelManager::nextLevel(){ setLevel(m_current<totalLevels()?m_current+1:-1); }
LevelData LevelManager::getLevelData() const{ return LevelLoader::load(m_current); }
