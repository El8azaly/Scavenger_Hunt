#include "Level2.h"

Level2::Level2(QObject* parent)
    : Level(
        ":/assets/sprites/levels/level2.png",
        ":/assets/sprites/levels/level2_collision_data.json",
        ":/assets/sprites/levels/level2_trap_data.json",
        "level2",
        std::make_unique<AnimatedSprite>(
            ":/assets/sprites/levels/level2_bottom.json",
            ":/assets/sprites/levels/level2_bottom.png"
        ),
        std::make_unique<AnimatedSprite>(
            ":/assets/sprites/levels/level2_top.json",
            ":/assets/sprites/levels/level2_top.png"
        ),
        parent
    )
{}