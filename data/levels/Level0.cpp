#include "Level0.h"

Level0::Level0(QObject* parent)
    : Level(
        ":/assets/sprites/levels/level0.png",
        ":/assets/sprites/levels/level0_collision_data.json",
        "",
        "level0",
        std::make_unique<AnimatedSprite>(
            ":/assets/sprites/levels/level0_bottom.json",
            ":/assets/sprites/levels/level0_bottom.png"
        ),
        std::make_unique<AnimatedSprite>(
            ":/assets/sprites/levels/level0_top.json",
            ":/assets/sprites/levels/level0_top.png"
        ),
        parent
    )
{}