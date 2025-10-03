#pragma once

#include "../CharacterBase.h"
#include <memory>

namespace ArenaFighter {

// ============================================================================
// EVOLUTION GAUGE - Absorption/Evolution Energy
// ============================================================================

struct EvolutionGauge {
    float current = 0.0f;
    float maximum = 100.0f;

    // Generation rates
    static constexpr float BASIC_HIT = 1.5f;           // Per hit
    static constexpr float DAMAGE_TAKEN = 0.5f;        // Per 10 HP lost
    static constexpr float ON_KILL = 15.0f;            // Instant on kill
    static constexpr float ABSORPTION_SKILL = 5.0f;    // Base absorption
    static constexpr float EQUIPMENT_PICKUP = 7.0f;    // Per gear piece

    // Death penalty
    static constexpr float DEATH_PENALTY = 0.35f;      // -35% on death

    // Vajrayaksa drain
    static constexpr float VAJRAYAKSA_DRAIN_RATE = 1.0f / 3.0f;  // 1% per 3 seconds

    void Generate(float amount);
    bool CanAfford(float cost) const;
    void Consume(float cost);
    void Update(float deltaTime, bool isVajrayaksa);
    void ApplyDeathPenalty();
    float GetPercentage() const { return (current / maximum) * 100.0f; }
};

// ============================================================================
// EVOLUTION FORMS
// ============================================================================

enum class EvolutionForm {
    Goblin,        // 0-24%: Weakest, small, fast
    Hobgoblin,     // 25-49%: Shadow warrior, balanced
    Ogre,          // 50-74%: Brutal tank, heavy
    ApostleLord,   // 75-99%: Demon commander
    Vajrayaksa     // 100%: Four-armed god (meter drains)
};

// ============================================================================
// GOB THE GOOD GOBLIN - The Evolving Predator
// ============================================================================

class GobTheGoodGoblin : public CharacterBase {
public:
    GobTheGoodGoblin();

    // Core Update
    void Update(float deltaTime) override;

    // Evolution Gauge Management
    EvolutionGauge evolutionGauge;
    void GenerateEvolutionEnergy(float amount);
    void OnBasicAttackHit();
    void OnDamageTaken(float damage);
    void OnKill();
    void OnEquipmentPickup();
    void OnDeath();

    // Evolution System
    EvolutionForm currentForm = EvolutionForm::Goblin;

    void CheckEvolution();
    void EvolveToForm(EvolutionForm newForm);
    EvolutionForm DetermineFormFromGauge() const;

    // Evolution transitions
    void TransformToGoblin();
    void TransformToHobgoblin();
    void TransformToOgre();
    void TransformToApostleLord();
    void TransformToVajrayaksa();

    // Emergency Protocol
    bool emergencyProtocolUsed = false;
    void CheckEmergencyProtocol();
    void TriggerEmergencyEvolution();  // At 30% HP

    // Form state tracking
    bool isInEvolutionAnimation = false;
    float evolutionAnimationTimer = 0.0f;
    static constexpr float EVOLUTION_ANIMATION_DURATION = 2.0f;

    // Form-specific state
    int shadowPhaseChance = 10;           // Hobgoblin: 10% phase chance
    int vulcanusForgeStacks = 0;          // Ogre: Damage stacks
    int apostleDemonBuffDuration = 0;     // Apostle: Buff tracking
    bool vajrayaksaMeterDrainPaused = false;  // Vajrayaksa: Emergency drain pause

    // Combat Overrides
    void ExecuteSpecialMove(Direction direction) override;
    void ExecuteGearSkill(int index) override;
    void Block() override;
    bool CanUseSpecialMoves() const override;

    // Form-Specific Abilities
    // Goblin Form (0-24%)
    void GoblinAbilities(Direction direction);
    void GoblinGearSkills(int index);
    void GoblinBlock();

    // Hobgoblin Form (25-49%)
    void HobgoblinAbilities(Direction direction);
    void HobgoblinGearSkills(int index);
    void HobgoblinBlock();

    // Ogre Form (50-74%)
    void OgreAbilities(Direction direction);
    void OgreGearSkills(int index);
    void OgreBlock();

    // Apostle Lord Form (75-99%)
    void ApostleLordAbilities(Direction direction);
    void ApostleLordGearSkills(int index);
    void ApostleLordBlock();

    // Vajrayaksa Overlord Form (100%)
    void VajrayaksaAbilities(Direction direction);
    void VajrayaksaGearSkills(int index);
    void VajrayaksaBlock();

    // Helper methods
    float GetCurrentDamageMultiplier() const;
    float GetCurrentDefenseMultiplier() const;
    float GetCurrentSpeedMultiplier() const;
    float GetCurrentSizeMultiplier() const;

private:
    void InitializeGobStats();
    void ApplyFormStatModifications();

    // Base stats before modifications
    struct BaseStats {
        float maxHealth = 200.0f;
        float attack = 100.0f;
        float defense = 80.0f;
        float speed = 100.0f;
    } baseStats;
};

// ============================================================================
// FORM STAT MODIFIERS
// ============================================================================

struct FormStatModifiers {
    // Goblin (0-24%)
    struct Goblin {
        static constexpr float SIZE = 0.7f;           // Child-sized
        static constexpr float DAMAGE_DEALT = 0.85f;  // Weak attacks
        static constexpr float DAMAGE_TAKEN = 1.15f;  // Fragile
        static constexpr float SPEED = 1.3f;          // Fast movement
        static constexpr float HP = 180.0f;           // Low HP
    };

    // Hobgoblin (25-49%)
    struct Hobgoblin {
        static constexpr float SIZE = 1.0f;           // Normal
        static constexpr float DAMAGE_DEALT = 1.0f;   // Standard
        static constexpr float DAMAGE_TAKEN = 1.0f;   // Standard
        static constexpr float SPEED = 1.1f;          // Slightly fast
        static constexpr float HP = 200.0f;           // Normal HP
    };

    // Ogre (50-74%)
    struct Ogre {
        static constexpr float SIZE = 2.5f;           // Large
        static constexpr float DAMAGE_DEALT = 1.25f;  // Strong
        static constexpr float DAMAGE_TAKEN = 0.85f;  // Tanky
        static constexpr float SPEED = 0.9f;          // Slower
        static constexpr float HP = 220.0f;           // High HP
    };

    // Apostle Lord (75-99%)
    struct ApostleLord {
        static constexpr float SIZE = 2.0f;           // Medium-large
        static constexpr float DAMAGE_DEALT = 1.4f;   // Very strong
        static constexpr float DAMAGE_TAKEN = 0.7f;   // Resistant
        static constexpr float SPEED = 1.0f;          // Normal with air dash
        static constexpr float HP = 210.0f;           // Good HP
    };

    // Vajrayaksa (100%)
    struct Vajrayaksa {
        static constexpr float SIZE = 2.2f;           // Large with presence
        static constexpr float DAMAGE_DEALT = 1.6f;   // Massive damage
        static constexpr float DAMAGE_TAKEN = 0.5f;   // Heavily armored
        static constexpr float SPEED = 1.1f;          // Good speed
        static constexpr float HP = 200.0f;           // Balanced HP
    };
};

} // namespace ArenaFighter
