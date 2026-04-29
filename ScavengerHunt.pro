# ═══════════════════════════════════════════════════════════════
#  ScavengerHunt.pro  —  Qt 6 project file
#  Compatible with Qt 6.2+ / Qt Creator 8+
#  C++ standard: C++17  (required for std::optional, if constexpr)
# ═══════════════════════════════════════════════════════════════

QT += core gui widgets

# If you ever add sound: QT += multimedia
# If you ever add networking/JSON (Qt6 bundles JSON in core — no extra module needed)

CONFIG += c++17
CONFIG -= app_bundle          # disable macOS .app bundle — keeps output flat

TARGET   = ScavengerHunt
TEMPLATE = app

# Treat warnings as errors so bad code surfaces fast
# Comment this out if a teammate's stub generates warnings temporarily
# QMAKE_CXXFLAGS += -Werror

# ── Include paths ───────────────────────────────────────────────
# Lets every file write  #include "core/Game.h"  from the project root
INCLUDEPATH += $$PWD

# ── Sources ─────────────────────────────────────────────────────
SOURCES += \
    main.cpp \
    \
    core/Game.cpp \
    core/GameState.cpp \
    core/GameLoop.cpp \
    \
    engine/InputHandler.cpp \
    engine/Camera.cpp \
    engine/CollisionEngine.cpp \
    \
    entities/GameObject.cpp \
    entities/Entity.cpp \
    entities/Player.cpp \
    entities/InteractiveObject.cpp \
    entities/Container.cpp \
    entities/CollectibleItem.cpp \
    entities/HiddenArea.cpp \
    entities/StaticPlatform.cpp \
    \
    systems/InventorySystem.cpp \
    systems/CraftingSystem.cpp \
    systems/QuestSystem.cpp \
    systems/LevelManager.cpp \
    systems/ScoreManager.cpp \
    \
    ui/MainWindow.cpp \
    ui/GameWidget.cpp \
    ui/MainMenuScreen.cpp \
    ui/PauseScreen.cpp \
    ui/GameOverScreen.cpp \
    ui/WinScreen.cpp \
    ui/HUD.cpp \
    ui/InventoryPanel.cpp \
    ui/CraftingPanel.cpp \
    ui/puzzles/PuzzleDialog.cpp \
    ui/puzzles/QuizPuzzle.cpp \
    ui/puzzles/SequencePuzzle.cpp \
    ui/puzzles/LockPuzzle.cpp \
    \
    data/LevelLoader.cpp \
    data/SaveManager.cpp

# ── Headers ─────────────────────────────────────────────────────
HEADERS += \
    core/Constants.h \
    core/Game.h \
    core/GameState.h \
    core/GameLoop.h \
    \
    engine/InputHandler.h \
    engine/Camera.h \
    engine/CollisionEngine.h \
    \
    entities/GameObject.h \
    entities/InteractionResult.h \
    entities/Entity.h \
    entities/Player.h \
    entities/InteractiveObject.h \
    entities/Container.h \
    entities/CollectibleItem.h \
    entities/HiddenArea.h \
    entities/StaticPlatform.h \
    \
    data/ItemData.h \
    data/LevelLoader.h \
    \
    systems/InventorySystem.h \
    systems/CraftingSystem.h \
    systems/QuestSystem.h \
    systems/LevelManager.h \
    systems/ScoreManager.h \
    \
    ui/MainWindow.h \
    ui/GameWidget.h \
    ui/MainMenuScreen.h \
    ui/PauseScreen.h \
    ui/GameOverScreen.h \
    ui/WinScreen.h \
    ui/HUD.h \
    ui/InventoryPanel.h \
    ui/CraftingPanel.h \
    ui/puzzles/PuzzleDialog.h \
    ui/puzzles/QuizPuzzle.h \
    ui/puzzles/SequencePuzzle.h \
    ui/puzzles/LockPuzzle.h \
    \
    data/SaveManager.h

# ── Qt Resources (images, level JSONs bundled into executable) ──
RESOURCES += resources.qrc

# ── Output directories (keeps build artefacts out of source tree) ─
OBJECTS_DIR = .build/obj
MOC_DIR     = .build/moc
RCC_DIR     = .build/rcc
DESTDIR     = bin
