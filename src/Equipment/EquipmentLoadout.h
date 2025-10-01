#pragma once

#include "EquipmentTypes.h"
#include <unordered_map>
#include <memory>

namespace ArenaFighter {

class EquipmentLoadout {
private:
    std::string m_name;
    std::unordered_map<EquipmentSlot, std::shared_ptr<EquipmentItem>> m_equippedItems;
    
public:
    EquipmentLoadout(const std::string& loadoutName = "Default");
    
    void equipItem(std::shared_ptr<EquipmentItem> item);
    void unequipSlot(EquipmentSlot slot);
    std::shared_ptr<EquipmentItem> getEquippedItem(EquipmentSlot slot) const;
    
    // Stat calculations
    int getTotalAttack() const;
    int getTotalDefense() const;
    int getTotalSpeed() const;
    int getTotalHealth() const;
    int getTotalMana() const;
    
    // Get all gear skills
    std::vector<GearSkillData> getAllGearSkills() const;
    
    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }
    
    bool hasItemInSlot(EquipmentSlot slot) const;
    void clear();
};

} // namespace ArenaFighter