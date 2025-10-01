#include "EquipmentTypes.h"
#include <algorithm>

namespace ArenaFighter {

EquipmentItem::EquipmentItem()
    : m_slot(EquipmentSlot::Weapon)
    , m_rarity(ItemRarity::Common)
    , m_attackBonus(0)
    , m_defenseBonus(0)
    , m_specialBonus(0)
    , m_manaCost(0)
    , m_skillDuration(0.0f) {
}

XMFLOAT4 EquipmentItem::getRarityColor() const {
    switch (m_rarity) {
        case ItemRarity::Common:
            return XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);  // Gray
        case ItemRarity::Uncommon:
            return XMFLOAT4(0.2f, 0.8f, 0.2f, 1.0f);  // Green
        case ItemRarity::Rare:
            return XMFLOAT4(0.2f, 0.4f, 1.0f, 1.0f);  // Blue
        case ItemRarity::Epic:
            return XMFLOAT4(0.6f, 0.2f, 0.8f, 1.0f);  // Purple
        case ItemRarity::Legendary:
            return XMFLOAT4(1.0f, 0.6f, 0.0f, 1.0f);  // Orange
        case ItemRarity::Unique:
            return XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f);  // Red
        default:
            return XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);  // White
    }
}

std::string EquipmentItem::getRarityString() const {
    switch (m_rarity) {
        case ItemRarity::Common:     return "Common";
        case ItemRarity::Uncommon:   return "Uncommon";
        case ItemRarity::Rare:       return "Rare";
        case ItemRarity::Epic:       return "Epic";
        case ItemRarity::Legendary:  return "Legendary";
        case ItemRarity::Unique:     return "Unique";
        default:                     return "Unknown";
    }
}

std::string EquipmentItem::getSlotString() const {
    switch (m_slot) {
        case EquipmentSlot::Weapon:        return "Weapon";
        case EquipmentSlot::Helmet:        return "Helmet";
        case EquipmentSlot::Armor:         return "Armor";
        case EquipmentSlot::Trinket:       return "Trinket";
        case EquipmentSlot::Fashion_Hair:  return "Hair";
        case EquipmentSlot::Fashion_Face:  return "Face";
        case EquipmentSlot::Fashion_Body:  return "Body";
        default:                           return "Unknown";
    }
}

bool EquipmentItem::hasCombatSkill() const {
    // Combat gear has skills with mana costs
    return m_manaCost > 0 && !m_skillName.empty();
}

bool SetBonus::isActive(const std::vector<std::string>& equippedIds) const {
    int matchCount = 0;
    
    for (const auto& requiredId : m_requiredItemIds) {
        if (std::find(equippedIds.begin(), equippedIds.end(), requiredId) != equippedIds.end()) {
            matchCount++;
        }
    }
    
    return matchCount >= m_piecesRequired;
}

int EquipmentPreset::calculateTotalAttack(const std::map<std::string, EquipmentItem>& inventory) const {
    int total = 0;
    
    for (const auto& pair : m_equippedItems) {
        auto itemIt = inventory.find(pair.second);
        if (itemIt != inventory.end()) {
            total += itemIt->second.m_attackBonus;
        }
    }
    
    return total;
}

int EquipmentPreset::calculateTotalDefense(const std::map<std::string, EquipmentItem>& inventory) const {
    int total = 0;
    
    for (const auto& pair : m_equippedItems) {
        auto itemIt = inventory.find(pair.second);
        if (itemIt != inventory.end()) {
            total += itemIt->second.m_defenseBonus;
        }
    }
    
    return total;
}

int EquipmentPreset::calculateTotalSpecial(const std::map<std::string, EquipmentItem>& inventory) const {
    int total = 0;
    
    for (const auto& pair : m_equippedItems) {
        auto itemIt = inventory.find(pair.second);
        if (itemIt != inventory.end()) {
            total += itemIt->second.m_specialBonus;
        }
    }
    
    return total;
}

} // namespace ArenaFighter