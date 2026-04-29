#pragma once
#include <QObject>
#include <QVector>
#include "data/ItemData.h"

class InventorySystem : public QObject {
    Q_OBJECT
public:
    explicit InventorySystem(QObject* parent = nullptr);
    void      addItem(const Item& item);
    bool      removeItem(const QString& itemId);
    bool      hasItem(const QString& itemId) const;
    Item      getItem(const QString& itemId) const;
    QVector<Item> allItems() const { return m_items; }
    int       itemCount() const    { return m_items.size(); }
    void      clear();
signals:
    void itemAdded(const Item& item);
    void itemRemoved(const QString& itemId);
    void inventoryChanged();
private:
    QVector<Item> m_items;
};
