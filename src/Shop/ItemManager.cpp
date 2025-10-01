#include "ItemManager.h"
#include <algorithm>

namespace ArenaFighter {

ItemManager* ItemManager::s_instance = nullptr;

void ItemManager::InitializeCharacter(CharacterBase* character) {
    if (!character) return;
    
    int charId = character->GetId();
    CharacterItemData& data = m_characterData[charId];
    
    // Store base stats
    StoreBaseStats(character, data);
}

void ItemManager::StoreBaseStats(CharacterBase* character, CharacterItemData& data) {
    data.baseMaxHealth = character->GetMaxHealth();
    data.baseMaxMana = character->GetMaxMana();
    data.baseDefense = character->GetDefense();
    data.baseSpeed = character->GetSpeed();
    data.baseCriticalChance = character->GetCriticalChance();
    data.basePowerModifier = character->GetPowerModifier();
    data.baseManaRegen = 5.0f; // Base mana regen from CLAUDE.md
}

void ItemManager::ApplyEquippedItems(CharacterBase* character,
                                    const std::vector<DFRShopItem>& equippedItems) {
    if (!character) return;
    
    int charId = character->GetId();
    CharacterItemData& data = m_characterData[charId];
    
    // Reset item bonuses
    data.itemHealthBonus = 0;
    data.itemManaBonus = 0;
    data.itemDefenseBonus = 0;
    data.itemSpeedBonus = 0;
    data.itemCriticalChanceBonus = 0;
    data.itemPowerModifierBonus = 0;
    data.itemManaRegenBonus = 0;
    data.gearCooldownReduction = 0;
    data.specialMoveDamageBonus = 0;
    data.gearSkillDamageBonus = 0;
    data.blockDamageReduction = 0;
    
    // Apply each equipped item
    for (const auto& item : equippedItems) {
        if (item.isEquipped && item.category != ItemCategory::Consumable) {
            ApplyItemStats(character, item, data);
        }
    }
}

void ItemManager::ApplyItemStats(CharacterBase* character, const DFRShopItem& item,
                               CharacterItemData& data) {
    // Accumulate bonuses
    data.itemHealthBonus += item.healthBonus;
    data.itemManaBonus += item.manaBonus;
    data.itemDefenseBonus += item.defenseBonus;
    data.itemSpeedBonus += item.speedBonus;
    data.itemCriticalChanceBonus += item.criticalChanceBonus;
    data.itemPowerModifierBonus += item.powerModifierBonus;
    data.itemManaRegenBonus += item.manaRegenBonus;
    
    // DFR-specific bonuses
    data.gearCooldownReduction += item.gearCooldownReduction;
    data.specialMoveDamageBonus += item.specialMoveDamageBonus;
    data.blockDamageReduction += item.blockDamageReduction;
    
    // Gear-specific enhancements
    if (item.category == ItemCategory::GearEnhancement) {
        data.gearSkillDamageBonus += item.gearSkillDamageBonus;
    } else {
        // General gear skill damage from other items
        data.gearSkillDamageBonus += item.gearSkillDamageBonus;
    }
    
    // Apply immediate health/mana changes
    if (item.healthBonus > 0) {
        character->Heal(item.healthBonus);
    }
}

void ItemManager::RemoveAllItemEffects(CharacterBase* character) {
    if (!character) return;
    
    int charId = character->GetId();
    CharacterItemData& data = m_characterData[charId];
    
    // Clear all bonuses
    data.itemHealthBonus = 0;
    data.itemManaBonus = 0;
    data.itemDefenseBonus = 0;
    data.itemSpeedBonus = 0;
    data.itemCriticalChanceBonus = 0;
    data.itemPowerModifierBonus = 0;
    data.itemManaRegenBonus = 0;
    data.gearCooldownReduction = 0;
    data.specialMoveDamageBonus = 0;
    data.gearSkillDamageBonus = 0;
    data.blockDamageReduction = 0;
    
    // Clear active effects
    data.activeEffects.clear();
}

void ItemManager::UseConsumable(CharacterBase* character, const DFRShopItem& consumable) {
    if (!character || consumable.category != ItemCategory::Consumable) return;
    
    int charId = character->GetId();
    CharacterItemData& data = m_characterData[charId];
    
    // Create and apply consumable effect
    auto effect = CreateConsumableEffect(consumable);
    if (effect) {
        effect->Apply(character);
        if (effect->GetDuration() > 0) {
            data.activeEffects.push_back(std::move(effect));
        }
    }
}

std::unique_ptr<ConsumableEffect> ItemManager::CreateConsumableEffect(
    const DFRShopItem& consumable) {
    
    // Health Elixir - instant heal
    if (consumable.id == 30) {
        return std::make_unique<ConsumableEffect>(
            0.0f, // Instant
            [health = consumable.healthBonus](CharacterBase* c) { c->Heal(health); },
            [](CharacterBase* c) {} // No removal needed
        );
    }
    
    // Mana Potion - instant mana restore
    if (consumable.id == 31) {
        return std::make_unique<ConsumableEffect>(
            0.0f, // Instant
            [mana = consumable.manaBonus](CharacterBase* c) { 
                // Would need a RestoreMana method in CharacterBase
            },
            [](CharacterBase* c) {}
        );
    }
    
    // Qi Booster - temporary special move damage boost
    if (consumable.id == 32) {
        return std::make_unique<ConsumableEffect>(
            30.0f, // 30 second duration
            [this, boost = consumable.specialMoveDamageBonus](CharacterBase* c) {
                int charId = c->GetId();
                m_characterData[charId].specialMoveDamageBonus += boost;
            },
            [this, boost = consumable.specialMoveDamageBonus](CharacterBase* c) {
                int charId = c->GetId();
                m_characterData[charId].specialMoveDamageBonus -= boost;
            }
        );
    }
    
    // Gear Cooldown Elixir - temporary cooldown reduction
    if (consumable.id == 33) {
        return std::make_unique<ConsumableEffect>(
            60.0f, // 60 second duration
            [this, reduction = consumable.gearCooldownReduction](CharacterBase* c) {
                int charId = c->GetId();
                m_characterData[charId].gearCooldownReduction += reduction;
            },
            [this, reduction = consumable.gearCooldownReduction](CharacterBase* c) {
                int charId = c->GetId();
                m_characterData[charId].gearCooldownReduction -= reduction;
            }
        );
    }
    
    return nullptr;
}

void ItemManager::UpdateConsumableEffects(CharacterBase* character, float deltaTime) {
    if (!character) return;
    
    int charId = character->GetId();
    auto it = m_characterData.find(charId);
    if (it == m_characterData.end()) return;
    
    CharacterItemData& data = it->second;
    
    // Update and remove expired effects
    data.activeEffects.erase(
        std::remove_if(data.activeEffects.begin(), data.activeEffects.end(),
            [character, deltaTime](std::unique_ptr<ConsumableEffect>& effect) {
                if (effect->Update(deltaTime)) {
                    effect->Remove(character);
                    return true;
                }
                return false;
            }),
        data.activeEffects.end()
    );
}

// Getter implementations

float ItemManager::GetTotalMaxHealth(CharacterBase* character) const {
    if (!character) return 0;
    
    auto it = m_characterData.find(character->GetId());
    if (it == m_characterData.end()) return character->GetMaxHealth();
    
    return it->second.baseMaxHealth + it->second.itemHealthBonus;
}

float ItemManager::GetTotalMaxMana(CharacterBase* character) const {
    if (!character) return 0;
    
    auto it = m_characterData.find(character->GetId());
    if (it == m_characterData.end()) return character->GetMaxMana();
    
    return it->second.baseMaxMana + it->second.itemManaBonus;
}

float ItemManager::GetTotalDefense(CharacterBase* character) const {
    if (!character) return 0;
    
    auto it = m_characterData.find(character->GetId());
    if (it == m_characterData.end()) return character->GetDefense();
    
    return it->second.baseDefense + it->second.itemDefenseBonus;
}

float ItemManager::GetTotalSpeed(CharacterBase* character) const {
    if (!character) return 0;
    
    auto it = m_characterData.find(character->GetId());
    if (it == m_characterData.end()) return character->GetSpeed();
    
    return it->second.baseSpeed + it->second.itemSpeedBonus;
}

float ItemManager::GetTotalCriticalChance(CharacterBase* character) const {
    if (!character) return 0;
    
    auto it = m_characterData.find(character->GetId());
    if (it == m_characterData.end()) return character->GetCriticalChance();
    
    return std::min(1.0f, it->second.baseCriticalChance + it->second.itemCriticalChanceBonus);
}

float ItemManager::GetTotalPowerModifier(CharacterBase* character) const {
    if (!character) return 0;
    
    auto it = m_characterData.find(character->GetId());
    if (it == m_characterData.end()) return character->GetPowerModifier();
    
    return it->second.basePowerModifier + it->second.itemPowerModifierBonus;
}

float ItemManager::GetTotalManaRegen(CharacterBase* character) const {
    if (!character) return 5.0f; // Base regen
    
    auto it = m_characterData.find(character->GetId());
    if (it == m_characterData.end()) return 5.0f;
    
    return it->second.baseManaRegen + it->second.itemManaRegenBonus;
}

float ItemManager::GetGearCooldownReduction(CharacterBase* character) const {
    if (!character) return 0;
    
    auto it = m_characterData.find(character->GetId());
    if (it == m_characterData.end()) return 0;
    
    return std::min(0.75f, it->second.gearCooldownReduction); // Cap at 75% reduction
}

float ItemManager::GetSpecialMoveDamageBonus(CharacterBase* character) const {
    if (!character) return 0;
    
    auto it = m_characterData.find(character->GetId());
    if (it == m_characterData.end()) return 0;
    
    return it->second.specialMoveDamageBonus;
}

float ItemManager::GetGearSkillDamageBonus(CharacterBase* character) const {
    if (!character) return 0;
    
    auto it = m_characterData.find(character->GetId());
    if (it == m_characterData.end()) return 0;
    
    return it->second.gearSkillDamageBonus;
}

float ItemManager::GetBlockDamageReduction(CharacterBase* character) const {
    if (!character) return 0;
    
    auto it = m_characterData.find(character->GetId());
    if (it == m_characterData.end()) return 0;
    
    return it->second.blockDamageReduction;
}

float ItemManager::CalculateGearSkillCooldown(CharacterBase* character, int skillIndex) const {
    if (!character || skillIndex < 0 || skillIndex >= 8) return 0;
    
    const auto& skill = character->GetGearSkills()[skillIndex];
    float baseCooldown = skill.cooldown;
    float reduction = GetGearCooldownReduction(character);
    
    return baseCooldown * (1.0f - reduction);
}

float ItemManager::CalculateSpecialMoveDamage(CharacterBase* character, float baseDamage) const {
    if (!character) return baseDamage;
    
    float powerMod = GetTotalPowerModifier(character);
    float specialBonus = GetSpecialMoveDamageBonus(character);
    
    // Apply bonuses multiplicatively
    return baseDamage * powerMod * (1.0f + specialBonus / 100.0f);
}

float ItemManager::CalculateGearSkillDamage(CharacterBase* character, float baseDamage,
                                           int gearSlot) const {
    if (!character) return baseDamage;
    
    float powerMod = GetTotalPowerModifier(character);
    float gearBonus = GetGearSkillDamageBonus(character);
    
    // Check for gear-specific enhancements
    auto it = m_characterData.find(character->GetId());
    if (it != m_characterData.end()) {
        // Additional logic for gear-specific bonuses could go here
    }
    
    return baseDamage * powerMod * (1.0f + gearBonus / 100.0f);
}

} // namespace ArenaFighter