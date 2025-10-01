# DFR Skill System Guide

## CRITICAL DISTINCTION: Two Types of Skills

### 1. Special Moves (S+Direction) - MANA ONLY
- **Input**: S + ↑/↓/←/→
- **Resource**: MANA ONLY (NO COOLDOWNS)
- **Purpose**: Core combat moves, frequently used
- **Balance**: Limited by mana regeneration (5/second)

### 2. Gear Skills (AS, AD, ASD, SD) - MANA + COOLDOWNS
- **Input**: AS, AD, ASD, SD (based on gear slot)
- **Resource**: BOTH Mana AND Cooldowns
- **Purpose**: Powerful abilities with tactical timing
- **Balance**: Limited by both resources

## Implementation Examples

### Special Move (CORRECT - No Cooldown)
```cpp
SpecialMove spearSeaImpact = {
    "Spear Sea Impact",         // name
    "SpearSeaAnimation",        // animation
    25.0f,                      // manaCost (ONLY MANA)
    150.0f,                     // baseDamage
    AttackType::Special,        // attackType
    15, 60, 20,                // frame data
    false,                      // isProjectile
    true,                       // canCombo
    true,                       // blockable
    ElementType::Water          // element
    // NO COOLDOWN FIELD!
};
```

### Gear Skill (CORRECT - Has Cooldown)
```cpp
GearSkill glassyDeathRain = {
    "Glassy Death Rain",        // name
    "GlassyRainAnimation",      // animation
    30.0f,                      // manaCost
    200.0f,                     // baseDamage
    10.0f,                      // COOLDOWN (seconds)
    AttackType::Heavy,          // attackType
    25, 180, 30,               // frame data
    true,                       // isProjectile
    false,                      // canCombo
    ElementType::Light          // element
};
```

## Blocking Interaction

### Blocking Rules:
1. Hold S for 1 second to activate block
2. While blocking:
   - Cannot use special moves (S+Direction)
   - CAN still use gear skills (AS, AD, ASD, SD)
   - Take 50% reduced damage
   - Chip damage still applies

## Resource Management Strategy

### Mana Management:
- Base: 100 mana
- Regen: 5/second
- Special moves: 15-40 mana typically
- Gear skills: 20-50 mana typically

### Cooldown Management:
- Gear skills only
- Range: 5-20 seconds typically
- Special mode reduces cooldowns by 20%
- No items or abilities affect special move availability

## Common Mistakes to Avoid

### ❌ WRONG: Special Move with Cooldown
```cpp
// DON'T DO THIS
SpecialMove wrongMove = {
    "Some Move",
    "Animation",
    20.0f,      // mana
    10.0f,      // cooldown <- WRONG! Special moves don't have cooldowns!
    // ...
};
```

### ✅ CORRECT: Special Move (Mana Only)
```cpp
SpecialMove correctMove = {
    "Some Move",
    "Animation", 
    20.0f,      // mana (ONLY resource cost)
    100.0f,     // damage
    // ... no cooldown field
};
```

## Character Implementation Checklist

When creating a character:

1. **Special Moves (S+Direction)**:
   - [ ] Register 4 directional moves minimum
   - [ ] Set appropriate mana costs (15-40 range)
   - [ ] NO cooldown values
   - [ ] Can be blocked by default

2. **Gear Skills (AS, AD, ASD, SD)**:
   - [ ] Create 8 skills (4 gears × 2 skills)
   - [ ] Set both mana AND cooldown
   - [ ] Cooldowns typically 5-20 seconds
   - [ ] More powerful than special moves

3. **Blocking**:
   - [ ] Override OnBlockStart() if needed
   - [ ] Special moves disabled during block
   - [ ] Gear skills remain available

## Balance Guidelines

### Special Moves:
- Light damage (50-100): 15-20 mana
- Medium damage (100-150): 20-30 mana
- Heavy damage (150-200): 30-40 mana
- Ultimate moves: 50-70 mana

### Gear Skills:
- Quick utility: 5-8s cooldown, 20-30 mana
- Standard damage: 10-15s cooldown, 30-40 mana
- Powerful effects: 15-20s cooldown, 40-50 mana
- Ultimate tier: 20-30s cooldown, 50-60 mana

## Testing Your Implementation

1. Verify special moves can be used repeatedly (mana permitting)
2. Verify gear skills go on cooldown after use
3. Test blocking prevents special moves
4. Ensure mana regenerates at 5/second
5. Confirm gear switching has no cost or cooldown