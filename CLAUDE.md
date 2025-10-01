# DFR Game Development Master Rules

This file contains the authoritative rules and patterns for DFR (Anime Arena Fighter) development.

## Core Development Standards

### Language & Framework
- **Language**: C++20
- **Framework**: LSFDC (Lost Saga Fighting Development Core)
- **Build System**: CMake 3.20+
- **Target Platform**: Windows (DirectX 9/11)

### Code Organization Rules
1. **File Size Limit**: Maximum 500 lines per file
2. **Separation**: Always separate headers (.h) and implementations (.cpp)
3. **Namespace**: All code must be within `namespace ArenaFighter`
4. **Include Guards**: Use `#pragma once` for headers

### Naming Conventions
- **Classes**: PascalCase (e.g., `CharacterBase`, `CombatSystem`)
- **Methods**: camelCase (e.g., `calculateDamage`, `updatePosition`)
- **Constants**: UPPER_SNAKE_CASE (e.g., `MAX_HEALTH`, `BASE_MANA`)
- **Member Variables**: m_camelCase (e.g., `m_health`, `m_currentState`)
- **Files**: PascalCase matching class names

## Game Design Rules

### Mana System
- **Base Mana**: 100 points for all characters
- **Mana Regeneration**: 5 points per second
- **Two Skill Types**:
  - **Special Moves (S+Direction)**: MANA ONLY, NO COOLDOWNS
  - **Gear Skills (AS, AD, ASD, SD)**: BOTH Mana AND Cooldowns
- **Mana Costs**:
  - Basic attacks: 5-15 mana
  - Special moves: 15-40 mana (S+Direction, no cooldowns)
  - Gear skills: 20-50 mana (plus 5-20s cooldowns)
  - Ultimate skills: 50-70 mana

### Character System
- **7 Categories**: System, GodsHeroes, Murim, Cultivation, Animal, Monsters, Chaos
- **Base Stats**:
  - Health: 1000 HP
  - Mana: 100 MP
  - Defense: 100
  - Speed: 100

### Gear & Skill System
- **4 Gears per Character**
- **2 Skills per Gear**
- **Total**: 8 skills per character
- **Gear Switching**: Instant, costs no mana
- **Skill Properties**: Each skill must define:
  - Mana cost
  - Base damage
  - Range
  - Frame data (startup, active, recovery)

### Combat Mechanics

#### Damage Calculation (LSFDC Formula)
```cpp
damage = baseDamage * (100.0f / (100.0f + defense))
damage *= elementMultiplier
damage *= pow(0.9f, comboCount)  // Combo scaling
```

#### Frame Data Standards
- **Light Attacks**: 5-8 startup, 2-3 active, 8-12 recovery
- **Medium Attacks**: 10-15 startup, 3-5 active, 15-20 recovery
- **Heavy Attacks**: 18-25 startup, 5-8 active, 25-35 recovery

#### Combo System
- **Max Combo**: 15 hits
- **Max Damage**: 60% of opponent's health
- **Scaling**: 0.9x per hit
- **Hitstun Decay**: 0.95x per hit

## LSFDC Pattern Requirements

### Combat Patterns
- Use damage formulas from `examples/LSFDC/combat/`
- Implement frame-perfect hit detection
- Follow LSFDC hitstun calculations
- Use proper hit priority system

### Network Patterns
- Implement packet structure from `examples/LSFDC/network/`
- Use LSFDC lag compensation (7 frame rollback max)
- Follow tick rate of 60Hz, send rate of 30Hz

### Physics Patterns
- Use collision system from `examples/LSFDC/physics/`
- Implement LSFDC standard hitboxes/hurtboxes
- Follow pushbox conventions for character collision

## Directory Structure Requirements

```
DFR/
├── src/                    # Main source code
│   ├── Characters/         # Character implementations
│   │   ├── System/
│   │   ├── GodsHeroes/
│   │   ├── Murim/
│   │   ├── Cultivation/
│   │   ├── Animal/
│   │   ├── Monsters/
│   │   └── Chaos/
│   ├── Combat/            # Combat system
│   ├── Network/           # Networking
│   ├── Physics/           # Physics engine
│   └── GameModes/         # Game mode logic
├── PRPs/                  # Pattern Reference Points
├── examples/              # LSFDC reference implementations
└── tests/                 # Unit and integration tests
```

## Development Workflow

### Adding New Characters
1. Use `/generate-character` command
2. Follow character template in `PRPs/characters/`
3. Implement all 8 skills (4 gears × 2)
4. Add to CharacterFactory
5. Test with `/balance-check character`

### Implementing Features
1. Check LSFDC patterns first
2. Use appropriate PRP template
3. Follow file size limits
4. Separate concerns properly
5. Write tests for new features

### Balance Testing
1. Run `/balance-check` regularly
2. Verify mana economy
3. Test frame data accuracy
4. Check matchup spread
5. Document any deviations

## Performance Requirements

### Target Performance
- **Frame Rate**: 60 FPS stable
- **Input Lag**: < 3 frames
- **Network Latency**: Handle up to 150ms
- **Memory Usage**: < 2GB RAM

### Optimization Rules
1. Prefer stack allocation over heap
2. Use object pooling for projectiles
3. Implement spatial partitioning for collision
4. Cache frequently accessed data
5. Profile before optimizing

## Testing Requirements

### Unit Tests
- Test damage calculations
- Verify frame data
- Check state transitions
- Validate input handling

### Integration Tests
- Test character interactions
- Verify network synchronization
- Check game mode logic
- Test save/load functionality

## Version Control

### Commit Standards
- Use descriptive commit messages
- Reference issue numbers
- Keep commits atomic
- Test before committing

### Branch Strategy
- `main`: Stable releases
- `develop`: Active development
- `feature/*`: New features
- `fix/*`: Bug fixes

## Common Pitfalls to Avoid

1. **Don't use cooldowns** - Only mana costs limit skills
2. **Don't exceed file limits** - Split large files
3. **Don't ignore LSFDC patterns** - They ensure consistency
4. **Don't hardcode values** - Use configuration files
5. **Don't skip frame data** - It's essential for balance

## MCP Integration

When MCP servers are active, they provide:
- **balance-validator**: Automated balance checking
- **character-assets**: Asset management
- **combat-system**: Combat verification
- **data-persistence**: Save system
- **environment-generator**: Stage creation

## Quick Reference

### Essential Constants
```cpp
constexpr float BASE_HEALTH = 1000.0f;
constexpr float BASE_MANA = 100.0f;
constexpr float MANA_REGEN = 5.0f;
constexpr float COMBO_SCALING = 0.9f;
constexpr int MAX_COMBO_HITS = 15;
```

### Key Commands
- `/generate-character` - Create new character
- `/implement-combat` - Add combat features
- `/balance-check` - Verify game balance

Remember: When in doubt, check the LSFDC reference!