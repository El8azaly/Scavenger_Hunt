#include "Level1.h"

Level1::Level1(QObject* parent)
    : Level(
        ":/assets/sprites/levels/level1.png",
        ":/assets/sprites/levels/level1_collision_data.json",
        ":/assets/sprites/levels/level1_trap_data.json",
        "level1",
        std::make_unique<AnimatedSprite>(
            ":/assets/sprites/levels/level1_bottom.json",
            ":/assets/sprites/levels/level1_bottom.png"
        ),
        std::make_unique<AnimatedSprite>(
            ":/assets/sprites/levels/level1_top.json",
            ":/assets/sprites/levels/level1_top.png"
        ),
        parent
    )
{}