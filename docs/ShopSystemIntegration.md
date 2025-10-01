# Shop System Integration Guide

## Overview

The provided ShopSystem.h includes a `cooldownReduction` stat that conflicts with DFR's design where special moves (S+Direction) have NO cooldowns. This document explains how the ShopSystemAdapter properly integrates the shop system.

## Key Design Principle

In DFR:
- **Special Moves (S+Direction)**: Use MANA ONLY - no cooldowns ever
- **Gear Skills (AS, AD, ASD, SD)**: Use BOTH mana AND cooldowns

Therefore, `cooldownReduction` from items ONLY affects gear skills, never special moves.

## Architecture

```
ShopSystem.h (provided)
         |
         v
ShopSystemAdapter
    |         |
    v         v
DFRShopSystem  ItemManager
    |              |
    v              v
DFRShopItem    CharacterBase
```

## Shop Item Categories

### 1. Weapons
- Provide attack bonuses and special effects
- Can boost special move damage (separate from cooldowns)
- May increase mana regeneration for more frequent special moves

### 2. Armor
- Defense and health bonuses
- Can provide gear cooldown reduction
- May enhance blocking effectiveness

### 3. Accessories
- Utility bonuses like speed and critical chance
- Mana regeneration boosts
- Special move damage amplification

### 4. Consumables
- Temporary effects with duration
- Can boost special move damage temporarily
- Can reduce gear cooldowns temporarily

### 5. Cosmetics
- Visual effects only
- No gameplay impact

### 6. Gear Enhancements (DFR Exclusive)
- Target specific gear slots (0-3)
- Reduce cooldowns for specific gear skills
- Increase damage for gear skills

## Item Stat Mapping

| Provided Stat | DFR Implementation |
|--------------|-------------------|
| healthBonus | Direct health increase |
| manaBonus | Direct mana increase |
| attackBonus | Maps to powerModifier |
| defenseBonus | Direct defense increase |
| skillPowerBonus | Maps to powerModifier |
| speedBonus | Direct speed increase |
| critChanceBonus | Direct critical chance |
| critDamageBonus | Would enhance critical multiplier |
| **cooldownReduction** | **ONLY affects gear skills** |

## DFR-Specific Stats

### Mana Regeneration Bonus
```cpp
float manaRegenBonus; // Increases base 5/sec regen
```
Crucial for special move frequency since they have no cooldowns.

### Special Move Damage Bonus
```cpp
float specialMoveDamageBonus; // % increase to S+Direction moves
```
Directly enhances special moves without affecting cooldowns.

### Gear Cooldown Reduction
```cpp
float gearCooldownReduction; // ONLY for AS, AD, ASD, SD skills
```
This is what `cooldownReduction` maps to - never affects special moves.

### Block Damage Reduction
```cpp
float blockDamageReduction; // Extra % reduction when blocking
```
Stacks with base 50% block reduction.

## Usage Examples

### Creating Items
```cpp
DFRShopItem weapon = {
    1, "Heavenly Demon Spear", "S-tier Murim weapon",
    ItemCategory::Weapon, ItemRarity::Legendary, 2000, 10,
    0, 0, 100.0f, 10.0f, 0, 0.1f, 0.2f,     // Basic stats
    2.0f,    // +2 mana/sec for MORE special moves
    0,       // No gear cooldown reduction on this weapon
    20.0f,   // +20% special move damage
    0,       // No block bonus
    -1, 0,   // Not gear-specific
    false, false, 0
};
```

### Applying Items to Character
```cpp
// Initialize item manager for character
ItemManager::GetInstance().InitializeCharacter(character);

// Apply equipped items
std::vector<DFRShopItem> equipped = shop.GetEquippedItems();
ItemManager::GetInstance().ApplyEquippedItems(character, equipped);

// Calculate modified stats
float manaRegen = CharacterStatsExtended::GetModifiedManaRegen(character);
// Now 7/sec instead of 5/sec with +2 bonus

// Special moves still have NO cooldown but can be used more often!
```

### Gear Skill Cooldown Calculation
```cpp
// Get base cooldown from gear skill
float baseCooldown = character->GetGearSkills()[skillIndex].cooldown;

// Apply item reductions (ONLY to gear skills)
float actualCooldown = ItemManager::GetInstance().CalculateGearSkillCooldown(
    character, skillIndex);
// If base was 10s and you have 20% reduction: 8s
```

### Using Consumables
```cpp
// Qi Booster - temporary special move damage
DFRShopItem qiBooster = shop.GetConsumableById(32);
ItemManager::GetInstance().UseConsumable(character, qiBooster);
// +50% special move damage for 30 seconds

// Gear Cooldown Elixir - temporary gear skill CDR
DFRShopItem cdrElixir = shop.GetConsumableById(33);
ItemManager::GetInstance().UseConsumable(character, cdrElixir);
// 50% gear skill CDR for 60 seconds
```

## Example Items

### Mana Regen Focus (for Special Moves)
```cpp
"Mana Crystal Pendant"
- +100 Max Mana
- +10 Mana/second
- Enables 3x more frequent special move usage
```

### Gear Skill Focus
```cpp
"Master Gear Enhancement"
- 25% gear skill cooldown reduction
- +30% gear skill damage
- Does NOT affect special moves at all
```

### Balanced Item
```cpp
"Master's Amulet"
- +5 Mana/second (more special moves)
- 20% gear cooldown reduction
- +15% special move damage
- Benefits both skill types appropriately
```

## Common Misconceptions

❌ **WRONG**: "This item reduces all skill cooldowns"
✅ **RIGHT**: "This item reduces gear skill cooldowns only"

❌ **WRONG**: "Special moves are limited by cooldowns"
✅ **RIGHT**: "Special moves are limited by mana only"

❌ **WRONG**: "CDR items make special moves faster"
✅ **RIGHT**: "Mana regen items make special moves more frequent"

## Integration with Combat

When calculating damage:
```cpp
// Special move damage
float damage = move.baseDamage;
damage = ItemManager::GetInstance().CalculateSpecialMoveDamage(character, damage);
// Includes powerModifier and specialMoveDamageBonus

// Gear skill damage
float damage = skill.baseDamage;
damage = ItemManager::GetInstance().CalculateGearSkillDamage(character, damage, gearSlot);
// Includes powerModifier and gearSkillDamageBonus
```

## Best Practices

1. **Always clarify**: When showing cooldown reduction in UI, specify "Gear Skill Cooldown Reduction"
2. **Highlight mana regen**: Since it's crucial for special move frequency
3. **Separate bonuses**: Show special move and gear skill bonuses separately
4. **Balance accordingly**: High mana regen can be as powerful as cooldown reduction

## Future Considerations

1. Add visual indicators for active item effects
2. Create item sets with bonuses
3. Add character-specific items (e.g., Murim-only weapons)
4. Implement item upgrade/enhancement system
5. Add trading between players