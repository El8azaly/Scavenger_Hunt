#include "Level0.h"

Level0::Level0(QObject* parent) : Level(parent) {}

void Level0::init()
{
    m_backgroundColor = QColor(30, 30, 45);
    m_levelImage.load(":/assets/sprites/levels/level0.png");
    loadCollisionData(":/assets/sprites/levels/level0_collision_data.json", "level0");

    m_bottomSprite = std::make_unique<AnimatedSprite>(
        ":/assets/sprites/levels/level0_bottom.json",
        ":/assets/sprites/levels/level0_bottom.png"
    );

    m_topSprite = std::make_unique<AnimatedSprite>(
        ":/assets/sprites/levels/level0_top.json",
        ":/assets/sprites/levels/level0_top.png"
    );
}

void Level0::update(int deltaTimeMs)
{
    m_animationAccumulator += deltaTimeMs;
    if (m_animationAccumulator >= 250) {
        if (m_bottomSprite) m_bottomSprite->update(m_animationAccumulator);
        if (m_topSprite) m_topSprite->update(m_animationAccumulator);
        m_animationAccumulator = 0;
    }
}

void Level0::drawBackLayer(QPainter& painter, const Camera& camera)
{
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

void Level0::drawFrontLayer(QPainter& painter, const Camera& camera)
{
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