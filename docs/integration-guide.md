# Integration Guide: Aligning Character System with DFR Design

## Core Design Principle
- **Special Skills (S+Direction)**: MANA ONLY - No cooldowns
- **Gear Skills (AS, AD, ASD, SD)**: MANA + COOLDOWNS

## Required Changes to Character.h

### 1. Update SpecialSkill Structure
```cpp
// CURRENT (Incorrect - has cooldowns)
struct SpecialSkill {
    std::string name;
    std::string description;
    int manaCost;
    float damage;
    float animationDuration;
    float currentCooldown;    // REMOVE THIS
    float cooldown;           // REMOVE THIS
    CharacterStance requiredStance;
    
    // Visual effects
    XMFLOAT4 effectColor;
    std::string particleEffect;
    std::string soundEffect;
};

// CORRECTED (Mana only)
struct SpecialSkill {
    std::string name;
    std::string description;
    int manaCost;             // MANA COST ONLY
    float damage;
    float animationDuration;
    CharacterStance requiredStance;
    
    // Frame data (for balance)
    int startupFrames;
    int activeFrames;
    int recoveryFrames;
    
    // Visual effects
    XMFLOAT4 effectColor;
    std::string particleEffect;
    std::string soundEffect;
};
```

### 2. Update GearSkill Structure (Keep Cooldowns)
```cpp
// CORRECT - Gear skills have both mana and cooldowns
struct GearSkill {
    std::string name;
    std::string description;
    int manaCost;            // Mana cost
    float damage;
    float cooldownTime;      // COOLDOWN - This is correct for gear skills
    float currentCooldown;   // Track cooldown
    GearSlot slot;
    CharacterStance requiredStance;
    
    // Visual effects
    std::string animationName;
    std::string particleEffect;
};
```

### 3. Update ExecuteSpecialSkill Method
```cpp
// CURRENT (Checks cooldown - incorrect)
bool ExecuteSpecialSkill(const std::string& direction) {
    std::string skillKey = GetStanceName() + "_" + direction;
    
    auto it = specialSkills.find(skillKey);
    if (it == specialSkills.end()) {
        return false;
    }
    
    SpecialSkill& skill = it->second;
    
    // Check mana
    if (stats.currentMana < skill.manaCost) {
        return false;
    }
    
    // Check cooldown - REMOVE THIS CHECK
    if (skill.currentCooldown > 0.0f) {
        return false;
    }
    
    // ... rest of method
}

// CORRECTED (Mana only)
bool ExecuteSpecialSkill(const std::string& direction) {
    std::string skillKey = GetStanceName() + "_" + direction;
    
    auto it = specialSkills.find(skillKey);
    if (it == specialSkills.end()) {
        return false;
    }
    
    SpecialSkill& skill = it->second;
    
    // Check if blocking (can't use specials while blocking)
    if (isBlocking && blockHoldTime >= BLOCK_ACTIVATION_TIME) {
        return false;
    }
    
    // Check mana ONLY
    if (stats.currentMana < skill.manaCost) {
        return false;
    }
    
    // Check state
    if (currentState != CharacterState::Idle && 
        currentState != CharacterState::Walking) {
        return false;
    }
    
    // Execute skill - NO COOLDOWN SET
    stats.currentMana -= skill.manaCost;
    SetState(CharacterState::UsingSkill);
    stateTimer = skill.animationDuration;
    
    // Build ultimate gauge
    ultimateGauge += 10.0f;
    if (ultimateGauge > 100.0f) {
        ultimateGauge = 100.0f;
    }
    
    return true;
}
```

### 4. Update the Update Method
```cpp
void Update(float deltaTime) {
    // Mana regeneration
    if (stats.currentMana < stats.maxMana) {
        manaRegenTimer += deltaTime;
        if (manaRegenTimer >= 1.0f) {
            stats.currentMana += static_cast<int>(manaRegenRate);
            if (stats.currentMana > stats.maxMana) {
                stats.currentMana = stats.maxMana;
            }
            manaRegenTimer = 0.0f;
        }
    }
    
    // REMOVE special skill cooldown updates
    // Special skills don't have cooldowns!
    
    // Update GEAR skill cooldowns ONLY
    float cooldownMultiplier = 1.0f - stats.gearCooldownReduction;
    for (auto& [slot, skill] : gearSkills) {
        if (skill.currentCooldown > 0.0f) {
            skill.currentCooldown -= deltaTime * cooldownMultiplier;
            if (skill.currentCooldown < 0.0f) {
                skill.currentCooldown = 0.0f;
            }
        }
    }
    
    // ... rest of update logic
}
```

## Integration with DFR SpecialMoveSystem

To properly integrate with our DFR system:

### 1. Use DFR's InputDirection Enum
```cpp
// Instead of string-based directions
enum class InputDirection {
    Neutral,
    Up,      // S+↑
    Down,    // S+↓
    Left,    // S+←
    Right,   // S+→
    DownDown // Down+S (stance switch)
};
```

### 2. Connect to SpecialMoveSystem
```cpp
// In Character class, add:
private:
    std::unique_ptr<SpecialMoveSystem> specialMoveSystem;
    
public:
    void InitializeSpecialMoveSystem() {
        specialMoveSystem = std::make_unique<SpecialMoveSystem>();
        specialMoveSystem->setCharacter(this);
    }
    
    void HandleSpecialInput(InputDirection direction, bool sPressed) {
        if (sPressed) {
            specialMoveSystem->handleSButtonPress();
        } else {
            specialMoveSystem->handleSButtonRelease();
        }
        
        if (direction != InputDirection::Neutral) {
            specialMoveSystem->handleDirectionalInput(direction);
        }
    }
```

## Example Character Implementation

```cpp
class HyukWoonSung : public Character {
public:
    void InitializeSpecialSkills() override {
        // Light Stance Special Skills (MANA ONLY)
        specialSkills["Light_Up"] = {
            "Spear Sea Impact",
            "Creates 10 spear geysers",
            25,     // Mana cost
            150.0f, // Damage
            1.5f,   // Animation duration
            "Light", // Required stance
            15, 60, 20, // Frame data
            XMFLOAT4(0.3f, 0.7f, 1.0f, 1.0f), // Blue color
            "SpearGeyserEffect",
            "SpearImpactSound"
        };
        
        // NO COOLDOWNS FOR SPECIAL SKILLS
    }
    
    void InitializeGearSkills() override {
        // Gear Skills HAVE COOLDOWNS
        gearSkills[GearSlot::Weapon] = {
            "Glassy Death Rain",
            "Rain of light spears",
            30,      // Mana cost
            200.0f,  // Damage
            10.0f,   // COOLDOWN - This is correct for gear skills
            0.0f,    // Current cooldown
            GearSlot::Weapon,
            CharacterStance::Light,
            "GlassyRainAnimation",
            "LightSpearRainEffect"
        };
    }
};
```

## Summary

The key distinction in our DFR system:
- **S+Direction moves**: Strategic mana management, spam protection through mana cost
- **Gear skills**: Tactical cooldown management, powerful effects with timing requirements
- **Blocking**: 1-second activation prevents special move usage

This creates depth where players must balance:
1. Mana for frequent special moves
2. Cooldowns for powerful gear skills
3. Blocking for defense (sacrificing offense)