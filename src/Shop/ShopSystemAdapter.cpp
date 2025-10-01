#include "ShopSystemAdapter.h"
#include <algorithm>
#include <cmath>

namespace ArenaFighter {

// DFRShopSystem Implementation

ShopSystemAdapter::DFRShopSystem::DFRShopSystem() 
    : m_playerCurrency(1000)
    , m_playerLevel(1) {
    InitializeShop();
}

void ShopSystemAdapter::DFRShopSystem::InitializeShop() {
    CreateWeapons();
    CreateArmor();
    CreateAccessories();
    CreateConsumables();
    CreateCosmetics();
    CreateGearEnhancements();
}

void ShopSystemAdapter::DFRShopSystem::CreateWeapons() {
    // Basic weapons
    m_shopInventory.push_back({
        1, "Iron Spear", "A basic spear for beginners",
        ItemCategory::Weapon, ItemRarity::Common, 100, 1,
        0, 0, 10.0f, 0, 0, 0, 0,  // Basic stats
        0, 0, 0, 0,                // DFR-specific
        -1, 0,                     // Gear enhancement
        false, false, 0
    });
    
    m_shopInventory.push_back({
        2, "Steel Blade", "Well-crafted steel blade",
        ItemCategory::Weapon, ItemRarity::Uncommon, 300, 3,
        0, 0, 25.0f, 0, 0, 0.02f, 0,
        0, 0, 5.0f, 0,             // +5% special move damage
        -1, 0,
        false, false, 0
    });
    
    m_shopInventory.push_back({
        3, "Heavenly Demon Spear", "Legendary weapon of S-tier Murim warriors",
        ItemCategory::Weapon, ItemRarity::Legendary, 2000, 10,
        0, 0, 100.0f, 10.0f, 0, 0.1f, 0.2f,
        2.0f, 0, 20.0f, 0,         // +2 mana/sec, +20% special move damage
        -1, 0,
        false, false, 0
    });
    
    m_shopInventory.push_back({
        4, "Cultivation Sword", "Channels qi for enhanced abilities",
        ItemCategory::Weapon, ItemRarity::Epic, 1500, 8,
        0, 50, 50.0f, 0, 0, 0.05f, 0.15f,
        3.0f, 0, 15.0f, 0,         // +3 mana/sec for special moves
        -1, 0,
        false, false, 0
    });
}

void ShopSystemAdapter::DFRShopSystem::CreateArmor() {
    m_shopInventory.push_back({
        10, "Cloth Robe", "Basic protection",
        ItemCategory::Armor, ItemRarity::Common, 80, 1,
        50, 10, 0, 10.0f, 0, 0, 0,
        0, 0, 0, 0,
        -1, 0,
        false, false, 0
    });
    
    m_shopInventory.push_back({
        11, "Martial Arts Gi", "Traditional Murim fighting garb",
        ItemCategory::Armor, ItemRarity::Uncommon, 250, 3,
        100, 20, 0, 25.0f, 5.0f, 0, 0.05f,
        1.0f, 0, 0, 5.0f,          // +5% block reduction
        -1, 0,
        false, false, 0
    });
    
    m_shopInventory.push_back({
        12, "Demon Lord Armor", "Armor of the Heavenly Demon cult",
        ItemCategory::Armor, ItemRarity::Epic, 1500, 8,
        200, 50, 5.0f, 50.0f, 0, 0, 0.1f,
        2.0f, 0.1f, 0, 10.0f,      // 10% gear cooldown reduction
        -1, 0,
        false, false, 0
    });
    
    m_shopInventory.push_back({
        13, "Divine Beast Hide", "Crafted from legendary beasts",
        ItemCategory::Armor, ItemRarity::Legendary, 2500, 12,
        300, 75, 0, 75.0f, 10.0f, 0.05f, 0.15f,
        3.0f, 0.15f, 10.0f, 15.0f,
        -1, 0,
        false, false, 0
    });
}

void ShopSystemAdapter::DFRShopSystem::CreateAccessories() {
    m_shopInventory.push_back({
        20, "Swift Boots", "Increases movement speed",
        ItemCategory::Accessory, ItemRarity::Rare, 400, 4,
        0, 30, 0, 0, 20.0f, 0, 0,
        0, 0, 0, 0,
        -1, 0,
        false, false, 0
    });
    
    m_shopInventory.push_back({
        21, "Ring of Power", "Enhances critical strikes",
        ItemCategory::Accessory, ItemRarity::Rare, 500, 5,
        0, 0, 0, 0, 0, 0.15f, 0.1f,
        0, 0, 0, 0,
        -1, 0,
        false, false, 0
    });
    
    m_shopInventory.push_back({
        22, "Master's Amulet", "Boosts all abilities",
        ItemCategory::Accessory, ItemRarity::Legendary, 3000, 15,
        100, 50, 20.0f, 20.0f, 10.0f, 0.05f, 0.2f,
        5.0f, 0.2f, 15.0f, 0,      // Major boosts
        -1, 0,
        false, false, 0
    });
    
    m_shopInventory.push_back({
        23, "Mana Crystal Pendant", "Enhances mana regeneration",
        ItemCategory::Accessory, ItemRarity::Epic, 1200, 7,
        0, 100, 0, 0, 0, 0, 0,
        10.0f, 0, 0, 0,            // +10 mana/sec (massive boost)
        -1, 0,
        false, false, 0
    });
}

void ShopSystemAdapter::DFRShopSystem::CreateConsumables() {
    m_shopInventory.push_back({
        30, "Health Elixir", "Restores 200 HP instantly",
        ItemCategory::Consumable, ItemRarity::Common, 50, 1,
        200, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,
        -1, 0,
        false, false, 10           // Stack of 10
    });
    
    m_shopInventory.push_back({
        31, "Mana Potion", "Restores 50 Mana instantly",
        ItemCategory::Consumable, ItemRarity::Common, 40, 1,
        0, 50, 0, 0, 0, 0, 0,
        0, 0, 0, 0,
        -1, 0,
        false, false, 10
    });
    
    m_shopInventory.push_back({
        32, "Qi Booster", "Increases special move damage by 50% for 30 seconds",
        ItemCategory::Consumable, ItemRarity::Uncommon, 150, 3,
        0, 0, 0, 0, 0, 0, 0,
        0, 0, 50.0f, 0,            // Temporary 50% special move boost
        -1, 0,
        false, false, 5
    });
    
    m_shopInventory.push_back({
        33, "Gear Cooldown Elixir", "Reduces all gear cooldowns by 50% for 60 seconds",
        ItemCategory::Consumable, ItemRarity::Rare, 300, 5,
        0, 0, 0, 0, 0, 0, 0,
        0, 0.5f, 0, 0,             // Temporary 50% gear cooldown reduction
        -1, 0,
        false, false, 3
    });
}

void ShopSystemAdapter::DFRShopSystem::CreateCosmetics() {
    m_shopInventory.push_back({
        40, "Heavenly Crown", "A radiant crown that glows with divine light",
        ItemCategory::Cosmetic, ItemRarity::Rare, 800, 5,
        0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,
        -1, 0,
        false, false, 0
    });
    
    m_shopInventory.push_back({
        41, "Demon Emperor Cloak", "Dark cloak with crimson aura",
        ItemCategory::Cosmetic, ItemRarity::Epic, 1200, 7,
        0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,
        -1, 0,
        false, false, 0
    });
    
    m_shopInventory.push_back({
        42, "Divine Beast Wings", "Ethereal wings from celestial beasts",
        ItemCategory::Cosmetic, ItemRarity::Legendary, 2000, 10,
        0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,
        -1, 0,
        false, false, 0
    });
}

void ShopSystemAdapter::DFRShopSystem::CreateGearEnhancements() {
    // Gear-specific enhancements (new category unique to DFR)
    m_shopInventory.push_back({
        50, "Gear Enhancement: AS", "Reduces cooldown of AS skills by 20%",
        ItemCategory::GearEnhancement, ItemRarity::Rare, 600, 6,
        0, 0, 0, 0, 0, 0, 0,
        0, 0.2f, 0, 0,
        0, 20.0f,                  // Gear 0, +20% damage
        false, false, 0
    });
    
    m_shopInventory.push_back({
        51, "Gear Enhancement: AD", "Reduces cooldown of AD skills by 20%",
        ItemCategory::GearEnhancement, ItemRarity::Rare, 600, 6,
        0, 0, 0, 0, 0, 0, 0,
        0, 0.2f, 0, 0,
        1, 20.0f,                  // Gear 1, +20% damage
        false, false, 0
    });
    
    m_shopInventory.push_back({
        52, "Master Gear Enhancement", "Reduces ALL gear skill cooldowns by 25%",
        ItemCategory::GearEnhancement, ItemRarity::Legendary, 2500, 12,
        0, 0, 0, 0, 0, 0, 0,
        0, 0.25f, 0, 0,
        -1, 30.0f,                 // All gears, +30% damage
        false, false, 0
    });
}

bool ShopSystemAdapter::DFRShopSystem::PurchaseItem(int itemId) {
    DFRShopItem* item = nullptr;
    for (auto& shopItem : m_shopInventory) {
        if (shopItem.id == itemId) {
            item = &shopItem;
            break;
        }
    }
    
    if (!item) {
        if (m_onPurchaseEvent) {
            m_onPurchaseEvent("Item not found!");
        }
        return false;
    }
    
    if (item->isPurchased && item->category != ItemCategory::Consumable) {
        if (m_onPurchaseEvent) {
            m_onPurchaseEvent("Item already owned!");
        }
        return false;
    }
    
    if (m_playerLevel < item->levelRequirement) {
        if (m_onPurchaseEvent) {
            m_onPurchaseEvent("Level requirement not met! Need level " + 
                            std::to_string(item->levelRequirement));
        }
        return false;
    }
    
    if (m_playerCurrency < item->price) {
        if (m_onPurchaseEvent) {
            m_onPurchaseEvent("Insufficient funds! Need " + 
                            std::to_string(item->price - m_playerCurrency) + " more gold");
        }
        return false;
    }
    
    m_playerCurrency -= item->price;
    
    // For consumables, add to existing stack or create new
    if (item->category == ItemCategory::Consumable) {
        bool found = false;
        for (auto& invItem : m_playerInventory) {
            if (invItem.id == itemId) {
                invItem.stackCount += item->stackCount;
                found = true;
                break;
            }
        }
        if (!found) {
            m_playerInventory.push_back(*item);
        }
    } else {
        item->isPurchased = true;
        m_playerInventory.push_back(*item);
    }
    
    if (m_onPurchaseEvent) {
        m_onPurchaseEvent("Purchased: " + item->name);
    }
    
    return true;
}

bool ShopSystemAdapter::DFRShopSystem::EquipItem(int itemId) {
    DFRShopItem* item = nullptr;
    for (auto& invItem : m_playerInventory) {
        if (invItem.id == itemId) {
            item = &invItem;
            break;
        }
    }
    
    if (!item) {
        return false;
    }
    
    // Can't equip consumables
    if (item->category == ItemCategory::Consumable) {
        return false;
    }
    
    // Unequip other items of same category
    for (auto& invItem : m_playerInventory) {
        if (invItem.category == item->category && invItem.id != itemId) {
            // Special case: Gear enhancements can stack if for different slots
            if (item->category == ItemCategory::GearEnhancement) {
                if (invItem.targetGearSlot == item->targetGearSlot || 
                    item->targetGearSlot == -1 || invItem.targetGearSlot == -1) {
                    invItem.isEquipped = false;
                }
            } else {
                invItem.isEquipped = false;
            }
        }
    }
    
    item->isEquipped = true;
    
    if (m_onItemEquipped) {
        m_onItemEquipped(*item);
    }
    
    return true;
}

bool ShopSystemAdapter::DFRShopSystem::UnequipItem(int itemId) {
    for (auto& item : m_playerInventory) {
        if (item.id == itemId) {
            item.isEquipped = false;
            return true;
        }
    }
    return false;
}

bool ShopSystemAdapter::DFRShopSystem::UseConsumable(int itemId) {
    for (auto it = m_playerInventory.begin(); it != m_playerInventory.end(); ++it) {
        if (it->id == itemId && it->category == ItemCategory::Consumable) {
            it->stackCount--;
            if (it->stackCount <= 0) {
                m_playerInventory.erase(it);
            }
            return true;
        }
    }
    return false;
}

void ShopSystemAdapter::DFRShopSystem::ApplyEquippedItemsToCharacter(CharacterBase* character) {
    if (!character) return;
    
    // Store base stats before modifications
    float baseHealth = character->GetMaxHealth();
    float baseMana = character->GetMaxMana();
    
    // Apply all equipped items
    for (const auto& item : m_playerInventory) {
        if (item.isEquipped && item.category != ItemCategory::Consumable) {
            // Apply basic stat bonuses
            // Note: DFR doesn't expose setters for all stats, so we'd need to extend CharacterBase
            // For now, we'll apply what we can
            
            // Health and mana can be adjusted through damage/heal
            if (item.healthBonus > 0) {
                character->Heal(item.healthBonus);
            }
            
            // Mana regen bonus would need to be tracked separately
            // Same for other bonuses that don't have direct setters
            
            // Gear cooldown reduction affects gear skills only
            if (item.gearCooldownReduction > 0) {
                // This would need to be implemented in CharacterBase
                // For now, it's tracked but not applied
            }
        }
    }
}

void ShopSystemAdapter::DFRShopSystem::RemoveItemEffectsFromCharacter(CharacterBase* character) {
    if (!character) return;
    
    // In a full implementation, we'd restore original stats
    // For now, this is a placeholder
}

std::vector<DFRShopItem> ShopSystemAdapter::DFRShopSystem::GetShopItemsByCategory(
    ItemCategory category) const {
    
    std::vector<DFRShopItem> filtered;
    for (const auto& item : m_shopInventory) {
        if (item.category == category && 
            (item.category == ItemCategory::Consumable || !item.isPurchased)) {
            filtered.push_back(item);
        }
    }
    return filtered;
}

std::vector<DFRShopItem> ShopSystemAdapter::DFRShopSystem::GetAvailableItems() const {
    std::vector<DFRShopItem> available;
    for (const auto& item : m_shopInventory) {
        if ((item.category == ItemCategory::Consumable || !item.isPurchased) && 
            m_playerLevel >= item.levelRequirement) {
            available.push_back(item);
        }
    }
    return available;
}

std::vector<DFRShopItem> ShopSystemAdapter::DFRShopSystem::GetEquippedItems() const {
    std::vector<DFRShopItem> equipped;
    for (const auto& item : m_playerInventory) {
        if (item.isEquipped) {
            equipped.push_back(item);
        }
    }
    return equipped;
}

std::string ShopSystemAdapter::DFRShopSystem::GetRarityColor(ItemRarity rarity) const {
    switch (rarity) {
        case ItemRarity::Common:    return "#FFFFFF";
        case ItemRarity::Uncommon:  return "#1EFF00";
        case ItemRarity::Rare:      return "#0070DD";
        case ItemRarity::Epic:      return "#A335EE";
        case ItemRarity::Legendary: return "#FF8000";
        case ItemRarity::Mythic:    return "#FF0000";
        default:                    return "#FFFFFF";
    }
}

std::string ShopSystemAdapter::DFRShopSystem::GetRarityName(ItemRarity rarity) const {
    switch (rarity) {
        case ItemRarity::Common:    return "Common";
        case ItemRarity::Uncommon:  return "Uncommon";
        case ItemRarity::Rare:      return "Rare";
        case ItemRarity::Epic:      return "Epic";
        case ItemRarity::Legendary: return "Legendary";
        case ItemRarity::Mythic:    return "Mythic";
        default:                    return "Unknown";
    }
}

// ShopSystemWrapper Implementation

void ShopSystemAdapter::ShopSystemWrapper::ApplyProvidedShopItem(
    float healthBonus, float manaBonus,
    float attackBonus, float defenseBonus,
    float skillPowerBonus, float speedBonus,
    float critChanceBonus, float critDamageBonus,
    float cooldownReduction) {
    
    if (!m_character) return;
    
    // Apply stat bonuses that DFR supports
    // Note: Some modifications would require extending CharacterBase
    
    // Health bonus
    if (healthBonus > 0) {
        m_character->Heal(healthBonus);
    }
    
    // For other stats, we'd need setters in CharacterBase
    // The key mapping is:
    // - cooldownReduction -> Only affects gear skills, not special moves
    // - skillPowerBonus -> Maps to powerModifier
    // - attackBonus -> Also maps to powerModifier
    // - critDamageBonus -> Would enhance critical multiplier
}

void ShopSystemAdapter::ShopSystemWrapper::ApplyDFRShopItem(const DFRShopItem& item) {
    if (!m_character) return;
    
    // Apply DFR-specific bonuses
    // This would require extending CharacterBase with item system support
    
    // Key differences:
    // - manaRegenBonus: Increases base mana regen (affects special move frequency)
    // - gearCooldownReduction: ONLY reduces gear skill cooldowns
    // - specialMoveDamageBonus: Increases S+Direction move damage
    // - blockDamageReduction: Extra reduction when blocking
}

void ShopSystemAdapter::ShopSystemWrapper::RemoveAllItemEffects() {
    if (!m_character) return;
    
    // Restore original stats
    // This would require proper stat tracking in CharacterBase
}

} // namespace ArenaFighter