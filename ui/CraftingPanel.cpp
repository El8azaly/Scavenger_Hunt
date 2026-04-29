#include "ui/CraftingPanel.h"
// TODO (Milestone 3): implement crafting slot UI
CraftingPanel::CraftingPanel(InventorySystem* inv, CraftingSystem* craft, QWidget* p)
    :QWidget(p),m_inv(inv),m_craft(craft){}
