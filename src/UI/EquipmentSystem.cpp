#include "EquipmentSystem.h"
#include "UIRenderer.h"
#include <algorithm>
#include <sstream>

namespace ArenaFighter {

EquipmentSystem::EquipmentSystem(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
    : m_device(device)
    , m_context(context)
    , m_selectedSlot(EquipmentSlot::Weapon)
    , m_currentPresetIndex(-1)
    , m_baseStats{1000, 100, 100, 100, 100} // Default base stats per CLAUDE.md
{
}

EquipmentSystem::~EquipmentSystem() {
}

void EquipmentSystem::initialize() {
    buildUI();
    loadDefaultTextures();
    refreshUI();
}

void EquipmentSystem::setBaseStats(int health, int mana, int attack, int defense, int speed) {
    m_baseStats.m_health = health;
    m_baseStats.m_mana = mana;
    m_baseStats.m_attack = attack;
    m_baseStats.m_defense = defense;
    m_baseStats.m_speed = speed;
    refreshStatsDisplay();
}

void EquipmentSystem::loadInventory(const std::vector<EquipmentItem>& items) {
    m_inventory.clear();
    for (const auto& item : items) {
        m_inventory[item.m_id] = item;
    }
    refreshInventoryDisplay();
}

void EquipmentSystem::loadSetBonuses(const std::vector<SetBonus>& setBonuses) {
    m_setBonuses = setBonuses;
    refreshStatsDisplay();
}

void EquipmentSystem::show() {
    if (m_rootPanel) {
        m_rootPanel->setVisible(true);
    }
}

void EquipmentSystem::hide() {
    if (m_rootPanel) {
        m_rootPanel->setVisible(false);
    }
}

bool EquipmentSystem::equipItem(const std::string& itemId) {
    auto itemIt = m_inventory.find(itemId);
    if (itemIt == m_inventory.end()) {
        return false;
    }
    
    const EquipmentItem& item = itemIt->second;
    if (!canEquipItem(itemId, item.m_slot)) {
        return false;
    }
    
    // Unequip current item in slot if any
    auto currentIt = m_equippedItems.find(item.m_slot);
    if (currentIt != m_equippedItems.end()) {
        unequipSlot(item.m_slot);
    }
    
    // Equip new item
    m_equippedItems[item.m_slot] = itemId;
    
    // Trigger callback
    if (m_onEquipmentChanged) {
        m_onEquipmentChanged(itemId, item.m_slot);
    }
    
    // Update UI
    refreshEquipmentSlots();
    refreshStatsDisplay();
    refreshComparisonPanel();
    
    return true;
}

bool EquipmentSystem::unequipSlot(EquipmentSlot slot) {
    auto it = m_equippedItems.find(slot);
    if (it == m_equippedItems.end()) {
        return false;
    }
    
    std::string itemId = it->second;
    m_equippedItems.erase(it);
    
    if (m_onEquipmentChanged) {
        m_onEquipmentChanged("", slot);
    }
    
    refreshEquipmentSlots();
    refreshStatsDisplay();
    refreshComparisonPanel();
    
    return true;
}

void EquipmentSystem::swapEquipment(const std::string& itemId) {
    auto itemIt = m_inventory.find(itemId);
    if (itemIt == m_inventory.end()) {
        return;
    }
    
    equipItem(itemId);
}

EquipmentComparison EquipmentSystem::compareItems(const std::string& itemId1, const std::string& itemId2) const {
    EquipmentComparison result{0, 0, 0, 0};
    
    auto item1It = m_inventory.find(itemId1);
    auto item2It = m_inventory.find(itemId2);
    
    if (item1It == m_inventory.end() || item2It == m_inventory.end()) {
        return result;
    }
    
    const EquipmentItem& item1 = item1It->second;
    const EquipmentItem& item2 = item2It->second;
    
    result.m_attackDiff = item1.m_attackBonus - item2.m_attackBonus;
    result.m_defenseDiff = item1.m_defenseBonus - item2.m_defenseBonus;
    result.m_specialDiff = item1.m_specialBonus - item2.m_specialBonus;
    result.m_totalDiff = result.m_attackDiff + result.m_defenseDiff + result.m_specialDiff;
    
    return result;
}

void EquipmentSystem::addItem(const EquipmentItem& item) {
    m_inventory[item.m_id] = item;
    refreshInventoryDisplay();
}

void EquipmentSystem::removeItem(const std::string& itemId) {
    // Unequip if equipped
    for (auto& pair : m_equippedItems) {
        if (pair.second == itemId) {
            unequipSlot(pair.first);
            break;
        }
    }
    
    m_inventory.erase(itemId);
    refreshInventoryDisplay();
}

const EquipmentItem* EquipmentSystem::getItem(const std::string& itemId) const {
    auto it = m_inventory.find(itemId);
    return (it != m_inventory.end()) ? &it->second : nullptr;
}

std::vector<std::string> EquipmentSystem::getFilteredInventory() const {
    std::vector<std::string> filtered;
    
    for (const auto& pair : m_inventory) {
        const EquipmentItem& item = pair.second;
        bool passesFilter = true;
        
        // Apply filter
        switch (m_currentFilter.m_filterType) {
            case EquipmentFilter::FilterType::Weapon:
                passesFilter = (item.m_slot == EquipmentSlot::Weapon);
                break;
            case EquipmentFilter::FilterType::Armor:
                passesFilter = (item.m_slot == EquipmentSlot::Armor);
                break;
            case EquipmentFilter::FilterType::Helmet:
                passesFilter = (item.m_slot == EquipmentSlot::Helmet);
                break;
            case EquipmentFilter::FilterType::Trinket:
                passesFilter = (item.m_slot == EquipmentSlot::Trinket);
                break;
            case EquipmentFilter::FilterType::Fashion:
                passesFilter = (item.m_slot == EquipmentSlot::Fashion_Hair ||
                              item.m_slot == EquipmentSlot::Fashion_Face ||
                              item.m_slot == EquipmentSlot::Fashion_Body);
                break;
            case EquipmentFilter::FilterType::CombatGear:
                passesFilter = item.hasCombatSkill();
                break;
            case EquipmentFilter::FilterType::Rarity:
                passesFilter = (item.m_rarity == m_currentFilter.m_rarityFilter);
                break;
            default:
                break;
        }
        
        if (passesFilter) {
            filtered.push_back(pair.first);
        }
    }
    
    // Sort filtered items
    std::sort(filtered.begin(), filtered.end(), [this](const std::string& a, const std::string& b) {
        const EquipmentItem* itemA = getItem(a);
        const EquipmentItem* itemB = getItem(b);
        if (!itemA || !itemB) return false;
        
        bool result = false;
        switch (m_currentFilter.m_sortBy) {
            case EquipmentFilter::SortBy::Name:
                result = itemA->m_name < itemB->m_name;
                break;
            case EquipmentFilter::SortBy::Rarity:
                result = static_cast<int>(itemA->m_rarity) < static_cast<int>(itemB->m_rarity);
                break;
            case EquipmentFilter::SortBy::Attack:
                result = itemA->m_attackBonus < itemB->m_attackBonus;
                break;
            case EquipmentFilter::SortBy::Defense:
                result = itemA->m_defenseBonus < itemB->m_defenseBonus;
                break;
            case EquipmentFilter::SortBy::Special:
                result = itemA->m_specialBonus < itemB->m_specialBonus;
                break;
            case EquipmentFilter::SortBy::Total:
                result = (itemA->m_attackBonus + itemA->m_defenseBonus + itemA->m_specialBonus) <
                        (itemB->m_attackBonus + itemB->m_defenseBonus + itemB->m_specialBonus);
                break;
            case EquipmentFilter::SortBy::Slot:
                result = static_cast<int>(itemA->m_slot) < static_cast<int>(itemB->m_slot);
                break;
        }
        
        return m_currentFilter.m_ascending ? result : !result;
    });
    
    return filtered;
}

void EquipmentSystem::savePreset(const std::string& name, const std::string& description) {
    EquipmentPreset preset;
    preset.m_name = name;
    preset.m_description = description;
    preset.m_equippedItems = m_equippedItems;
    
    m_presets.push_back(preset);
}

void EquipmentSystem::loadPreset(int presetIndex) {
    if (presetIndex < 0 || presetIndex >= static_cast<int>(m_presets.size())) {
        return;
    }
    
    const EquipmentPreset& preset = m_presets[presetIndex];
    
    // Clear current equipment
    std::vector<EquipmentSlot> slotsToUnequip;
    for (const auto& pair : m_equippedItems) {
        slotsToUnequip.push_back(pair.first);
    }
    for (auto slot : slotsToUnequip) {
        unequipSlot(slot);
    }
    
    // Equip preset items
    for (const auto& pair : preset.m_equippedItems) {
        if (m_inventory.find(pair.second) != m_inventory.end()) {
            equipItem(pair.second);
        }
    }
    
    m_currentPresetIndex = presetIndex;
    
    if (m_onPresetLoaded) {
        m_onPresetLoaded(preset);
    }
}

void EquipmentSystem::deletePreset(int presetIndex) {
    if (presetIndex < 0 || presetIndex >= static_cast<int>(m_presets.size())) {
        return;
    }
    
    m_presets.erase(m_presets.begin() + presetIndex);
    
    if (m_currentPresetIndex == presetIndex) {
        m_currentPresetIndex = -1;
    } else if (m_currentPresetIndex > presetIndex) {
        m_currentPresetIndex--;
    }
}

void EquipmentSystem::setFilter(const EquipmentFilter& filter) {
    m_currentFilter = filter;
    refreshInventoryDisplay();
}

void EquipmentSystem::sortInventory(EquipmentFilter::SortBy sortBy, bool ascending) {
    m_currentFilter.m_sortBy = sortBy;
    m_currentFilter.m_ascending = ascending;
    refreshInventoryDisplay();
}

int EquipmentSystem::getTotalAttack() const {
    int total = m_baseStats.m_attack;
    
    for (const auto& pair : m_equippedItems) {
        auto itemIt = m_inventory.find(pair.second);
        if (itemIt != m_inventory.end()) {
            total += itemIt->second.m_attackBonus;
        }
    }
    
    // Add set bonuses
    auto activeSets = getActiveSetBonuses();
    for (const auto& setName : activeSets) {
        for (const auto& setBonus : m_setBonuses) {
            if (setBonus.m_setName == setName) {
                total += setBonus.m_attackBonus;
            }
        }
    }
    
    return total;
}

int EquipmentSystem::getTotalDefense() const {
    int total = m_baseStats.m_defense;
    
    for (const auto& pair : m_equippedItems) {
        auto itemIt = m_inventory.find(pair.second);
        if (itemIt != m_inventory.end()) {
            total += itemIt->second.m_defenseBonus;
        }
    }
    
    // Add set bonuses
    auto activeSets = getActiveSetBonuses();
    for (const auto& setName : activeSets) {
        for (const auto& setBonus : m_setBonuses) {
            if (setBonus.m_setName == setName) {
                total += setBonus.m_defenseBonus;
            }
        }
    }
    
    return total;
}

int EquipmentSystem::getTotalSpeed() const {
    int total = m_baseStats.m_speed;
    
    for (const auto& pair : m_equippedItems) {
        auto itemIt = m_inventory.find(pair.second);
        if (itemIt != m_inventory.end()) {
            total += itemIt->second.m_specialBonus; // Using special as speed for now
        }
    }
    
    // Add set bonuses
    auto activeSets = getActiveSetBonuses();
    for (const auto& setName : activeSets) {
        for (const auto& setBonus : m_setBonuses) {
            if (setBonus.m_setName == setName) {
                total += setBonus.m_specialBonus;
            }
        }
    }
    
    return total;
}

int EquipmentSystem::getTotalHealth() const {
    return m_baseStats.m_health; // Could add health bonuses from equipment
}

int EquipmentSystem::getTotalMana() const {
    int total = m_baseStats.m_mana;
    
    // Add set bonuses
    auto activeSets = getActiveSetBonuses();
    for (const auto& setName : activeSets) {
        for (const auto& setBonus : m_setBonuses) {
            if (setBonus.m_setName == setName) {
                total += setBonus.m_manaBonus;
            }
        }
    }
    
    return total;
}

std::vector<std::string> EquipmentSystem::getActiveSetBonuses() const {
    std::vector<std::string> activeSets;
    auto equippedIds = getEquippedItemIds();
    
    for (const auto& setBonus : m_setBonuses) {
        if (setBonus.isActive(equippedIds)) {
            activeSets.push_back(setBonus.m_setName);
        }
    }
    
    return activeSets;
}

void EquipmentSystem::selectItem(const std::string& itemId) {
    m_selectedItemId = itemId;
    refreshItemDetails();
    refreshComparisonPanel();
}

void EquipmentSystem::selectSlot(EquipmentSlot slot) {
    m_selectedSlot = slot;
    refreshComparisonPanel();
}

void EquipmentSystem::setEquipmentChangedCallback(std::function<void(const std::string&, EquipmentSlot)> callback) {
    m_onEquipmentChanged = callback;
}

void EquipmentSystem::setPresetLoadedCallback(std::function<void(const EquipmentPreset&)> callback) {
    m_onPresetLoaded = callback;
}

void EquipmentSystem::setCloseCallback(std::function<void()> callback) {
    m_onClose = callback;
}

std::string EquipmentSystem::getSlotName(EquipmentSlot slot) const {
    switch (slot) {
        case EquipmentSlot::Weapon: return "Weapon";
        case EquipmentSlot::Helmet: return "Helmet";
        case EquipmentSlot::Armor: return "Armor";
        case EquipmentSlot::Trinket: return "Trinket";
        case EquipmentSlot::Fashion_Hair: return "Hair";
        case EquipmentSlot::Fashion_Face: return "Face";
        case EquipmentSlot::Fashion_Body: return "Body";
        default: return "Unknown";
    }
}

std::string EquipmentSystem::getSlotKeyBinding(EquipmentSlot slot) const {
    switch (slot) {
        case EquipmentSlot::Weapon: return "1";
        case EquipmentSlot::Helmet: return "2";
        case EquipmentSlot::Armor: return "3";
        case EquipmentSlot::Trinket: return "4";
        default: return "";
    }
}

bool EquipmentSystem::isSlotCombatGear(EquipmentSlot slot) const {
    return slot == EquipmentSlot::Weapon || 
           slot == EquipmentSlot::Helmet || 
           slot == EquipmentSlot::Armor || 
           slot == EquipmentSlot::Trinket;
}

bool EquipmentSystem::canEquipItem(const std::string& itemId, EquipmentSlot slot) const {
    auto itemIt = m_inventory.find(itemId);
    if (itemIt == m_inventory.end()) {
        return false;
    }
    
    return itemIt->second.m_slot == slot;
}

std::vector<std::string> EquipmentSystem::getEquippedItemIds() const {
    std::vector<std::string> ids;
    for (const auto& pair : m_equippedItems) {
        ids.push_back(pair.second);
    }
    return ids;
}

} // namespace ArenaFighter