#pragma once

#include <string>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <d3d11.h>

namespace ArenaFighter {

using namespace DirectX;
using Microsoft::WRL::ComPtr;

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

enum class ItemRarity {
    Common,
    Uncommon,
    Rare,
    Epic,
    Legendary,
    Unique
};

struct GearSkillData {
    std::string m_id;
    std::string m_name;
    std::string m_description;
    float m_manaCost;        // Mana cost instead of cooldown
    float m_baseDamage;
    float m_duration;        // For buff/debuff skills
    float m_range;
    int m_startupFrames;
    int m_activeFrames;
    int m_recoveryFrames;
};

struct EquipmentItem {
    std::string m_id;
    std::string m_name;
    std::string m_description;
    EquipmentSlot m_slot;
    ItemRarity m_rarity;
    
    // Base stat bonuses
    int m_attackBonus;
    int m_defenseBonus;
    int m_speedBonus;
    int m_healthBonus;
    int m_manaBonus;
    
    // Gear skills (for combat equipment only)
    GearSkillData m_skill1;
    GearSkillData m_skill2;
    
    // Visual assets
    ComPtr<ID3D11ShaderResourceView> m_iconTexture;
    ComPtr<ID3D11ShaderResourceView> m_previewTexture;
    
    EquipmentItem();
    XMFLOAT4 getRarityColor() const;
    bool hasCombatSkills() const;
};

} // namespace ArenaFighter