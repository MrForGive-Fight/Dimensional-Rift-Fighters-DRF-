# /implement-combat

Implement combat mechanics following LSFDC patterns.

## Usage
```
/implement-combat <feature> [options]
```

## Features

### combo-system
Implement combo mechanics with LSFDC timing windows.
```
/implement-combat combo-system --max-hits=10 --scaling=0.9
```

### damage-calculation
Implement damage formulas from LSFDC reference.
```
/implement-combat damage-calculation --include-elements --include-criticals
```

### frame-data
Set up frame data system for moves.
```
/implement-combat frame-data --character=<Name>
```

### hit-detection
Implement collision and hit detection.
```
/implement-combat hit-detection --use-lsfdc-physics
```

## LSFDC Combat Formulas

### Base Damage Calculation
```cpp
float CalculateDamage(const AttackData& attack, const DefenseData& defense) {
    float baseDamage = attack.baseDamage * attack.powerModifier;
    float defense = defense.defensePower * defense.defenseModifier;
    
    // LSFDC formula
    float damage = baseDamage * (100.0f / (100.0f + defense));
    
    // Apply element multiplier
    damage *= GetElementMultiplier(attack.element, defense.element);
    
    // Apply combo scaling
    damage *= pow(COMBO_SCALING, comboCount);
    
    return damage;
}
```

### Frame Data Standards
- Light attacks: 5-8 startup frames
- Medium attacks: 10-15 startup frames
- Heavy attacks: 18-25 startup frames
- Special moves: Variable based on mana cost

### Hit Properties
- Normal: Standard hitstun
- Counter: 1.5x damage, extended hitstun
- Crush: Breaks guard
- Launch: Starts air combo

## Implementation Steps

1. **Parse LSFDC Reference**
   - Extract combat formulas from `reference/LSFDC/combat/`
   - Identify key mechanics to implement

2. **Create Combat Components**
   - AttackData structure
   - DefenseData structure
   - HitResult enumeration
   - ComboTracker class

3. **Implement Hit Detection**
   - Use LSFDC collision boxes
   - Frame-perfect detection
   - Proper priority system

4. **Add Visual Feedback**
   - Hit sparks
   - Damage numbers
   - Combo counter

5. **Test and Balance**
   - Verify frame data accuracy
   - Test combo routes
   - Ensure mana costs are balanced