#pragma once
#include "core/Level.h"
#include "ui/sprite/AnimatedSprite.h"

class Level3 : public Level {
    Q_OBJECT
public:
    explicit Level3(QObject* parent = nullptr);

    void update(int deltaTimeMs) override;
    void drawWindowLayer(QPainter &painter, const Camera &camera);
private:
    std::unique_ptr<AnimatedSprite> m_windowSprite;
    std::unique_ptr<AnimatedSprite> m_windowLightSprite;
};