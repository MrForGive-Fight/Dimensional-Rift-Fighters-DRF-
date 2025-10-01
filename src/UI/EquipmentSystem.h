#pragma once

#include "EquipmentTypes.h"
#include "UIPanel.h"
#include "UIButton.h"
#include "UILabel.h"
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <wrl/client.h>
#include <d3d11.h>

namespace ArenaFighter {

using Microsoft::WRL::ComPtr;

class EquipmentSystem {
private:
    // DirectX resources
    ComPtr<ID3D11Device> m_device;
    ComPtr<ID3D11DeviceContext> m_context;
    
    // Equipment data
    std::map<std::string, EquipmentItem> m_inventory;
    std::map<EquipmentSlot, std::string> m_equippedItems;
    std::vector<EquipmentPreset> m_presets;
    std::vector<SetBonus> m_setBonuses;
    
    // Current selection state
    std::string m_selectedItemId;
    EquipmentSlot m_selectedSlot;
    EquipmentFilter m_currentFilter;
    int m_currentPresetIndex;
    
    // UI element IDs (using string IDs to avoid pointer invalidation)
    std::string m_characterPreviewPanelId;
    std::string m_equipmentSlotsPanelId;
    std::string m_inventoryPanelId;
    std::string m_itemDetailsPanelId;
    std::string m_totalStatsLabelId;
    std::string m_comparisonPanelId;
    
    // UI panels (kept for direct access)
    std::shared_ptr<UIPanel> m_rootPanel;
    
    // Callbacks
    std::function<void(const std::string&, EquipmentSlot)> m_onEquipmentChanged;
    std::function<void(const EquipmentPreset&)> m_onPresetLoaded;
    std::function<void()> m_onClose;
    
    // Base character stats
    struct BaseStats {
        int m_health;
        int m_mana;
        int m_attack;
        int m_defense;
        int m_speed;
    } m_baseStats;

public:
    EquipmentSystem(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    ~EquipmentSystem();
    
    // Initialization
    void initialize();
    void setBaseStats(int health, int mana, int attack, int defense, int speed);
    void loadInventory(const std::vector<EquipmentItem>& items);
    void loadSetBonuses(const std::vector<SetBonus>& setBonuses);
    
    // UI Access
    std::shared_ptr<UIPanel> getRootPanel() const { return m_rootPanel; }
    void show();
    void hide();
    
    // Equipment Management
    bool equipItem(const std::string& itemId);
    bool unequipSlot(EquipmentSlot slot);
    void swapEquipment(const std::string& itemId);
    EquipmentComparison compareItems(const std::string& itemId1, const std::string& itemId2) const;
    
    // Inventory Management
    void addItem(const EquipmentItem& item);
    void removeItem(const std::string& itemId);
    const EquipmentItem* getItem(const std::string& itemId) const;
    std::vector<std::string> getFilteredInventory() const;
    
    // Preset Management
    void savePreset(const std::string& name, const std::string& description);
    void loadPreset(int presetIndex);
    void deletePreset(int presetIndex);
    const std::vector<EquipmentPreset>& getPresets() const { return m_presets; }
    
    // Filter and Sort
    void setFilter(const EquipmentFilter& filter);
    void sortInventory(EquipmentFilter::SortBy sortBy, bool ascending);
    
    // Stats Calculation
    int getTotalAttack() const;
    int getTotalDefense() const;
    int getTotalSpeed() const;
    int getTotalHealth() const;
    int getTotalMana() const;
    std::vector<std::string> getActiveSetBonuses() const;
    
    // UI Updates
    void refreshUI();
    void refreshEquipmentSlots();
    void refreshInventoryDisplay();
    void refreshStatsDisplay();
    void refreshItemDetails();
    void refreshComparisonPanel();
    
    // Selection
    void selectItem(const std::string& itemId);
    void selectSlot(EquipmentSlot slot);
    const std::string& getSelectedItemId() const { return m_selectedItemId; }
    EquipmentSlot getSelectedSlot() const { return m_selectedSlot; }
    
    // Callbacks
    void setEquipmentChangedCallback(std::function<void(const std::string&, EquipmentSlot)> callback);
    void setPresetLoadedCallback(std::function<void(const EquipmentPreset&)> callback);
    void setCloseCallback(std::function<void()> callback);

private:
    // UI Creation
    void buildUI();
    void createCharacterPreviewPanel();
    void createEquipmentSlotsPanel();
    void createInventoryPanel();
    void createItemDetailsPanel();
    void createComparisonPanel();
    void createPresetPanel();
    void createFilterPanel();
    
    // UI Helpers
    std::shared_ptr<UIPanel> createEquipmentSlotUI(EquipmentSlot slot, const XMFLOAT2& position);
    std::shared_ptr<UIPanel> createInventoryItemUI(const std::string& itemId);
    void updateEquipmentSlotUI(const std::string& panelId, const std::string& itemId);
    void updateStatsLabel();
    
    // Helper Methods
    std::string getSlotName(EquipmentSlot slot) const;
    std::string getSlotKeyBinding(EquipmentSlot slot) const;
    bool isSlotCombatGear(EquipmentSlot slot) const;
    bool canEquipItem(const std::string& itemId, EquipmentSlot slot) const;
    std::vector<std::string> getEquippedItemIds() const;
    
    // Asset Loading
    void loadDefaultTextures();
    ComPtr<ID3D11ShaderResourceView> getSlotIconTexture(EquipmentSlot slot) const;
};

} // namespace ArenaFighter