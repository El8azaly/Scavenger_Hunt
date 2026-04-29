#pragma once
#include <QWidget>
class InventorySystem;

class InventoryPanel : public QWidget {
    Q_OBJECT
public:
    explicit InventoryPanel(InventorySystem* inv, QWidget* parent = nullptr);
    void refresh();
signals:
    void itemSelectedForCraft(const QString& itemId);
private:
    InventorySystem* m_inv;
};
