#pragma once
#include "entities/InteractiveObject.h"
#include <QVector>
#include <QString>
#include <memory>
#include "data/ItemData.h"

class AnimatedSprite;

class Container : public InteractiveObject {
public:
    Container(float x, float y, float w, float h, const QString& type = "chest");
    ~Container() override;

    void update() override;
    void draw(QPainter& painter, float camX, float camY) override;
    void updateAnimation();

    InteractionResult interact() override;
    void addItem(const Item& item);

    bool isInteractable() const override { return !m_opened && !m_opening; }

private:
    QString m_type;
    std::unique_ptr<AnimatedSprite> m_sprite;
    QVector<Item> m_contents;

    bool m_opened = false;
    bool m_opening = false;

    float m_spriteWidth;
    float m_spriteHeight;
    float m_spriteOffsetX;
    float m_spriteOffsetY;
};