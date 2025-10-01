#pragma once

#include "UIPanel.h"
#include "UIButton.h"
#include "UILabel.h"
#include "../Equipment/EquipmentLoadout.h"
#include <vector>
#include <memory>
#include <functional>
#include <d3d11.h>

namespace ArenaFighter {

class EquipmentScreen {
private:
    std::shared_ptr<UIPanel> m_rootPanel;
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_context;
    
    // Equipment data
    EquipmentLoadout m_currentLoadout;
    std::vector<std::shared_ptr<EquipmentItem>> m_inventory;
    std::shared_ptr<EquipmentItem> m_selectedItem;
    EquipmentSlot m_selectedSlot;
    std::string m_currentCategory;
    
    // UI references
    std::shared_ptr<UIPanel> m_characterPreviewPanel;
    std::shared_ptr<UIPanel> m_equipmentSlotsPanel;
    std::shared_ptr<UIPanel> m_inventoryPanel;
    std::shared_ptr<UIPanel> m_itemDetailsPanel;
    std::shared_ptr<UILabel> m_totalStatsLabel;
    
    // Callbacks
    std::function<void()> m_onEquipmentChanged;
    std::function<void()> m_onClose;

public:
    EquipmentScreen(ID3D11Device* dev, ID3D11DeviceContext* ctx);
    
    std::shared_ptr<UIPanel> getRootPanel() { return m_rootPanel; }
    
    void setCallbacks(
        std::function<void()> equipChangedCallback,
        std::function<void()> closeCallback
    );
    
    const EquipmentLoadout& getCurrentLoadout() const { return m_currentLoadout; }
    
private:
    void initializeInventory();
    void buildUI();
    
    // UI creation methods
    void createCharacterPreviewPanel();
    void createEquipmentSlotsPanel();
    std::shared_ptr<UIPanel> createEquipmentSlot(const std::string& name, EquipmentSlot slot,
                                                  const std::string& keyBinding, XMFLOAT2 pos);
    void createTotalStatsPanel();
    void createInventoryPanel();
    void populateInventoryGrid();
    std::shared_ptr<UIPanel> createInventoryItemCard(std::shared_ptr<EquipmentItem> item, 
                                                     XMFLOAT2 pos, XMFLOAT2 size);
    void createItemDetailsPanel();
    void createCategoryTabs();
    
    // Equipment management
    void equipItem(std::shared_ptr<EquipmentItem> item);
    void unequipSlot(EquipmentSlot slot);
    void selectItem(std::shared_ptr<EquipmentItem> item);
    
    // UI updates
    void updateTotalStatsDisplay();
    void updateItemDetails();
    void refreshEquipmentSlots();
    void refreshInventory();
    void filterByCategory(const std::string& category);
    
    // Helper methods
    std::shared_ptr<EquipmentItem> createSampleItem(
        const std::string& id, const std::string& name, const std::string& desc,
        EquipmentSlot slot, ItemRarity rarity, int atk, int def, int spd);
    std::string getSlotCategory(EquipmentSlot slot) const;
};

} // namespace ArenaFighter