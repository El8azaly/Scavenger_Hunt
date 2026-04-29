#pragma once
#include <QWidget>
class InventorySystem;
class CraftingSystem;

class CraftingPanel : public QWidget {
    Q_OBJECT
public:
    explicit CraftingPanel(InventorySystem* inv, CraftingSystem* craft, QWidget* parent = nullptr);
signals:
    void craftingDone();
private:
    InventorySystem* m_inv;
    CraftingSystem*  m_craft;
};
