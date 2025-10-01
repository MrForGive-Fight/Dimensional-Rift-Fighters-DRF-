# /balance-check

Analyze and balance game mechanics using LSFDC standards.

## Usage
```
/balance-check <target> [--fix] [--report]
```

## Targets

### characters
Check character balance across all categories.
```
/balance-check characters --report
```

### skills
Analyze skill mana costs and damage ratios.
```
/balance-check skills --fix
```

### frame-data
Verify frame data consistency.
```
/balance-check frame-data
```

### damage-output
Test damage output across all characters.
```
/balance-check damage-output --scenarios=100
```

## Balance Standards

### Mana Economy
- Basic attacks: 5-15 mana
- Special moves: 20-40 mana
- Ultimate skills: 50-70 mana
- Mana regeneration: 5/second (base)

### Damage Ratios
- Light attacks: 50-80 damage
- Medium attacks: 100-150 damage
- Heavy attacks: 200-300 damage
- Skill multiplier: 1.5x-3x base

### Frame Data Balance
```
Total frames = Startup + Active + Recovery

Light:  5-8 + 2-3 + 8-12 = 15-23 frames
Medium: 10-15 + 3-5 + 15-20 = 28-40 frames  
Heavy:  18-25 + 5-8 + 25-35 = 48-68 frames
```

### Character Category Balance
Each category should have:
- Unique playstyle advantage
- Clear weaknesses
- Viable in all game modes
- Balanced risk/reward

## Automated Checks

### 1. Mana Efficiency
```cpp
float efficiency = damage / manaCost;
// Target: 5-10 damage per mana point
```

### 2. Frame Advantage
```cpp
int advantage = opponentHitstun - (active + recovery);
// Light: -2 to +2
// Medium: 0 to +5
// Heavy: -5 to +10
```

### 3. Combo Potential
- Max combo length: 15 hits
- Max combo damage: 60% health
- Mana requirement: 80% of max mana

### 4. Matchup Spread
- No 7-3 or worse matchups
- At least 40% favorable matchups
- Maximum 20% hard counters

## Fix Mode

When using `--fix` flag:
1. Automatically adjust values within 10% tolerance
2. Log all changes made
3. Create backup of original values
4. Generate balance patch notes

## Report Format

```
=== Balance Report ===
Date: [timestamp]
Version: [version]

Character Balance:
- [Character]: [Issues/Pass]

Skill Balance:
- Total skills analyzed: [count]
- Overtuned: [list]
- Undertuned: [list]

Frame Data:
- Inconsistencies: [count]
- Details: [list]

Recommendations:
- [Priority 1 fixes]
- [Priority 2 adjustments]
- [Priority 3 considerations]
```