#pragma once
#include "core/Level.h"
#include "../../ui/sprite/AnimatedSprite.h"
#include <memory>

class Level0 : public Level
{
    Q_OBJECT
public:
    explicit Level0(QObject* parent = nullptr);

    void init() override;
    void update(int deltaTimeMs) override;

    void drawBackLayer(QPainter& painter, const Camera& camera) override;
    void drawFrontLayer(QPainter& painter, const Camera& camera) override;

private:
    std::unique_ptr<AnimatedSprite> m_bottomSprite;
    std::unique_ptr<AnimatedSprite> m_topSprite;
};