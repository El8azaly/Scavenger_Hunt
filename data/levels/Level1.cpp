#include "Level1.h"

Level1::Level1(QObject* parent) : Level(parent) {}

void Level1::init() {
    m_backgroundColor = QColor(30, 30, 45);
    m_levelImage.load(":/assets/sprites/levels/level1.png");
    loadCollisionData(":/assets/sprites/levels/level1_collision_data.json", "level1");
    loadTrapData(":/assets/sprites/levels/level1_trap_data.json", "level1");

    m_bottomSprite = std::make_unique<AnimatedSprite>(
        ":/assets/sprites/levels/level1_bottom.json",
        ":/assets/sprites/levels/level1_bottom.png"
    );

    m_topSprite = std::make_unique<AnimatedSprite>(
        ":/assets/sprites/levels/level1_top.json",
        ":/assets/sprites/levels/level1_top.png"
    );
}

void Level1::update(int deltaTimeMs) {
    m_animationAccumulator += deltaTimeMs;
    if (m_animationAccumulator >= 250) {
        if (m_bottomSprite) m_bottomSprite->update(m_animationAccumulator);
        if (m_topSprite) m_topSprite->update(m_animationAccumulator);
        m_animationAccumulator = 0;
    }
}

void Level1::drawBackLayer(QPainter& painter, const Camera& camera) {
    if (!m_bottomSprite) return;

    float scale = Constants::GAME_SCALE;
    float yOffset = verticalOffset();

    QRectF destRect(
        camera.toScreenX(0),
        camera.toScreenY(yOffset),
        worldWidth(),
        m_levelImage.height() * scale
    );

    m_bottomSprite->draw(painter, destRect, false);
}

void Level1::drawFrontLayer(QPainter& painter, const Camera& camera) {
    if (!m_topSprite) return;

    float scale = Constants::GAME_SCALE;
    float yOffset = verticalOffset();

    QRectF destRect(
        camera.toScreenX(0),
        camera.toScreenY(yOffset),
        worldWidth(),
        m_levelImage.height() * scale
    );

    m_topSprite->draw(painter, destRect, false);
}