#include "SkyBackground.h"
#include "SlicedSprite.h"
#include "core/Constants.h"
#include <tuple>
#include <cmath>

float SkyBackground::s_cloudOffsetX = 0.0f;
float SkyBackground::s_shipX = 0.0f;
bool SkyBackground::s_shipInitialized = false;

SkyBackground::SkyBackground() {
    constexpr int scale = Constants::UI_SCALE;
    QList<std::tuple<int, int, QString>> configs = {
        { 20, 145, "Big" }, { 250, 150, "Big" }, { 40, 170, "Medium" },
        { 150, 176, "Medium" }, { 10, 200, "Small" }, { 190, 200, "Big" },
    };
    m_waters.reserve(configs.size());
    for (const auto& [x, y, state] : configs) {
        WaterInstance instance;
        instance.sprite = new AnimatedSprite(":/assets/sprites/sky/water.json", ":/assets/sprites/sky/water.png");
        instance.pos = QPoint(x * scale, y * scale);
        instance.sprite->setState(state);
        m_waters.append(instance);
    }

    m_shipSprite = new AnimatedSprite(":/assets/sprites/sky/ship.json", ":/assets/sprites/sky/ship.png");
    m_waterWavesSprite = new AnimatedSprite(":/assets/sprites/sky/water_waves.json", ":/assets/sprites/sky/water_waves.png");
}

SkyBackground::~SkyBackground() {
    for (auto& water : m_waters) delete water.sprite;
    delete m_shipSprite;
    delete m_waterWavesSprite;
}

void SkyBackground::update(int deltaTimeMs) {
    int cloudScaledW = 448 * Constants::UI_SCALE;
    s_cloudOffsetX += m_cloudSpeed * (deltaTimeMs / 1000.0f);
    if (s_cloudOffsetX >= cloudScaledW) s_cloudOffsetX -= cloudScaledW;

    for (auto& water : m_waters) if (water.sprite) water.sprite->update(deltaTimeMs);

    if (m_shipSprite) {
        m_shipSprite->update(deltaTimeMs);
        s_shipX += (m_cloudSpeed * 1.5f) * (deltaTimeMs / 1000.0f);
    }
    if (m_waterWavesSprite) m_waterWavesSprite->update(deltaTimeMs);
}
void SkyBackground::draw(QPainter& p, int screenW, int screenH, float cameraX) {
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

    float cloudRenderX = std::fmod(s_cloudOffsetX + (cameraX * 0.5f), (float)cloudW);
    for (float cx = -cloudRenderX; cx < screenW; cx += cloudW) {
        SlicedSprite("clouds").draw(p, cx, cloudY, cloudW, cloudH);
        if (cx + cloudW < 0) cx += cloudW;
    }

    int wWidth = 170 * scale;
    int wHeight = 16 * scale;
    float spacing = 250 * scale;
    float tileStep = wWidth + spacing;
    for (const auto& water : m_waters) {
        float baseX = water.pos.x();
        float offsetX = std::fmod(cameraX * 1.0f, tileStep);
        for (float wx = baseX - offsetX - tileStep; wx < screenW; wx += tileStep) {
            water.sprite->draw(p, QRectF(wx, water.pos.y(), wWidth, wHeight), false);
        }
    }

    int shipW = 78 * scale;
    int shipH = 72 * scale;
    int waveW = 96 * scale;
    int waveH = 96 * scale;
    int waveY = screenH - waveH + 15 * scale;

    if (!s_shipInitialized) {
        s_shipX = cameraX + (40 * scale);
        s_shipInitialized = true;
    }
    float shipScreenX = s_shipX - cameraX;

    if (shipScreenX > screenW) {
        s_shipX = cameraX - shipW;
    } else if (shipScreenX < -shipW) {
        s_shipX = cameraX + screenW;
    }
    float finalDrawX = s_shipX - cameraX;
    int shipY = waveY - shipH + (40 * scale);
    if (m_shipSprite) {
        m_shipSprite->draw(p, QRectF(finalDrawX, shipY, shipW, shipH), false);
    }

    if (m_waterWavesSprite) {
        float waveOffset = std::fmod(cameraX * 1.07f, (float)waveW);
        for (float wx = -waveOffset; wx < screenW; wx += waveW) {
            m_waterWavesSprite->draw(p, QRectF(wx, waveY, waveW, waveH), false);
        }
    }
}