#include "EquipmentLoadout.h"

namespace ArenaFighter {

EquipmentLoadout::EquipmentLoadout(const std::string& loadoutName)
    : m_name(loadoutName) {}

void EquipmentLoadout::equipItem(std::shared_ptr<EquipmentItem> item) {
    if (item) {
        m_equippedItems[item->m_slot] = item;
    }
}

void EquipmentLoadout::unequipSlot(EquipmentSlot slot) {
    m_equippedItems.erase(slot);
}

std::shared_ptr<EquipmentItem> EquipmentLoadout::getEquippedItem(EquipmentSlot slot) const {
    auto it = m_equippedItems.find(slot);
    return (it != m_equippedItems.end()) ? it->second : nullptr;
}

int EquipmentLoadout::getTotalAttack() const {
    int total = 0;
    for (const auto& [slot, item] : m_equippedItems) {
        if (item) total += item->m_attackBonus;
    }
    return total;
}

int EquipmentLoadout::getTotalDefense() const {
    int total = BASE_DEFENSE;  // Start with base defense
    for (const auto& [slot, item] : m_equippedItems) {
        if (item) total += item->m_defenseBonus;
    }
    return total;
}

int EquipmentLoadout::getTotalSpeed() const {
    int total = BASE_SPEED;  // Start with base speed
    for (const auto& [slot, item] : m_equippedItems) {
        if (item) total += item->m_speedBonus;
    }
    return total;
}

int EquipmentLoadout::getTotalHealth() const {
    int total = 0;
    for (const auto& [slot, item] : m_equippedItems) {
        if (item) total += item->m_healthBonus;
    }
    return total;
}

int EquipmentLoadout::getTotalMana() const {
    int total = 0;
    for (const auto& [slot, item] : m_equippedItems) {
        if (item) total += item->m_manaBonus;
    }
    return total;
}

std::vector<GearSkillData> EquipmentLoadout::getAllGearSkills() const {
    std::vector<GearSkillData> skills;
    
    // Collect skills from combat equipment (4 gears x 2 skills = 8 total)
    for (const auto& [slot, item] : m_equippedItems) {
        if (item && item->hasCombatSkills()) {
            if (item->m_skill1.m_manaCost > 0) {
                skills.push_back(item->m_skill1);
            }
            if (item->m_skill2.m_manaCost > 0) {
                skills.push_back(item->m_skill2);
            }
        }
    }
    
    return skills;
}

bool EquipmentLoadout::hasItemInSlot(EquipmentSlot slot) const {
    return m_equippedItems.find(slot) != m_equippedItems.end();
}

void EquipmentLoadout::clear() {
    m_equippedItems.clear();
}

} // namespace ArenaFighter