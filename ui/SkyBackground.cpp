#include "ui/SkyBackground.h"
#include "ui/SlicedSprite.h"
#include "core/Constants.h"
#include <tuple>

SkyBackground::SkyBackground() {
    constexpr int scale = Constants::UI_SCALE;
    QList<std::tuple<int, int, QString>> configs = {
        { 20, 145, "Big" },
        { 250, 150, "Big" },
        { 40, 170, "Medium" },
        { 150, 176, "Medium" },
        { 10, 200, "Small" },
        { 190, 200, "Big" },
    };
    m_waters.reserve(configs.size());
    for (const auto& [x, y, state] : configs) {
        WaterInstance instance;
        instance = {
            new AnimatedSprite(":/assets/sprites/sky/water.json", ":/assets/sprites/sky/water.png"),
            instance.pos = QPoint(x*scale, y*scale)
        };
        instance.sprite->setState(state);
        m_waters.append(instance);
    }
}
SkyBackground::~SkyBackground() {
    for (auto& water : m_waters) {
        delete water.sprite;
    }
    m_waters.clear();
}

void SkyBackground::setCloudSpeed(float pixelsPerSecond) { m_cloudSpeed = pixelsPerSecond; }
void SkyBackground::setCloudOffsetY(int offsetY) { m_cloudOffsetY = offsetY; }

void SkyBackground::setWaterSprite(int index, int x, int y, const QString& state) {
    if (index < 0 || index >= m_waters.size()) return;

    m_waters[index].pos = QPoint(x, y);
    m_waters[index].sprite->setState(state);
}

void SkyBackground::update(int deltaTimeMs) {
    int cloudScaledW = 448 * Constants::UI_SCALE;
    m_cloudOffsetX += m_cloudSpeed * (deltaTimeMs / 1000.0f);
    if (m_cloudOffsetX >= cloudScaledW) {
        m_cloudOffsetX -= cloudScaledW;
    }

    for (auto& water : m_waters) {
        if (water.sprite) {
            water.sprite->update(deltaTimeMs);
        }
    }
}

void SkyBackground::draw(QPainter& p, int screenW, int screenH) {
    int scale = Constants::UI_SCALE;
    int skyH = 128 * scale;
    int skyY = (screenH - skyH) / 2;
    int bottomY = skyY + skyH;

    SlicedSprite("sky_top_block").draw(p, 0, 0, screenW, skyY);
    SlicedSprite("sky").draw(p, 0, skyY, screenW, skyH);
    SlicedSprite("sky_bottom_block").draw(p, 0, bottomY, screenW, screenH - bottomY);

    int cloudW = 448 * scale;
    int cloudH = 192 * scale;
    int cloudY = (screenH - cloudH) / 2 + (m_cloudOffsetY * scale);

    SlicedSprite clouds("clouds");
    for (int cx = -m_cloudOffsetX; cx < screenW; cx += cloudW) {
        clouds.draw(p, cx, cloudY, cloudW, cloudH);
    }
    int wWidth = 170 * scale;
    int wHeight = 16 * scale;
    for (const auto& water : m_waters) {
        if (water.sprite) {
            water.sprite->draw(p, QRectF(water.pos.x(), water.pos.y(), wWidth, wHeight), false);
        }
    }
}