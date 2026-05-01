#pragma once
#include <QString>

// ─────────────────────────────────────────────────────────────
//  Constants.h  —  single source of truth for all magic numbers
//  If you need to tune gameplay, this is the ONLY file to edit.
// ─────────────────────────────────────────────────────────────

namespace Constants
{
    // #define DEBUG_DRAW_COLLISIONS

    // ── Window ───────────────────────────────────────────────
    constexpr int WINDOW_WIDTH  = 1280;
    constexpr int WINDOW_HEIGHT = 720;
    constexpr int UI_SCALE = 3;
    constexpr int GAME_SCALE = 2;


    // ── World / Tile grid ────────────────────────────────────
    constexpr int   TILE_SIZE      = 48;   // pixels per tile cell
    constexpr float WORLD_WIDTH    = 5760; // 120 tiles wide  (pixels)
    constexpr float WORLD_HEIGHT   = 720;  // 15 tiles tall   (pixels)

    // ── Physics ──────────────────────────────────────────────
    constexpr float GRAVITY        = 0.55f;  // added to velY each frame
    constexpr float MAX_FALL_SPEED = 16.0f;  // terminal velocity
    constexpr float PLAYER_SPEED   = 5.0f;   // horizontal run speed
    constexpr float JUMP_VELOCITY  = -13.0f; // negative = upward

    // ── Frame rate ───────────────────────────────────────────
    constexpr int FRAME_RATE_MS    = 16;     // ~60 fps

    // ── Interaction ──────────────────────────────────────────
    constexpr int INTERACT_RANGE   = 70;     // pixels; must overlap + this margin

    // ── Difficulty tiers (applied per level) ─────────────────
    constexpr int LEVEL1_TIME_SEC  = 300;    // 5 min
    constexpr int LEVEL2_TIME_SEC  = 240;    // 4 min
    constexpr int LEVEL3_TIME_SEC  = 180;    // 3 min

    // ── Scoring ──────────────────────────────────────────────
    constexpr int SCORE_ITEM_FOUND    = 100;
    constexpr int SCORE_PUZZLE_SOLVED = 150;
    constexpr int SCORE_CRAFTED       = 75;
    constexpr int SCORE_TIME_BONUS    = 10;  // per remaining second at level end

    // ── Hidden-area triggers ─────────────────────────────────
    constexpr int DEFAULT_TRIGGER_COUNT = 3; // hits needed to reveal hidden area

    // ── Asset paths (Qt resource prefix) ─────────────────────
    const inline QString ASSETS_PATH = ":/assets/";
    const inline QString LEVELS_PATH = ":/data/levels/";
    const inline QString SAVES_DIR   = "saves/";
}
