cmake_minimum_required(VERSION 3.16)
project(ScavengerHunt VERSION 1.0 LANGUAGES CXX) #
set(CMAKE_CXX_STANDARD 17) #
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets) #
file(GLOB_RECURSE PROJECT_SOURCES
        CONFIGURE_DEPENDS
        "core/*.cpp" "core/*.h"
        "engine/*.cpp" "engine/*.h"
        "entities/*.cpp" "entities/*.h"
        "systems/*.cpp" "systems/*.h"
        "ui/*.cpp" "ui/*.h"
        "data/*.cpp" "data/*.h"
)
list(APPEND PROJECT_SOURCES
        main.cpp
        resources.qrc
)

add_executable(ScavengerHunt ${PROJECT_SOURCES}
        engine/AnimatedSprite.cpp
        engine/AnimatedSprite.h
        ui/PixelFont.cpp
        ui/PixelFont.h
        ui/UISpriteSheet.cpp
        ui/UISpriteSheet.h
        ui/SlicedSprite.cpp
        ui/SlicedSprite.h
        ui/SpriteButton.cpp
        ui/SpriteButton.h)
target_include_directories(ScavengerHunt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ScavengerHunt PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)

set_target_properties(ScavengerHunt PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)