#pragma once
#include "entities/GameObject.h"
#include "entities/InteractionResult.h"

class InteractiveObject : public GameObject {
public:
    InteractiveObject(float x, float y, float w, float h);
    bool isInteractable() const override { return true; }
    virtual InteractionResult interact() = 0;

    void setLocked(const QString& pType, const QStringList& pData);
    void unlock();
    virtual void update() {}
    bool isLocked() const { return m_isLocked; }
    void setShowHint(bool s) { m_showHint = s; }

protected:
    bool        m_isLocked  = false;
    bool        m_showHint  = false;
    QString     m_puzzleType;
    QStringList m_puzzleData;
};