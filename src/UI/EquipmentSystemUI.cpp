#include "EquipmentSystem.h"
#include "UIRenderer.h"
#include <sstream>

namespace ArenaFighter {

void EquipmentSystem::refreshUI() {
    refreshEquipmentSlots();
    refreshInventoryDisplay();
    refreshStatsDisplay();
    refreshItemDetails();
    refreshComparisonPanel();
}

void EquipmentSystem::refreshEquipmentSlots() {
    if (m_equipmentSlotsPanelId.empty()) return;
    
    // Update each equipment slot UI
    for (int i = 0; i < static_cast<int>(EquipmentSlot::Count); ++i) {
        EquipmentSlot slot = static_cast<EquipmentSlot>(i);
        std::string slotPanelId = m_equipmentSlotsPanelId + "_slot_" + std::to_string(i);
        
        auto it = m_equippedItems.find(slot);
        if (it != m_equippedItems.end()) {
            updateEquipmentSlotUI(slotPanelId, it->second);
        } else {
            updateEquipmentSlotUI(slotPanelId, "");
        }
    }
}

void EquipmentSystem::refreshInventoryDisplay() {
    if (m_inventoryPanelId.empty()) return;
    
    // Clear current inventory display
    // Note: In a real implementation, we'd clear child panels here
    
    // Get filtered inventory
    auto filteredItems = getFilteredInventory();
    
    // Create item panels for filtered items
    float x = 10.0f;
    float y = 10.0f;
    float itemWidth = 80.0f;
    float itemHeight = 100.0f;
    float padding = 10.0f;
    int itemsPerRow = 6;
    
    for (size_t i = 0; i < filteredItems.size(); ++i) {
        auto itemPanel = createInventoryItemUI(filteredItems[i]);
        if (itemPanel) {
            int row = static_cast<int>(i) / itemsPerRow;
            int col = static_cast<int>(i) % itemsPerRow;
            
            float posX = x + col * (itemWidth + padding);
            float posY = y + row * (itemHeight + padding);
            
            itemPanel->setPosition(XMFLOAT2(posX, posY));
            itemPanel->setSize(XMFLOAT2(itemWidth, itemHeight));
            
            // Add to inventory panel
            // Note: In real implementation, we'd add as child to m_inventoryPanelId
        }
    }
}

void EquipmentSystem::refreshStatsDisplay() {
    updateStatsLabel();
}

void EquipmentSystem::refreshItemDetails() {
    if (m_itemDetailsPanelId.empty() || m_selectedItemId.empty()) return;
    
    auto itemIt = m_inventory.find(m_selectedItemId);
    if (itemIt == m_inventory.end()) return;
    
    const EquipmentItem& item = itemIt->second;
    
    // Update item details display
    std::stringstream details;
    details << "Name: " << item.m_name << "\n";
    details << "Rarity: " << item.getRarityString() << "\n";
    details << "Slot: " << item.getSlotString() << "\n\n";
    
    details << "Stats:\n";
    if (item.m_attackBonus > 0) details << "  Attack: +" << item.m_attackBonus << "\n";
    if (item.m_defenseBonus > 0) details << "  Defense: +" << item.m_defenseBonus << "\n";
    if (item.m_specialBonus > 0) details << "  Special: +" << item.m_specialBonus << "\n";
    
    if (item.hasCombatSkill()) {
        details << "\nSkill: " << item.m_skillName << "\n";
        details << "Mana Cost: " << item.m_manaCost << " MP\n";
        details << item.m_skillDescription << "\n";
    }
    
    details << "\n" << item.m_description;
    
    // Update the label text
    // Note: In real implementation, we'd update the label component
}

void EquipmentSystem::refreshComparisonPanel() {
    if (m_comparisonPanelId.empty() || m_selectedItemId.empty()) return;
    
    // Find currently equipped item in selected slot
    auto selectedItem = getItem(m_selectedItemId);
    if (!selectedItem) return;
    
    auto equippedIt = m_equippedItems.find(selectedItem->m_slot);
    if (equippedIt == m_equippedItems.end()) {
        // No item equipped in this slot, show direct stats
        return;
    }
    
    // Compare with equipped item
    EquipmentComparison comparison = compareItems(m_selectedItemId, equippedIt->second);
    
    std::stringstream compText;
    compText << "Comparison:\n";
    
    if (comparison.m_attackDiff != 0) {
        compText << "Attack: " << (comparison.m_attackDiff > 0 ? "+" : "") 
                 << comparison.m_attackDiff << "\n";
    }
    
    if (comparison.m_defenseDiff != 0) {
        compText << "Defense: " << (comparison.m_defenseDiff > 0 ? "+" : "") 
                 << comparison.m_defenseDiff << "\n";
    }
    
    if (comparison.m_specialDiff != 0) {
        compText << "Special: " << (comparison.m_specialDiff > 0 ? "+" : "") 
                 << comparison.m_specialDiff << "\n";
    }
    
    compText << "\nTotal: " << (comparison.m_totalDiff > 0 ? "+" : "") 
             << comparison.m_totalDiff;
    
    // Update comparison display
    // Note: In real implementation, we'd update the comparison panel
}

void EquipmentSystem::buildUI() {
    // Create root panel
    m_rootPanel = std::make_shared<UIPanel>();
    m_rootPanel->setPosition(XMFLOAT2(0, 0));
    m_rootPanel->setSize(XMFLOAT2(1280, 720));
    m_rootPanel->setBackgroundColor(XMFLOAT4(0.1f, 0.1f, 0.1f, 0.9f));
    
    // Create sub-panels
    createCharacterPreviewPanel();
    createEquipmentSlotsPanel();
    createInventoryPanel();
    createItemDetailsPanel();
    createComparisonPanel();
    createPresetPanel();
    createFilterPanel();
    
    // Create close button
    auto closeButton = std::make_shared<UIButton>();
    closeButton->setPosition(XMFLOAT2(1230, 10));
    closeButton->setSize(XMFLOAT2(40, 40));
    closeButton->setText("X");
    closeButton->setOnClick([this]() {
        if (m_onClose) m_onClose();
    });
    m_rootPanel->addChild(closeButton);
}

void EquipmentSystem::createCharacterPreviewPanel() {
    auto panel = std::make_shared<UIPanel>();
    panel->setPosition(XMFLOAT2(20, 20));
    panel->setSize(XMFLOAT2(300, 400));
    panel->setBackgroundColor(XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f));
    
