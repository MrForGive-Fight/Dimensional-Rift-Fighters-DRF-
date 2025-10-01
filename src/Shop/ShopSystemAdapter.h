#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include "../Characters/CharacterBase.h"

namespace ArenaFighter {

/**
 * @brief Item categories for DFR shop system
 */
enum class ItemCategory {
    Weapon,
    Armor, 
    Accessory,
    Consumable,
    Cosmetic,
    GearEnhancement  // New category for gear skill improvements
};

/**
 * @brief Item rarity tiers
 */
enum class ItemRarity {
    Common,
    Uncommon,
    Rare,
    Epic,
    Legendary,
    Mythic  // Added for ultra-rare items
};

/**
 * @brief Shop item aligned with DFR's system
 * 
 * Key difference: cooldownReduction only affects GEAR SKILLS, not special moves
 */
struct DFRShopItem {
    int id;
    std::string name;
    std::string description;
    ItemCategory category;
    ItemRarity rarity;
    int price;
    int levelRequirement;
    
    // Stat modifiers
    float healthBonus;
    float manaBonus;
    float attackBonus;
    float defenseBonus;
    float speedBonus;
    float criticalChanceBonus;
    float powerModifierBonus;
    
    // DFR-specific modifiers
    float manaRegenBonus;          // Increases mana regen rate
    float gearCooldownReduction;   // ONLY affects gear skills (AS, AD, ASD, SD)
    float specialMoveDamageBonus;  // Bonus damage for S+Direction moves
    float blockDamageReduction;    // Extra damage reduction when blocking
    
    // Gear-specific enhancements
    int targetGearSlot;            // -1 for general, 0-3 for specific gear
    float gearSkillDamageBonus;    // Bonus damage for gear skills
    
    // Item state
    bool isPurchased;
    bool isEquipped;
    int stackCount;  // For consumables
};

/**
 * @brief Adapter to integrate shop system with DFR
 */
class ShopSystemAdapter {
public:
    class DFRShopSystem {
    private:
        std::vector<DFRShopItem> m_shopInventory;
        std::vector<DFRShopItem> m_playerInventory;
        int m_playerCurrency;
        int m_playerLevel;
        
        std::function<void(const std::string&)> m_onPurchaseEvent;
        std::function<void(const DFRShopItem&)> m_onItemEquipped;
        
    public:
        DFRShopSystem();
        
        // Shop operations
        bool PurchaseItem(int itemId);
        bool EquipItem(int itemId);
        bool UnequipItem(int itemId);
        bool UseConsumable(int itemId);
        
        // Apply items to DFR character
        void ApplyEquippedItemsToCharacter(CharacterBase* character);
        void RemoveItemEffectsFromCharacter(CharacterBase* character);
        
        // Query methods
        std::vector<DFRShopItem> GetShopItemsByCategory(ItemCategory category) const;
        std::vector<DFRShopItem> GetAvailableItems() const;
        const std::vector<DFRShopItem>& GetPlayerInventory() const { return m_playerInventory; }
        std::vector<DFRShopItem> GetEquippedItems() const;
        
        // Currency and level
        void AddCurrency(int amount) { m_playerCurrency += amount; }
        void SetPlayerLevel(int level) { m_playerLevel = level; }
        int GetPlayerCurrency() const { return m_playerCurrency; }
        int GetPlayerLevel() const { return m_playerLevel; }
        
        // Callbacks
        void SetPurchaseEventCallback(std::function<void(const std::string&)> callback) {
            m_onPurchaseEvent = callback;
        }
        void SetItemEquippedCallback(std::function<void(const DFRShopItem&)> callback) {
            m_onItemEquipped = callback;
        }
        
        // Helper methods
        std::string GetRarityColor(ItemRarity rarity) const;
        std::string GetRarityName(ItemRarity rarity) const;
        
    private:
        void InitializeShop();
        void CreateWeapons();
        void CreateArmor();
        void CreateAccessories();
        void CreateConsumables();
        void CreateCosmetics();
        void CreateGearEnhancements();
    };
    
    /**
     * @brief Wrapper to make provided ShopSystem work with DFR
     */
    class ShopSystemWrapper {
    private:
        CharacterBase* m_character;
        
        // Store original stats for proper removal
        struct OriginalStats {
            float maxHealth;
            float maxMana;
            float defense;
            float speed;
            float criticalChance;
            float powerModifier;
        };
        OriginalStats m_originalStats;
        
    public:
        ShopSystemWrapper(CharacterBase* character) : m_character(character) {
            if (m_character) {
                // Store original stats
                m_originalStats.maxHealth = m_character->GetMaxHealth();
                m_originalStats.maxMana = m_character->GetMaxMana();
                m_originalStats.defense = m_character->GetDefense();
                m_originalStats.speed = m_character->GetSpeed();
                m_originalStats.criticalChance = m_character->GetCriticalChance();
                m_originalStats.powerModifier = m_character->GetPowerModifier();
            }
        }
        
        /**
         * @brief Apply item stats from provided ShopSystem format
         * Maps cooldownReduction to gear skills only
         */
        void ApplyProvidedShopItem(float healthBonus, float manaBonus,
                                 float attackBonus, float defenseBonus,
                                 float skillPowerBonus, float speedBonus,
                                 float critChanceBonus, float critDamageBonus,
                                 float cooldownReduction);
        
        /**
         * @brief Apply DFR-specific item
         */
        void ApplyDFRShopItem(const DFRShopItem& item);
        
        /**
         * @brief Remove all item effects
         */
        void RemoveAllItemEffects();
    };
};

/**
 * @brief Item effect that modifies character stats temporarily
 */
class ItemEffect {
public:
    virtual ~ItemEffect() = default;
    virtual void Apply(CharacterBase* character) = 0;
    virtual void Remove(CharacterBase* character) = 0;
    virtual float GetDuration() const { return -1.0f; } // -1 for permanent
};

/**
 * @brief Consumable effect for temporary buffs
 */
class ConsumableEffect : public ItemEffect {
private:
    float m_duration;
    float m_elapsed;
    std::function<void(CharacterBase*)> m_applyFunc;
    std::function<void(CharacterBase*)> m_removeFunc;
    
public:
    ConsumableEffect(float duration,
                    std::function<void(CharacterBase*)> apply,
                    std::function<void(CharacterBase*)> remove)
        : m_duration(duration)
        , m_elapsed(0.0f)
        , m_applyFunc(apply)
        , m_removeFunc(remove) {}
    
    void Apply(CharacterBase* character) override { m_applyFunc(character); }
    void Remove(CharacterBase* character) override { m_removeFunc(character); }
    float GetDuration() const override { return m_duration; }
    
    bool Update(float deltaTime) {
        m_elapsed += deltaTime;
        return m_elapsed >= m_duration;
    }
};

} // namespace ArenaFighter