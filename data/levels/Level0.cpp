#include "Level0.h"

Level0::Level0(QObject* parent) : Level(parent) {}

void Level0::init()
{
    // Define the solid color that surrounds the centered level
    m_backgroundColor = QColor(30, 30, 45); // Dark slate/blue

    // Load image first, so loadCollisionData can accurately compute the vertical offset
    m_levelImage.load(":/assets/sprites/levels/level0.png");
    loadCollisionData(":/assets/sprites/levels/level0_collision_data.json", "level0");
}