    m_characterPreviewPanelId = "character_preview";
    
    // Add title
    auto title = std::make_shared<UILabel>();
    title->setPosition(XMFLOAT2(10, 10));
    title->setText("Character Preview");
    title->setColor(XMFLOAT4(1, 1, 1, 1));
    panel->addChild(title);
    
    // Add stats display
    auto statsLabel = std::make_shared<UILabel>();
    statsLabel->setPosition(XMFLOAT2(10, 350));
    m_totalStatsLabelId = "total_stats";
    panel->addChild(statsLabel);
    
    m_rootPanel->addChild(panel);
}

void EquipmentSystem::createEquipmentSlotsPanel() {
    auto panel = std::make_shared<UIPanel>();
    panel->setPosition(XMFLOAT2(340, 20));
    panel->setSize(XMFLOAT2(200, 400));
    panel->setBackgroundColor(XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f));
    
    m_equipmentSlotsPanelId = "equipment_slots";
    
    // Add title
    auto title = std::make_shared<UILabel>();
    title->setPosition(XMFLOAT2(10, 10));
    title->setText("Equipment");
    title->setColor(XMFLOAT4(1, 1, 1, 1));
    panel->addChild(title);
    
    // Create slot UIs
    float yOffset = 50.0f;
    for (int i = 0; i < static_cast<int>(EquipmentSlot::Count); ++i) {
        EquipmentSlot slot = static_cast<EquipmentSlot>(i);
        auto slotPanel = createEquipmentSlotUI(slot, XMFLOAT2(10, yOffset));
        panel->addChild(slotPanel);
        yOffset += 50.0f;
    }
    
    m_rootPanel->addChild(panel);
}

