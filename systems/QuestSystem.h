#pragma once
#include <QObject>
#include <QSet>
#include <QStringList>
#include "systems/InventorySystem.h"

class QuestSystem : public QObject {
    Q_OBJECT
public:
    explicit QuestSystem(InventorySystem* inventory, QObject* parent = nullptr);
    void setTargets(const QStringList& ids);
    int  totalTargets()  const { return m_targetIds.size(); }
    int  foundTargets()  const { return m_foundIds.size();  }
    bool isTarget(const QString& id)  const { return m_targetIds.contains(id); }
    bool isFound(const QString& id)   const { return m_foundIds.contains(id);  }
    bool isComplete()                 const { return !m_targetIds.isEmpty() && m_foundIds.size()==m_targetIds.size(); }
    QStringList remainingTargets()    const;
    QSet<QString> targetIds()         const { return m_targetIds; }
    QSet<QString> foundIds()          const { return m_foundIds;  }
signals:
    void targetFound(const QString& itemId);
    void allTargetsFound();
private slots:
    void onItemAdded(const Item& item);

private:
    QSet<QString> m_targetIds, m_foundIds;
    InventorySystem* m_inventory;
};