# Rendering System Integration Guide

## Overview

The provided RenderingSystem.h uses an incorrect Character API that gives special skills cooldowns. This document explains how the RenderingSystemAdapter integrates it with DFR's correct implementation.

## Key Differences

### Character API Mapping

| Provided System | DFR System |
|----------------|------------|
| Character class with special skills having cooldowns | CharacterBase with special moves (mana-only) |
| GetStance() returns enum | GetCurrentStance() returns int (0=Light, 1=Dark) |
| IsBlockActive() checks 1-second hold | SpecialMoveSystem manages block state |
| Skill execution with cooldowns | Special moves use mana, gear skills have cooldowns |

## Integration Architecture

```
RenderingSystem.h (provided)
         |
         v
RenderingSystemAdapter
    |         |
    v         v
CharacterView  DFRRenderingSystem
    |              |
    v              v
CharacterBase   DirectX 11 (ComPtr)
```

## Usage Examples

### Basic Setup
```cpp
// Create DirectX device and context
ComPtr<ID3D11Device> device;
ComPtr<ID3D11DeviceContext> context;
// ... initialize DirectX ...

// Create DFR rendering system
auto renderSystem = std::make_unique<RenderingSystemAdapter::DFRRenderingSystem>(
    device, context);
renderSystem->Initialize(1920, 1080);
```

### Rendering Characters
```cpp
// Option 1: Direct DFR character rendering
CharacterBase* character = GetCharacter();
XMFLOAT3 position(0, 0, 0);
renderSystem->RenderCharacter(character, position);

// Option 2: Using CharacterView for compatibility
RenderingSystemAdapter::CharacterView view(character, specialMoveSystem);
view.SetRotation(XMFLOAT3(0, playerRotation, 0));
renderSystem->RenderCharacterWithView(view, position);
```

### Creating Effects

#### Special Move Effects (Mana-Only)
```cpp
// When executing a special move
character->ExecuteSpecialMove(InputDirection::Up);
renderSystem->CreateSpecialMoveEffect(character, InputDirection::Up, position);
```

#### Gear Skill Effects (Mana + Cooldown)
```cpp
// When using a gear skill
int skillIndex = 2; // AS skill
if (!character->IsGearSkillOnCooldown(skillIndex)) {
    character->StartGearSkillCooldown(skillIndex);
    renderSystem->CreateGearSkillEffect(character, skillIndex, position);
}
```

#### Stance Switch Effects
```cpp
// When switching stance (Murim characters)
int oldStance = character->GetCurrentStance();
character->SwitchStance(1 - oldStance);
renderSystem->CreateStanceSwitchEffect(character, oldStance, 1 - oldStance, position);
```

## Particle System

The adapter includes a particle system that respects DFR's skill distinction:

- **Special Move Particles**: Direction-based, shorter duration
- **Gear Skill Particles**: Spiral pattern, longer lasting
- **Stance Effects**: Yin-yang pattern for Light/Dark transitions
- **Hit Effects**: Scale with damage amount
- **Block Effects**: Blue shield bubble

## Color Coding

Characters are tinted based on state and category:

### State Colors
- **Blocking**: Blue (0.5, 0.5, 1.0)
- **Special Move**: Category-specific
- **Ultimate/Transform**: Gold (1.0, 0.8, 0.0)
- **Hit Stun**: Red tint (1.0, 0.5, 0.5)
- **Knocked Down**: Gray (0.5, 0.5, 0.5)

### Category Colors (During Special Moves)
- **System**: Light purple
- **GodsHeroes**: Golden
- **Murim**: Light blue/Dark red (stance-based)
- **Cultivation**: Jade green
- **Animal**: Brown
- **Monsters**: Purple
- **Chaos**: Crimson

## Performance Considerations

1. **ComPtr Usage**: All DirectX resources use ComPtr for automatic cleanup
2. **Particle Limits**: Keep particle count reasonable (50 max per effect)
3. **Update Frequency**: Particle updates run at game tick rate (60Hz)
4. **Instanced Rendering**: Consider implementing for many particles

## Shader Integration

The system includes HLSL shaders in `DFRShaders.hlsl`:
- Standard vertex/pixel shaders with rim lighting
- Particle shaders with soft edges
- Special effect shaders (stance switch, ground impact)

## Common Issues and Solutions

### Issue: Character appears with wrong color
**Solution**: Ensure CharacterBase state is properly synchronized

### Issue: Particles not appearing
**Solution**: Check that particle vertex buffer is created and bound

### Issue: Effects play during cooldown
**Solution**: Always check `IsGearSkillOnCooldown()` before gear skills

## Future Improvements

1. Add texture support for characters
2. Implement shadow mapping
3. Add post-processing effects
4. Create arena-specific shaders
5. Add weather effects system