void EquipmentSystem::createInventoryPanel() {
    auto panel = std::make_shared<UIPanel>();
    panel->setPosition(XMFLOAT2(560, 20));
    panel->setSize(XMFLOAT2(520, 400));
    panel->setBackgroundColor(XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f));
    
    m_inventoryPanelId = "inventory";
    
    // Add title
    auto title = std::make_shared<UILabel>();
    title->setPosition(XMFLOAT2(10, 10));
    title->setText("Inventory");
    title->setColor(XMFLOAT4(1, 1, 1, 1));
    panel->addChild(title);
    
    m_rootPanel->addChild(panel);
}

void EquipmentSystem::createItemDetailsPanel() {
    auto panel = std::make_shared<UIPanel>();
    panel->setPosition(XMFLOAT2(1100, 20));
    panel->setSize(XMFLOAT2(160, 400));
    panel->setBackgroundColor(XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f));
    
    m_itemDetailsPanelId = "item_details";
    
    // Add title
    auto title = std::make_shared<UILabel>();
    title->setPosition(XMFLOAT2(10, 10));
    title->setText("Item Details");
    title->setColor(XMFLOAT4(1, 1, 1, 1));
    panel->addChild(title);
    
    m_rootPanel->addChild(panel);
}

void EquipmentSystem::createComparisonPanel() {
    auto panel = std::make_shared<UIPanel>();
    panel->setPosition(XMFLOAT2(560, 440));
    panel->setSize(XMFLOAT2(250, 150));
    panel->setBackgroundColor(XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f));
    
    m_comparisonPanelId = "comparison";
    
    // Add title
    auto title = std::make_shared<UILabel>();
    title->setPosition(XMFLOAT2(10, 10));
    title->setText("Comparison");
    title->setColor(XMFLOAT4(1, 1, 1, 1));
    panel->addChild(title);
    
    m_rootPanel->addChild(panel);
}

void EquipmentSystem::createPresetPanel() {
    auto panel = std::make_shared<UIPanel>();
    panel->setPosition(XMFLOAT2(20, 440));
    panel->setSize(XMFLOAT2(300, 150));
    panel->setBackgroundColor(XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f));
    
    // Add title
    auto title = std::make_shared<UILabel>();
    title->setPosition(XMFLOAT2(10, 10));
    title->setText("Presets");
    title->setColor(XMFLOAT4(1, 1, 1, 1));
    panel->addChild(title);
    
    // Add preset buttons
    auto saveButton = std::make_shared<UIButton>();
    saveButton->setPosition(XMFLOAT2(10, 40));
    saveButton->setSize(XMFLOAT2(80, 30));
    saveButton->setText("Save");
    saveButton->setOnClick([this]() {
        savePreset("Custom " + std::to_string(m_presets.size() + 1), "Custom preset");
    });
    panel->addChild(saveButton);
    
    m_rootPanel->addChild(panel);
}

void EquipmentSystem::createFilterPanel() {
    auto panel = std::make_shared<UIPanel>();
    panel->setPosition(XMFLOAT2(830, 440));
    panel->setSize(XMFLOAT2(250, 150));
    panel->setBackgroundColor(XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f));
    
    // Add title
    auto title = std::make_shared<UILabel>();
    title->setPosition(XMFLOAT2(10, 10));
    title->setText("Filter & Sort");
    title->setColor(XMFLOAT4(1, 1, 1, 1));
    panel->addChild(title);
    
    // Add filter buttons
    auto allButton = std::make_shared<UIButton>();
    allButton->setPosition(XMFLOAT2(10, 40));
    allButton->setSize(XMFLOAT2(50, 25));
    allButton->setText("All");
    allButton->setOnClick([this]() {
        EquipmentFilter filter = m_currentFilter;
        filter.m_filterType = EquipmentFilter::FilterType::All;
        setFilter(filter);
    });
    panel->addChild(allButton);
    
    m_rootPanel->addChild(panel);
}

