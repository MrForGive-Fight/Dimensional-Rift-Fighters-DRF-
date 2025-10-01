# Game Application Integration Guide

## Overview

The provided GameApplication.h combines multiple systems that use incorrect APIs (Character.h with cooldowns on special skills, etc.). This document explains how DFRGameApplication properly integrates all systems while maintaining DFR's core design principles.

## Key Integration Points

### 1. Input System Correction

The provided input system doesn't distinguish between special moves and gear skills. DFRInputSystem fixes this:

**Special Moves (S+Direction)**:
- Triggered by holding S + pressing a direction
- Can't be used while blocking (S held for 1+ seconds)
- Use MANA ONLY - no cooldowns

**Gear Skills (AS, AD, SD, ASD)**:
- Triggered by specific key combinations
- Have both mana costs AND cooldowns
- Can be used while blocking

### 2. Skill Execution Flow

```cpp
// Special Move (mana only)
if (inputSystem->IsSpecialMoveTriggered(direction)) {
    if (character->CanExecuteSpecialMove(direction)) {  // Checks mana only
        character->ExecuteSpecialMove(direction);
        // No cooldown started!
    }
}

// Gear Skill (mana + cooldown)
if (inputSystem->IsGearSkillTriggered(skillIndex)) {
    if (!character->IsGearSkillOnCooldown(skillIndex)) {  // Check cooldown
        if (character->CanAffordSkill(manaCost)) {        // Check mana
            character->StartGearSkillCooldown(skillIndex); // Start cooldown
        }
    }
}
```

### 3. System Architecture

```
GameApplication.h (provided)
         |
         v
DFRGameApplication
    |         |         |         |
    v         v         v         v
DFRInput  Rendering  ShopSystem  GameModes
System    Adapter    Adapter     (DFR native)
```

### 4. DirectX Integration

The adapter uses ComPtr throughout for safe resource management:

```cpp
ComPtr<ID3D11Device> m_device;
ComPtr<ID3D11DeviceContext> m_deviceContext;
ComPtr<IDXGISwapChain> m_swapChain;
```

## Input Handling Details

### Key Combinations

| Input | Action | Type | Resource |
|-------|--------|------|----------|
| S + ↑ | Special Move Up | Special | Mana only |
| S + ↓ | Special Move Down | Special | Mana only |
| S + ← | Special Move Left | Special | Mana only |
| S + → | Special Move Right | Special | Mana only |
| A→S | Gear Skill 0 | Gear | Mana + CD |
| A→D | Gear Skill 2 | Gear | Mana + CD |
| S→D | Gear Skill 4 | Gear | Mana + CD |
| A→S→D | Gear Skill 6 | Gear | Mana + CD |
| Hold S (1s) | Block | Defense | None |

### Blocking Mechanics

1. Hold S for 1 second to activate block
2. While blocking:
   - Can't use special moves
   - CAN use gear skills
   - Can't move
   - Take 50% reduced damage (+ item bonuses)

## Shop Integration

The shop system's cooldown reduction is properly mapped:

```cpp
void OnItemEquipped(const DFRShopItem& item) {
    // Apply to character
    ItemManager::GetInstance().ApplyEquippedItems(character, equipped);
    
    // item.gearCooldownReduction ONLY affects gear skills
    // Special moves remain mana-limited
}
```

## Rendering Integration

Visual effects distinguish between skill types:

```cpp
// Special move effect (directional particles)
renderer->CreateSpecialMoveEffect(character, direction, position);

// Gear skill effect (spiral particles)
renderer->CreateGearSkillEffect(character, skillIndex, position);
```

## Game Flow

### Match Start
1. Select game mode
2. Choose characters
3. Apply equipped items
4. Initialize match

### During Combat
1. Input system tracks key combinations
2. Special moves checked first (mana only)
3. Gear skills checked second (cooldown + mana)
4. Movement and blocking processed
5. Effects and damage calculated with item bonuses

### Match End
1. Calculate results
2. Update rankings
3. Award currency
4. Return to menu

## Common Integration Issues

### Issue: Special moves have cooldowns
**Solution**: DFRInputSystem and CharacterBase ensure special moves never have cooldowns

### Issue: Shop items affect all cooldowns
**Solution**: ItemManager applies cooldown reduction ONLY to gear skills

### Issue: Can't tell skill types apart
**Solution**: Different visual effects and UI indicators for each type

## Performance Optimizations

1. **Batched Rendering**: Characters rendered in groups
2. **Particle Pooling**: Reuse particle objects
3. **Input Buffering**: 300ms window for combos
4. **State Caching**: Avoid redundant D3D state changes

## Testing Integration

```cpp
// Test special move frequency
character->ConsumeMana(90); // Leave 10 mana
// Can use 10-mana special move
// Can't use 20-mana special move

// Test gear skill cooldown
character->StartGearSkillCooldown(0);
// Can't use skill 0
// CAN use skills 1-7
// CAN use all special moves
```

## Future Enhancements

1. **Network Integration**: Add rollback netcode
2. **Replay System**: Record inputs with proper skill distinction
3. **Training Mode**: Show mana costs vs cooldowns clearly
4. **Spectator Mode**: Highlight skill types differently

## Key Takeaways

✅ Special moves are NEVER limited by cooldowns
✅ Gear skills have BOTH resources
✅ Shop items respect this distinction
✅ Visual feedback shows skill type
✅ Input system enforces proper combos