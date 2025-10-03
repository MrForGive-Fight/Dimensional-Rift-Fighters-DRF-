#pragma once

#include "../CharacterBase.h"
#include <memory>

namespace ArenaFighter {

// ============================================================================
// CULTIVATION ESSENCE - Primary Resource
// ============================================================================

struct CultivationEssence {
    float current = 100.0f;
    float maximum = 100.0f;

    // Regeneration rates
    static constexpr float ON_HIT = 3.0f;
    static constexpr float ON_DEFENSE = 2.0f;
    static constexpr float NATURAL_REGEN = 1.0f;  // Per second

    void Generate(float amount);
    bool CanAfford(float cost) const;
    void Consume(float cost);
    void Update(float deltaTime);
    void FillToMax();
};

// ============================================================================
// CONVERGENCE METER - Secondary Resource
// ============================================================================

struct ConvergenceMeter {
    float current = 0.0f;
    float maximum = 100.0f;

    // Build rates
    static constexpr float PER_TECHNIQUE = 2.0f;
    static constexpr float COMBAT_PASSIVE = 1.0f;  // Per second in combat

    void Generate(float amount);
    void Update(float deltaTime, bool inCombat);
    void FillToMax();
    bool IsMaxed() const { return current >= maximum; }
    void Lock() { isLocked = true; }
    void Unlock() { isLocked = false; }

private:
    bool isLocked = false;
};

// ============================================================================
// DAO PATHS
// ============================================================================

enum class DaoPath {
    Ice,     // Glacial Purity - Slow/freeze/crowd control
    Poison   // Toxic Decay - DoT/critical hits
};

// ============================================================================
// SERAPHINA - Celestial Poison Sage
// ============================================================================

class Seraphina : public CharacterBase {
public:
    Seraphina();

    // Core Update
    void Update(float deltaTime) override;

    // Resource Management
    CultivationEssence cultivationEssence;
    ConvergenceMeter convergenceMeter;

    void GenerateCultivationEssence(float amount);
    void OnSuccessfulHit();
    void OnSuccessfulDefense();
    void OnTechniqueUsed();

    // Dao Control System
    DaoPath currentDao = DaoPath::Ice;
    void ToggleDao();  // S+Down switches Ice ↔ Poison
    bool IsIceDao() const { return currentDao == DaoPath::Ice; }
    bool IsPoisonDao() const { return currentDao == DaoPath::Poison; }

    // Heavenly Convergence State
    bool isInConvergenceState = false;
    float convergenceTimeRemaining = 0.0f;
    bool convergenceEnhanced = false;  // Emergency protocol version

    void CheckConvergenceActivation();
    void ActivateConvergence(bool enhanced = false);
    void UpdateConvergence(float deltaTime);
    void EndConvergence();

    // Convergence durations
    static constexpr float NORMAL_CONVERGENCE_DURATION = 20.0f;
    static constexpr float ENHANCED_CONVERGENCE_DURATION = 25.0f;

    // Emergency Protocol
    bool angelsDesperation Used = false;
    void CheckEmergencyProtocol();
    void TriggerAngelsDesperateAscension();  // At 30% HP

    // Poison Stack System
    struct PoisonStackData {
        int stacks = 0;
        float duration = 0.0f;
        static constexpr int MAX_NORMAL_STACKS = 3;
        static constexpr int MAX_AWAKENED_STACKS = 6;
        static constexpr float STACK_DURATION = 3.0f;
        static constexpr float SLOW_FROZEN_BONUS = 0.10f;  // +10% damage
    };
    // Would need to track per enemy in real implementation

    // Combat Overrides
    void ExecuteSpecialMove(Direction direction) override;
    void ExecuteGearSkill(int index) override;
    void Block() override;
    bool CanUseSpecialMoves() const override;

    // Dao-Specific Abilities
    // Ice Dao Techniques
    void IceDaoAbilities(Direction direction);
    void IceDaoGearSkills(int index);
    void IceDaoBlock();

    // Poison Dao Techniques
    void PoisonDaoAbilities(Direction direction);
    void PoisonDaoGearSkills(int index);
    void PoisonDaoBlock();

    // Convergence State Techniques
    void ConvergenceAbilities(Direction direction);
    void ConvergenceGearSkills(int index);
    void ConvergenceBlock();

    // Wing Technique (SD - Dao Awakening)
    bool isDaoAwakened = false;
    float daoAwakeningTimer = 0.0f;
    void ActivateDaoAwakening(bool enhancedDual = false);

    // Cultivation Skills State
    bool celestialAuthorityActive = false;
    float celestialAuthorityTimer = 0.0f;

    bool iceMirrorBombActive = false;
    bool toxicShadowBombActive = false;
    float cloneBombTimer = 0.0f;

    bool glacialSymbolActive = false;
    bool toxicSymbolActive = false;
    float elementSymbolTimer = 0.0f;

    bool heavensDominionActive = false;
    float heavensDominionTimer = 0.0f;

    // Helper methods
    float GetDamageMultiplier() const;
    float GetEssenceCostMultiplier() const;
    float GetCooldownReduction() const;

private:
    void InitializeSeraphinaStats();
    void ApplyConvergenceEnhancements();
};

// ============================================================================
// CONVERGENCE STATE BONUSES
// ============================================================================

struct ConvergenceEnhancements {
    // Emergency Protocol bonuses
    struct Emergency {
        static constexpr float DAMAGE_BOOST = 2.0f;      // +100% damage
        static constexpr float ESSENCE_EFFICIENCY = 0.5f; // 50% cost
        static constexpr float COOLDOWN_REDUCTION = 0.6f; // 60% reduced
        static constexpr float LIFE_STEAL = 0.25f;        // 25% healing
        static constexpr float SLOW_DEBUFF = 0.30f;       // 30% slow aura
        static constexpr float DAMAGE_DEBUFF = 0.20f;     // 20% damage reduction
    };
};

// ============================================================================
// TECHNIQUE COSTS
// ============================================================================

struct TechniqueCosts {
    // Special Moves
    static constexpr float ASCENDING_TECHNIQUE = 10.0f;
    static constexpr float FORWARD_TECHNIQUE = 15.0f;
    static constexpr float RETREAT_TECHNIQUE = 12.0f;

    // D Hold
    static constexpr float FROZEN_MIRROR_DASH = 20.0f;
    static constexpr float POISON_HUNT = 20.0f;
    static constexpr float DIMENSIONAL_RIFT_GATE = 30.0f;

    // Gear Skills - Essence Enhancements
    static constexpr float DAO_AWAKENING_ENHANCE = 35.0f;
    static constexpr float CELESTIAL_AUTHORITY_ENHANCE = 15.0f;
    static constexpr float DAO_CLONE_ENHANCE = 20.0f;
    static constexpr float ELEMENT_DAO_ENHANCE = 20.0f;
    static constexpr float HEAVENS_DOMINION_ENHANCE = 25.0f;
};

} // namespace ArenaFighter
