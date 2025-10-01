#pragma once

#include "ShopSystemAdapter.h"
#include "../Characters/CharacterBase.h"
#include <memory>
#include <vector>
#include <unordered_map>

namespace ArenaFighter {

/**
 * @brief Manages item effects and stat modifications for characters
 * 
 * This extends CharacterBase functionality to support the shop system
 */
class ItemManager {
private:
    // Track item modifications per character
    struct CharacterItemData {
        // Base stats (before items)
        float baseMaxHealth;
        float baseMaxMana;
        float baseDefense;
        float baseSpeed;
        float baseCriticalChance;
        float basePowerModifier;
        float baseManaRegen;
        
        // Item bonuses
        float itemHealthBonus = 0;
        float itemManaBonus = 0;
        float itemDefenseBonus = 0;
        float itemSpeedBonus = 0;
        float itemCriticalChanceBonus = 0;
        float itemPowerModifierBonus = 0;
        float itemManaRegenBonus = 0;
        
        // DFR-specific bonuses
        float gearCooldownReduction = 0;      // Reduces gear skill cooldowns ONLY
        float specialMoveDamageBonus = 0;     // Bonus damage for S+Direction moves
        float gearSkillDamageBonus = 0;       // Bonus damage for gear skills
        float blockDamageReduction = 0;       // Extra reduction when blocking
        
        // Active consumable effects
        std::vector<std::unique_ptr<ConsumableEffect>> activeEffects;
    };
    
    std::unordered_map<int, CharacterItemData> m_characterData;
    static ItemManager* s_instance;
    
public:
    static ItemManager& GetInstance() {
        if (!s_instance) {
            s_instance = new ItemManager();
        }
        return *s_instance;
    }
    
    /**
     * @brief Initialize item data for a character
     */
    void InitializeCharacter(CharacterBase* character);
    
    /**
     * @brief Apply equipped items from shop to character
     */
    void ApplyEquippedItems(CharacterBase* character, 
                          const std::vector<DFRShopItem>& equippedItems);
    
    /**
     * @brief Remove all item effects from character
     */
    void RemoveAllItemEffects(CharacterBase* character);
    
    /**
     * @brief Use a consumable item on character
     */
    void UseConsumable(CharacterBase* character, const DFRShopItem& consumable);
    
    /**
     * @brief Update active consumable effects
     */
    void UpdateConsumableEffects(CharacterBase* character, float deltaTime);
    
    /**
     * @brief Get total stats including items
     */
    float GetTotalMaxHealth(CharacterBase* character) const;
    float GetTotalMaxMana(CharacterBase* character) const;
    float GetTotalDefense(CharacterBase* character) const;
    float GetTotalSpeed(CharacterBase* character) const;
    float GetTotalCriticalChance(CharacterBase* character) const;
    float GetTotalPowerModifier(CharacterBase* character) const;
    float GetTotalManaRegen(CharacterBase* character) const;
    
    /**
     * @brief Get DFR-specific bonuses
     */
    float GetGearCooldownReduction(CharacterBase* character) const;
    float GetSpecialMoveDamageBonus(CharacterBase* character) const;
    float GetGearSkillDamageBonus(CharacterBase* character) const;
    float GetBlockDamageReduction(CharacterBase* character) const;
    
    /**
     * @brief Calculate actual gear skill cooldown with items
     */
    float CalculateGearSkillCooldown(CharacterBase* character, int skillIndex) const;
    
    /**
     * @brief Calculate special move damage with bonuses
     */
    float CalculateSpecialMoveDamage(CharacterBase* character, float baseDamage) const;
    
    /**
     * @brief Calculate gear skill damage with bonuses
     */
    float CalculateGearSkillDamage(CharacterBase* character, float baseDamage, 
                                 int gearSlot = -1) const;
    
private:
    ItemManager() = default;
    
    /**
     * @brief Store base stats before item application
     */
    void StoreBaseStats(CharacterBase* character, CharacterItemData& data);
    
    /**
     * @brief Apply a single item's stats
     */
    void ApplyItemStats(CharacterBase* character, const DFRShopItem& item,
                       CharacterItemData& data);
    
    /**
     * @brief Create consumable effect based on item
     */
    std::unique_ptr<ConsumableEffect> CreateConsumableEffect(const DFRShopItem& consumable);
};

/**
 * @brief Extended character stats for item system
 * 
 * This would be integrated into CharacterBase in a full implementation
 */
class CharacterStatsExtended {
public:
    static float GetModifiedManaRegen(CharacterBase* character) {
        float baseRegen = 5.0f; // CLAUDE.md: 5 mana per second
        float itemBonus = ItemManager::GetInstance().GetTotalManaRegen(character);
        return baseRegen + itemBonus;
    }
    
    static float GetModifiedGearCooldown(CharacterBase* character, int skillIndex) {
        return ItemManager::GetInstance().CalculateGearSkillCooldown(character, skillIndex);
    }
    
    static float GetModifiedSpecialMoveDamage(CharacterBase* character, 
                                            InputDirection direction) {
        const SpecialMove* move = character->GetSpecialMove(direction);
        if (!move) return 0;
        
        return ItemManager::GetInstance().CalculateSpecialMoveDamage(
            character, move->baseDamage);
    }
    
    static float GetModifiedGearSkillDamage(CharacterBase* character, int skillIndex) {
        if (skillIndex < 0 || skillIndex >= 8) return 0;
        
        const auto& skill = character->GetGearSkills()[skillIndex];
        int gearSlot = skillIndex / 2; // 2 skills per gear
        
        return ItemManager::GetInstance().CalculateGearSkillDamage(
            character, skill.baseDamage, gearSlot);
    }
    
    static float GetModifiedBlockReduction(CharacterBase* character) {
        float baseReduction = 0.5f; // 50% base block reduction
        float itemBonus = ItemManager::GetInstance().GetBlockDamageReduction(character);
        return std::min(0.9f, baseReduction + itemBonus); // Cap at 90% reduction
    }
};

} // namespace ArenaFighter