# Rou Implementation Summary - DFR Game

## Overview
Rou is a System category character with an evolution-based gameplay mechanic. The character has 5 distinct evolution forms that progress based on an Evolution Gauge (0-100%).

## Character Design

### Evolution Forms
1. **Goblin** (0-24% gauge)
   - High speed (1.3x), low damage (0.85x)
   - Size: 0.8x
   - HP: 180
   - 3-hit combo (21-26 damage)

2. **Hobgoblin** (25-49% gauge)
   - Balanced stats
   - Size: 1.0x
   - HP: 200
   - 4-hit combo (35-40 damage)

3. **Ogre** (50-74% gauge)
   - High damage (1.25x), low defense (0.85x)
   - Size: 2.5x
   - HP: 220
   - 5-hit combo (62-70 damage)

4. **Apostle Lord** (75-99% gauge)
   - Very high damage (1.4x), medium defense (0.7x)
   - Size: 2.0x
   - HP: 210
   - 6-hit combo (83-95 damage)

5. **Vajrayaksa** (100% gauge)
   - Highest damage (1.6x), lowest defense (0.5x)
   - Size: 2.5x
   - HP: 200
   - 8-hit combo (130-150 damage)

### Core Mechanics

#### Evolution Gauge Building
- **On Hit**: +1.5% per hit landed
- **On Take Damage**: +0.5% per 10 HP lost
- **On Kill**: +15% instant
- **On Equipment Pickup**: +7% per piece
- **On Death**: -35% of current gauge

#### Emergency Protocol
- Triggers automatically at 30% HP (once per life)
- Auto-evolves to next form
- Heals 15% of max HP
- Grants 30% damage reduction for 3 seconds
- Vajrayaksa special: Full heal + gauge locked at 100%

### Special Moves (S+Direction)

#### Goblin Form
- **S+Up - Panic Jump**: Quick vertical escape with i-frames (3s cd)
- **S+Left - Survival Bite**: Counter stance → bite heal 30 HP (5s cd)
- **S+Right - Goblin Rush**: Fast roll, steals 5% meter (4s cd)

#### Hobgoblin Form
- **S+Up - Shadow Upper**: Rising uppercut → air combo (4s cd)
- **S+Left - Dark Counter**: Counter → shadow explosion (6s cd)
- **S+Right - Phantom Strike**: Teleport backstab (7s cd)

#### Ogre Form
- **S+Up - Ogre Slam**: Jump slam with guard break (7s cd)
- **S+Left - Ground Quake**: Earth spikes forward (8s cd)
- **S+Right - Brutal Charge**: Armored rush grab (9s cd)

#### Apostle Lord Form
- **S+Up - Demon Ascension**: Fly → rain 5 orbs (9s cd)
- **S+Left - Lord's Territory**: Buff field for allies (11s cd)
- **S+Right - Orb Barrage**: 3 homing orbs (8s cd)

#### Vajrayaksa Form
- **S+Up - Heaven Splitter**: 4-arm energy pillar (12s cd)
- **S+Left - Overlord's Decree**: AOE fear + reset cooldowns (15s cd)
- **S+Right - Thousand Arms Rush**: Teleport → 20-hit barrage (13s cd)

## File Structure

```
DFR/
├── src/Heroes/Rou/
│   ├── Rou.h                           # Main character class
│   ├── Rou.cpp                         # Character implementation
│   ├── States/
│   │   ├── EvolutionStateMachine.h     # State management
│   │   ├── EvolutionStateMachine.cpp   # State implementation
│   │   └── FormStates.h                # Form-specific states
│   ├── Visuals/
│   │   ├── EvolutionVFX.h              # Visual effects header
│   │   └── EvolutionVFX.cpp            # VFX implementation
│   └── Tests/
│       └── RouTests.cpp                # Unit tests
├── game-project/src/
│   └── test_rou_main.cpp               # Test program
└── DFR/
    ├── DFR.vcxproj                     # Visual Studio project
    └── DFR.vcxproj.filters             # VS folder structure
```

## Implementation Details

### Key Classes

1. **Rou** (Rou.h/cpp)
   - Main character class inheriting from CharacterBase
   - Manages evolution state, gauge, and HP
   - Handles all combat mechanics and special moves
   - Implements gauge building and emergency protocol

2. **EvolutionStateMachine** (States/)
   - Manages form transitions
   - Tracks evolution requirements
   - Applies form-specific stat modifiers
   - Handles state enter/exit logic

3. **EvolutionVFX** (Visuals/)
   - Particle system for evolution effects
   - Form-specific auras and colors
   - Special move visual effects
   - Gauge threshold effects

### Technical Specifications

- **Language**: C++20
- **Namespace**: ArenaFighter
- **Base Stats**: 1000 HP, 100 Mana
- **Mana Regen**: 5/second
- **Frame Data**: Scales from 3-5f (Goblin) to 10-15f (Vajrayaksa)
- **Max Combo**: 15 hits with 0.9x scaling per hit

## Testing

### Unit Tests (RouTests.cpp)
- Evolution threshold verification
- Emergency Protocol triggering
- Gauge building mechanics
- Death penalty calculation
- Special move execution
- Combo damage verification
- Frame data validation
- Cooldown management

### Test Program (test_rou_main.cpp)
Interactive test demonstrating:
- Evolution progression
- All gauge building methods
- Emergency Protocol activation
- Special move execution
- Visual effects system
- Combo system for each form

## Visual Studio Integration

### Project Configuration
- C++20 standard enabled
- Include directories properly set
- All source files added to project
- Proper folder organization in Solution Explorer
- Test files excluded from Release builds

### Building Instructions
1. Open `DFR.sln` in Visual Studio 2022
2. Select Debug configuration and x64 platform
3. Build solution (Ctrl+Shift+B)
4. Run with F5 (debug) or Ctrl+F5 (no debug)

## Notes for Future Development

### Balancing Considerations
- Vajrayaksa's low defense (0.5x) makes it high-risk/high-reward
- Emergency Protocol provides comeback potential
- Death penalty prevents gauge camping
- Cooldowns prevent special move spam

### Integration Points
- CharacterFactory needs CreateRou() implementation
- Combat system integration for damage calculation
- Physics system for movement and collision
- Network sync for evolution state
- UI for gauge display

### Potential Enhancements
- Form-specific voice lines
- Unique death animations per form
- Evolution cutscenes
- Gauge steal mechanics in PvP
- Form-locked game modes

## Save Date
Created: December 2024
Version: 1.0
Framework: LSFDC (Lost Saga Fighting Development Core)