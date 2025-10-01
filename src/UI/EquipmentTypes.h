#pragma once

#include <string>
#include <vector>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <d3d11.h>

using namespace DirectX;
using Microsoft::WRL::ComPtr;

namespace ArenaFighter {

// Equipment slot types
enum class EquipmentSlot {
    Weapon,
    Helmet,
    Armor,
    Trinket,
    Fashion_Hair,
    Fashion_Face,
    Fashion_Body,
    Count
};

// Item rarity
enum class ItemRarity {
    Common,
    Uncommon,
    Rare,
    Epic,
    Legendary,
    Unique
};

// Equipment item data
struct EquipmentItem {
    std::string m_id;
    std::string m_name;
    std::string m_description;
    EquipmentSlot m_slot;
    ItemRarity m_rarity;
    
    // Stats bonuses
    int m_attackBonus;
    int m_defenseBonus;
    int m_specialBonus;
    
    // Skill data (mana-based, no cooldowns per CLAUDE.md)
    std::string m_skillName;
    std::string m_skillDescription;
    int m_manaCost;          // Mana cost instead of cooldown
    float m_skillDuration;   // Duration for buff effects
    
    // Textures using ComPtr for safety
    ComPtr<ID3D11ShaderResourceView> m_iconTexture;
    ComPtr<ID3D11ShaderResourceView> m_previewTexture;
    
    EquipmentItem();
    
    XMFLOAT4 getRarityColor() const;
    std::string getRarityString() const;
    std::string getSlotString() const;
    bool hasCombatSkill() const;
};

// Equipment set bonus
struct SetBonus {
    std::string m_setName;
    std::vector<std::string> m_requiredItemIds;
    int m_piecesRequired;
    
    // Bonuses when set is active
    int m_attackBonus;
    int m_defenseBonus;
    int m_specialBonus;
    int m_manaBonus;
    
    bool isActive(const std::vector<std::string>& equippedIds) const;
};

// Equipment comparison result
struct EquipmentComparison {
    int m_attackDiff;
    int m_defenseDiff;
    int m_specialDiff;
    int m_totalDiff;
    
    bool isBetter() const { return m_totalDiff > 0; }
};

// Equipment filter criteria
struct EquipmentFilter {
    enum class SortBy {
        Name,
        Rarity,
        Attack,
        Defense,
        Special,
        Total,
        Slot
    };
    
    enum class FilterType {
        All,
        Weapon,
        Armor,
        Helmet,
        Trinket,
        Fashion,
        CombatGear,  // Items with skills
        Rarity
    };
    
    FilterType m_filterType;
    ItemRarity m_rarityFilter;
    SortBy m_sortBy;
    bool m_ascending;
    
    EquipmentFilter() 
        : m_filterType(FilterType::All)
        , m_rarityFilter(ItemRarity::Common)
        , m_sortBy(SortBy::Name)
        , m_ascending(true) {}
};

// Equipment loadout preset
struct EquipmentPreset {
    std::string m_name;
    std::string m_description;
    std::map<EquipmentSlot, std::string> m_equippedItems;
    
    int calculateTotalAttack(const std::map<std::string, EquipmentItem>& inventory) const;
    int calculateTotalDefense(const std::map<std::string, EquipmentItem>& inventory) const;
    int calculateTotalSpecial(const std::map<std::string, EquipmentItem>& inventory) const;
};

} // namespace ArenaFighter