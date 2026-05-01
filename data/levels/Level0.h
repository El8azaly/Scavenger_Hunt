#pragma once
#include "core/Level.h"

class Level0 : public Level
{
    Q_OBJECT
public:
    explicit Level0(QObject* parent = nullptr);

    void init() override;

    // You can override layers here if you want level-specific rendering.
    // void drawFrontLayer(QPainter& painter, const Camera& camera) override;
};