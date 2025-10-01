#include "EquipmentTypes.h"

namespace ArenaFighter {

EquipmentItem::EquipmentItem()
    : m_slot(EquipmentSlot::Weapon), 
      m_rarity(ItemRarity::Common),
      m_attackBonus(0), 
      m_defenseBonus(0), 
      m_speedBonus(0),
      m_healthBonus(0),
      m_manaBonus(0) {
    
    // Initialize empty skills
    m_skill1.m_manaCost = 0.0f;
    m_skill2.m_manaCost = 0.0f;
}

XMFLOAT4 EquipmentItem::getRarityColor() const {
    switch (m_rarity) {
        case ItemRarity::Common:    return XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);  // Gray
        case ItemRarity::Uncommon:  return XMFLOAT4(0.3f, 0.8f, 0.3f, 1.0f);  // Green
        case ItemRarity::Rare:      return XMFLOAT4(0.3f, 0.5f, 0.9f, 1.0f);  // Blue
        case ItemRarity::Epic:      return XMFLOAT4(0.7f, 0.3f, 0.9f, 1.0f);  // Purple
        case ItemRarity::Legendary: return XMFLOAT4(1.0f, 0.6f, 0.0f, 1.0f);  // Orange
        case ItemRarity::Unique:    return XMFLOAT4(1.0f, 0.3f, 0.3f, 1.0f);  // Red
        default:                    return XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);  // White
    }
}

bool EquipmentItem::hasCombatSkills() const {
    // Only combat equipment slots have skills
    return (m_slot == EquipmentSlot::Weapon || 
            m_slot == EquipmentSlot::Helmet || 
            m_slot == EquipmentSlot::Armor || 
            m_slot == EquipmentSlot::Trinket) &&
           (m_skill1.m_manaCost > 0 || m_skill2.m_manaCost > 0);
}

} // namespace ArenaFighter