std::shared_ptr<UIPanel> EquipmentSystem::createEquipmentSlotUI(EquipmentSlot slot, const XMFLOAT2& position) {
    auto panel = std::make_shared<UIPanel>();
    panel->setPosition(position);
    panel->setSize(XMFLOAT2(180, 40));
    panel->setBackgroundColor(XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f));
    
    // Slot name
    auto nameLabel = std::make_shared<UILabel>();
    nameLabel->setPosition(XMFLOAT2(5, 10));
    nameLabel->setText(getSlotName(slot));
    nameLabel->setColor(XMFLOAT4(1, 1, 1, 1));
    panel->addChild(nameLabel);
    
    // Key binding (for combat gear)
    if (isSlotCombatGear(slot)) {
        auto keyLabel = std::make_shared<UILabel>();
        keyLabel->setPosition(XMFLOAT2(150, 10));
        keyLabel->setText("[" + getSlotKeyBinding(slot) + "]");
        keyLabel->setColor(XMFLOAT4(0.7f, 0.7f, 0.7f, 1));
        panel->addChild(keyLabel);
    }
    
    // Click handler
    panel->setOnClick([this, slot]() {
        selectSlot(slot);
    });
    
    return panel;
}

std::shared_ptr<UIPanel> EquipmentSystem::createInventoryItemUI(const std::string& itemId) {
    auto item = getItem(itemId);
    if (!item) return nullptr;
    
    auto panel = std::make_shared<UIPanel>();
    panel->setBackgroundColor(item->getRarityColor());
    
    // Item name
    auto nameLabel = std::make_shared<UILabel>();
    nameLabel->setPosition(XMFLOAT2(5, 5));
    nameLabel->setText(item->m_name);
    nameLabel->setColor(XMFLOAT4(1, 1, 1, 1));
    panel->addChild(nameLabel);
    
    // Click handlers
    panel->setOnClick([this, itemId]() {
        selectItem(itemId);
    });
    
    panel->setOnDoubleClick([this, itemId]() {
        swapEquipment(itemId);
    });
    
    return panel;
}

void EquipmentSystem::updateEquipmentSlotUI(const std::string& panelId, const std::string& itemId) {
    // In a real implementation, we'd find the panel by ID and update its display
    // For now, this is a placeholder
}

void EquipmentSystem::updateStatsLabel() {
    std::stringstream stats;
    stats << "Total Stats:\n";
    stats << "ATK: " << getTotalAttack() << "\n";
    stats << "DEF: " << getTotalDefense() << "\n";
    stats << "SPD: " << getTotalSpeed() << "\n";
    stats << "HP: " << getTotalHealth() << "\n";
    stats << "MP: " << getTotalMana() << "\n";
    
    auto activeSets = getActiveSetBonuses();
    if (!activeSets.empty()) {
        stats << "\nSet Bonuses:\n";
        for (const auto& setName : activeSets) {
            stats << "- " << setName << "\n";
        }
    }
    
    // Update the stats label
    // Note: In real implementation, we'd update the label by ID
}

void EquipmentSystem::loadDefaultTextures() {
    // Load default textures for equipment slots
    // This would typically load placeholder icons for empty slots
}

ComPtr<ID3D11ShaderResourceView> EquipmentSystem::getSlotIconTexture(EquipmentSlot slot) const {
    // Return appropriate icon texture for the slot type
    return nullptr; // Placeholder
}

} // namespace ArenaFighter