#pragma once
#include <QString>

namespace Constants
{

    constexpr int WINDOW_WIDTH  = 1280;
    constexpr int WINDOW_HEIGHT = 720;
    constexpr int UI_SCALE = 3;
    constexpr int GAME_SCALE = 2;

    constexpr int   TILE_SIZE      = 48;
    constexpr float WORLD_WIDTH    = 5760;
    constexpr float WORLD_HEIGHT   = 720;

    constexpr float GRAVITY        = 0.55f;
    constexpr float MAX_FALL_SPEED = 16.0f;
    constexpr float PLAYER_SPEED   = 5.0f;
    constexpr float JUMP_VELOCITY  = -13.0f;

    constexpr int FRAME_RATE_MS    = 16;

    constexpr int INTERACT_RANGE   = 70;

    constexpr int LEVEL1_TIME_SEC  = 300;
    constexpr int LEVEL2_TIME_SEC  = 240;
    constexpr int LEVEL3_TIME_SEC  = 180;

    constexpr int SCORE_ITEM_FOUND    = 100;
    constexpr int SCORE_PUZZLE_SOLVED = 150;
    constexpr int SCORE_CRAFTED       = 75;
    constexpr int SCORE_TIME_BONUS    = 10;

    constexpr int DEFAULT_TRIGGER_COUNT = 3;

    const inline QString ASSETS_PATH = ":/assets/";
    const inline QString LEVELS_PATH = ":/data/levels/";
    const inline QString SAVES_DIR   = "saves/";
}