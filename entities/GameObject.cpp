#include "entities/GameObject.h"

GameObject::GameObject(float x, float y, float w, float h)
    : m_x(x), m_y(y), m_w(w), m_h(h)
{}

QRectF GameObject::boundingBox() const
{
    return { static_cast<double>(m_x),
             static_cast<double>(m_y),
             static_cast<double>(m_w),
             static_cast<double>(m_h) };
}
