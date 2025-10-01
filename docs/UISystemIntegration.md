# UI System Integration Guide

## Overview

The provided UISystem.h contains stat mode descriptions that incorrectly suggest cooldowns affect all skills. The UISystemAdapter fixes these descriptions and ensures the UI clearly communicates DFR's skill system distinction.

## Key UI Corrections

### 1. Stat Mode Descriptions

**Original (Incorrect)**:
```
"SPECIAL MODE
+50% Skill Power
20% Faster Cooldowns"
```

**DFR Corrected**:
```
"SPECIAL MODE
+50% Skill Power
+10% Mana Regen
20% Faster GEAR Cooldowns"
```

The correction emphasizes that cooldown reduction ONLY affects gear skills (AS, AD, SD, ASD), never special moves.

### 2. Skill System Tooltip

Added clear explanation in the UI:
```
SKILL SYSTEM:
• Special Moves (S+↑↓←→): Mana cost only, NO cooldowns
• Gear Skills (AS/AD/SD/ASD): Mana cost AND cooldowns
• Blocking (Hold S 1s): Prevents special moves
```

## UI Components

### Main Menu
- Standard game menu options
- Training mode prominently displayed for skill practice

### Mode Selection
- Weekly rotation clearly shown for Ranked mode
- Tooltips explain each mode's unique features

### Character Selection
- Categories match DFR's 7-category system
- Character stats shown (HP, Mana, Tier)
- Visual indication of selected characters

### Loadout Setup (New Panel)
Critical panel that shows:
1. **Stat Mode Selection** with corrected descriptions
2. **Gear Preview** showing both mana costs AND cooldowns
3. **Skill System Info** panel explaining the distinction

### Combat HUD

The combat HUD clearly differentiates skill types:

```
[Special Moves]          [Gear Skills]
  S+↑ (Ready)             AS (5s CD)
S+← S+→ (Ready)           AD (Ready)
  S+↓ (Ready)             SD (3s CD)
                          ASD (Ready)

[Mana Bar: ████████░░]   [Block: ░░░░░░░░░░]
```

## Visual Indicators

### Color Coding
- **Green**: Skill available (has mana/no cooldown)
- **Yellow**: Has cooldown but affordable
- **Red**: Can't afford mana cost
- **Gray**: On cooldown (gear skills only)

### Skill Availability Logic

**Special Moves**:
```cpp
bool available = character->CanAffordSkill(manaCost) && !character->IsBlocking();
// NO cooldown check!
```

**Gear Skills**:
```cpp
bool available = character->CanAffordSkill(manaCost) && 
                !character->IsGearSkillOnCooldown(index);
// Both checks required!
```

## Button Layouts

### Special Move Indicators
```
     [S+↑]
[S+←]     [S+→]
     [S+↓]
```
Positioned to match directional input.

### Gear Skill Indicators
```
[AS]  [AD]  [SD]  [ASD]
```
Linear layout with cooldown timers shown.

## Implementation Details

### Character Display Structure
```cpp
struct CharacterDisplay {
    std::string name;
    float healthPercent;
    float manaPercent;
    int currentStance;
    std::vector<float> gearCooldowns; // Track all 8 skills
    bool isBlocking;
    float blockHoldTime;
};
```

### Updating Skill Status
```cpp
// Update special moves (mana check only)
for (int i = 0; i < 4; ++i) {
    InputDirection dir = static_cast<InputDirection>(i);
    bool available = player->CanExecuteSpecialMove(dir);
    hud->ShowSpecialMoveAvailable(dir, available);
}

// Update gear skills (mana + cooldown)
for (int i = 0; i < 8; ++i) {
    float cd = player->GetGearSkillCooldownRemaining(i);
    bool canAfford = player->CanAffordSkill(gearSkills[i].manaCost);
    hud->ShowGearSkillStatus(i, cd, canAfford);
}
```

## Stance Display

For characters with stance systems (like Murim):
```
Current Stance: [Light ☀] [Dark ☾]
                  ^^^^^ Highlighted
```

## Block Indicator

Shows block charging progress:
```
Block: [████████░░] 0.8s/1.0s
```

When fully charged:
```
Block: [ACTIVE] (Special moves disabled)
```

## Mana Display

Emphasize mana's importance:
```
Mana: [████████░░] 80/100 (+5/s)
                          ^^^^^^ Show regen rate
```

With items:
```
Mana: [████████░░] 80/100 (+15/s)
                          ^^^^^^^ Enhanced regen
```

## Post-Match Display

Show skill usage statistics:
```
Match Statistics:
- Special Moves Used: 45 (Mana limited)
- Gear Skills Used: 12 (Cooldown limited)
- Average Mana: 65%
- Time Blocked: 15s
```

## Best Practices

1. **Always show both resources** for gear skills
2. **Never show cooldowns** for special moves
3. **Use consistent color coding** across all UI
4. **Provide tooltips** explaining the system
5. **Update in real-time** during combat

## Common UI Mistakes to Avoid

❌ Showing cooldown timers on special moves
❌ Using same indicators for both skill types
❌ Hiding mana costs for gear skills
❌ Not showing mana regeneration rate

✅ Clear visual distinction between skill types
✅ Both resources shown for gear skills
✅ Mana-only indication for special moves
✅ Real-time updates of all resources

## Accessibility

- Colorblind modes with shape differentiation
- Text labels in addition to colors
- Scalable UI for different resolutions
- Clear audio cues for skill availability