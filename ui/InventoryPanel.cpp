#include "ui/InventoryPanel.h"
#include "systems/InventorySystem.h"
// TODO (Milestone 3): implement grid of item icons
InventoryPanel::InventoryPanel(InventorySystem* inv, QWidget* p):QWidget(p),m_inv(inv){}
void InventoryPanel::refresh(){}
