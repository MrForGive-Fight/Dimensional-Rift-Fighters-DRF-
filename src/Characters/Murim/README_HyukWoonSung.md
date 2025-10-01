# Hyuk Woon Sung Character Implementation

## Overview
S-Tier Unique martial artist character with a dual stance system, part of the Murim category.

## Character Features

### Stance System
- **Light Stance (Orthodox Spear)**: Blue-themed, defensive, faster attacks
  - +15% Speed, +10% Defense
  - Focus on precision and combos
  
- **Dark Stance (Heavenly Demon)**: Red-themed, aggressive, higher damage
  - +20% Damage, -10% Defense, -5% Speed
  - Focus on powerful strikes

### Special Moves System (S+Direction Inputs - MANA ONLY, NO COOLDOWNS)

#### Light Stance Special Moves
- **S+↑: Spear Sea Impact** (25 mana) - Multi-hit spear thrust with water waves
  - Damage: 150, Frames: 15/8/20, Projectile
- **S+→: Divine Wind of the Past** (20 mana) - Fast movement skill with wind trails
  - Damage: 80, Frames: 8/5/12, Grants 1.5x speed boost
- **S+←: Lightning Stitching Art** (30 mana) - Rapid multi-hit combo with lightning
  - Damage: 120, Frames: 12/10/18, High gauge build

#### Dark Stance Special Moves  
- **S+↑: Heavenly Demon Divine Power** (25 mana) - Powerful palm strike with dark energy
  - Damage: 180, Frames: 18/6/22
- **S+→: Black Night of Fourth Moon** (30 mana) - Dark wave projectile attack
  - Damage: 160, Frames: 16/8/20, Projectile, Cannot combo
- **S+←: Mind Split Double Will** (35 mana) - Unblockable finisher
  - Damage: 200, Frames: 20/12/25, Unblockable, Cannot combo

Note: Special moves automatically update when switching stances!

### Ultimate Ability
- **Divine Arts of the Unrecorded** (70 mana + full gauge)
- 10 second duration with enhanced stats
- Requires full Tempered True Blossom gauge (100 points)

### Gear Skills (WITH COOLDOWNS)
Each stance has 4 unique gear sets with 2 skills each (8 total per stance):
- Light Stance: Orthodox Spear Arts, Divine Techniques, Defensive Forms, Ultimate Techniques
- Dark Stance: Heavenly Demon Arts, Destruction Techniques, Aggressive Forms, Demon Lord Techniques

Note: Gear skills use BOTH mana costs AND cooldowns, unlike special moves which only use mana!

## Implementation Details

### Files
- `HyukWoonSung.h/cpp` - Main character class
- `StanceSystem.h/cpp` - Stance switching mechanics
- Integrated with `CharacterFactory` (ID: 300)

### Key Mechanics
- Tempered True Blossom gauge builds through combat
- Stance switching has 0.5s cooldown with yin-yang shatter effect
- S-Tier stats: +10% health, +10% defense, +15% speed, +15% damage

### Visual Effects
- Blue aura and effects in Light Stance
- Red aura and effects in Dark Stance
- Yin-yang shatter effect on stance switch
- Unique ultimate effect combining both stances