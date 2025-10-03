#pragma once

#include "../CharacterBase.h"
#include <memory>
#include <vector>

namespace ArenaFighter {

// ============================================================================
// AUTHORITY GAUGE - Primary Resource
// ============================================================================

struct AuthorityGauge {
    float current = 0.0f;
    float maximum = 100.0f;

    // Generation rates
    static constexpr float BASIC_ATTACK = 2.0f;
    static constexpr float DAMAGE_TAKEN = 3.0f;  // Per 100 damage
    static constexpr float SPECIAL_MOVE = 3.0f;
    static constexpr float JUMP = 1.0f;
    static constexpr float EXECUTION = 30.0f;
    static constexpr float COMBAT_PASSIVE = 5.0f;  // Per second in combat

    void Generate(float amount);
    bool CanAfford(float cost) const;
    void Consume(float cost);
    void Update(float deltaTime, bool inCombat);
    void FillToMax();
};

// ============================================================================
// BLOOD ESSENCE - Secondary Resource
// ============================================================================

struct BloodEssence {
    int current = 0;
    int maximum = 10;

    void Generate(int amount);
    bool CanAfford(int cost) const;
    void Consume(int cost);
};

// ============================================================================
// BLOOD RESONANCE STACKS - Tertiary Resource
// ============================================================================

struct BloodResonanceStacks {
    int current = 0;
    int maximum = 20;
    int ultimateMaximum = 30;

    void AddStack();
    void AddPermanentStack();  // From executions
    void Reset();
    float GetDamageBonus() const { return current * 0.01f; }  // +1% per stack
};

// ============================================================================
// BLOOD PUPPET - Summoned Clone
// ============================================================================

class BloodPuppet {
public:
    BloodPuppet(float statMultiplier);

    float health;
    float maxHealth;
    float damage;
    bool isAlive;
    float x, y, z;

    // Blood Tax mechanic
    float bloodTaxTimer = 0.0f;
    static constexpr float BLOOD_TAX_INTERVAL = 3.0f;
    static constexpr float BLOOD_TAX_RADIUS = 8.0f;
    static constexpr float BLOOD_TAX_HP_DRAIN = 0.02f;  // 2% max HP

    void Update(float deltaTime);
    void OnDeath();  // Explodes for 80 damage
};

// ============================================================================
// BLOOD CONSTRUCT - Placeable Structure
// ============================================================================

enum class ConstructType {
    CrimsonBastion,    // Wall - defensive
    HemorrhageSpire,   // Tower - offensive
    SanguineNexus,     // Fountain - support
    BloodAnchor        // Gravity well - control
};

class BloodConstruct {
public:
    BloodConstruct(ConstructType type);

    ConstructType type;
    float health;
    float maxHealth;
    float x, y, z;
    float lifetime;
    bool isEvolved;

    static constexpr float BASE_LIFETIME = 25.0f;
    static constexpr float EVOLUTION_TIME = 25.0f;
    static constexpr float BASE_HP = 300.0f;

    void Update(float deltaTime);
    void Evolve();  // Becomes Greater Construct
    void Sacrifice();  // Manual detonation
};

// ============================================================================
// BLOOD FORMS - Transformation States
// ============================================================================

enum class BloodForm {
    None,
    CrimsonReaver,     // Offensive
    SanguineFortress,  // Defensive
    HemomagueWraith,   // Debuff
    VitaeSovereign     // Support
};

// ============================================================================
// MISS BAT CRIMSON AUTHORITY
// ============================================================================

class MissBatCrimsonAuthority : public CharacterBase {
public:
    MissBatCrimsonAuthority();

    // Core Update
    void Update(float deltaTime) override;

    // Resource Management
    AuthorityGauge authorityGauge;
    BloodEssence bloodEssence;
    BloodResonanceStacks bloodResonance;

    void GenerateAuthority(float amount);
    void OnBasicAttackHit();
    void OnDamageTaken(float damage);
    void OnSpecialMoveUsed();
    void OnJump();

    // Blood Puppet System (SD Weapon)
    std::vector<std::shared_ptr<BloodPuppet>> bloodPuppets;
    int GetMaxPuppets() const;
    void CreateBloodPuppet(float statMultiplier);
    void UpdateBloodPuppets(float deltaTime);
    void FusePuppetsToGolem();

    // Blood Construct System (ASD Armor)
    std::vector<std::shared_ptr<BloodConstruct>> bloodConstructs;
    int GetMaxConstructs() const;
    void PlaceConstruct(ConstructType type, float x, float y, float z);
    void UpdateConstructs(float deltaTime);
    void SacrificeConstruct(int index);
    bool CheckConstructResonance();

    // Blood Form Transformation (AS Cloak)
    BloodForm currentForm = BloodForm::None;
    float formDuration = 0.0f;
    float formMasteryPoints = 0.0f;
    bool isTranscendent = false;

    void TransformToForm(BloodForm form);
    void UpdateTransformation(float deltaTime);
    void EndTransformation();
    void QuickSwapForm(BloodForm newForm);

    // Gear Theft System (AD Helmet)
    struct StolenSkill {
        int skillType;  // Weapon/Helmet/Armor/Cloak
        // Additional skill data would be stored here
    };
    std::vector<StolenSkill> stolenSkills;
    int GetMaxStolenSkills() const;
    void StealGearSkill();
    void UseStolenSkill(int index);

    // Crimson Execution System
    float executionCastTimer = 0.0f;
    int executionTargetId = -1;
    bool isExecuting = false;

    bool CanExecuteTarget(int targetId) const;
    void StartExecution(int targetId);
    void UpdateExecution(float deltaTime);
    void CompleteExecution();
    void InterruptExecution();

    static constexpr float EXECUTION_CAST_TIME = 4.0f;
    static constexpr float EXECUTION_RANGE = 6.0f;

    // Ultimate - Sovereign of Blood
    bool isInUltimate = false;
    float ultimateTimeRemaining = 0.0f;
    bool ultimateRecovery = false;
    float ultimateRecoveryTimer = 0.0f;

    void ActivateUltimate();
    void UpdateUltimate(float deltaTime);
    void EndUltimate();
    void CrimsonCataclysm();  // Ultimate finisher

    static constexpr float ULTIMATE_DURATION = 15.0f;
    static constexpr float ULTIMATE_RECOVERY_TIME = 3.0f;

    // Combat Overrides
    void ExecuteSpecialMove(Direction direction) override;
    void ExecuteGearSkill(int index) override;
    void Block() override;
    bool CanUseSpecialMoves() const override;

    // Base Kit Abilities
    void BloodSpearVolley(bool charged);
    void CrimsonStep();
    void CrimsonArsenal();
    void BloodSurge();
    void RisingCrimson();
    void CrimsonDescent();
    void BloodGeyser();
    void BloodSpikeTrap();
    void HemorrhageJavelin(float chargeTime);
    void BloodEchoStrike();

    // Helper Methods
    float GetAuthorityScaledMultiplier() const;
    float GetDamageMultiplier() const;
    bool IsAuthorityEnhanced() const { return authorityGauge.current >= 50.0f; }
    bool IsAuthorityMaximum() const { return authorityGauge.current >= 75.0f; }

private:
    void InitializeMissBatStats();

    // Execution thresholds
    struct ExecutionThresholds {
        static constexpr float NORMAL = 0.0f;          // Requires setup
        static constexpr float BLOOD_ANCHOR = 0.40f;   // 40% HP
        static constexpr float HEMORRHAGE = 0.40f;     // 40% HP
        static constexpr float HEMOPHILIA = 0.35f;     // 35% HP
        static constexpr float ULTIMATE = 0.35f;       // 35% HP
        static constexpr float CATACLYSM = 0.30f;      // 30% HP
    };
};

// ============================================================================
// ULTIMATE ENHANCEMENTS
// ============================================================================

struct UltimateEnhancements {
    static constexpr float MOVEMENT_SPEED_BONUS = 0.50f;  // +50%
    static constexpr float ATTACK_SPEED_BONUS = 0.60f;    // +60%
    static constexpr float DAMAGE_BONUS = 0.40f;          // +40%
    static constexpr float DAMAGE_REDUCTION = 0.30f;      // 30% reduction
    static constexpr float AUTHORITY_COST_REDUCTION = 0.50f;  // 50% cost

    static constexpr float EXECUTION_CAST_TIME = 2.0f;    // Reduced from 4s
    static constexpr float EXECUTION_THRESHOLD = 0.35f;   // 35% HP
    static constexpr float EXECUTION_RANGE = 8.0f;        // Increased from 6m
    static constexpr float EXECUTION_COOLDOWN = 4.0f;
    static constexpr int EXECUTION_DURATION_EXTENSION = 3;  // Seconds

    static constexpr int INSTANT_RESONANCE = 20;
    static constexpr int MAX_RESONANCE_ULTIMATE = 30;
    static constexpr float RESONANCE_DAMAGE_PER_STACK = 0.02f;  // 2% per stack

    static constexpr float PASSIVE_AUTHORITY_REGEN = 10.0f;  // Per second
    static constexpr float AUTHORITY_MULTIPLIER = 2.0f;      // Double generation
    static constexpr float DAMAGE_TO_AUTHORITY = 5.0f;       // Per 100 damage

    // Blood Field Aura
    static constexpr float AURA_RADIUS = 15.0f;
    static constexpr float ENEMY_DAMAGE_DEBUFF = 0.25f;     // -25%
    static constexpr float ENEMY_SPEED_DEBUFF = 0.20f;      // -20%
    static constexpr float ENEMY_DOT = 40.0f;               // Damage per second
    static constexpr float ALLY_DAMAGE_BUFF = 0.20f;        // +20%
    static constexpr float ALLY_SPEED_BUFF = 0.15f;         // +15%
    static constexpr float ALLY_REGEN = 30.0f;              // HP per second
};

// ============================================================================
// ABILITY COSTS
// ============================================================================

struct AbilityCosts {
    // Base Abilities
    static constexpr float BLOOD_SPEAR_VOLLEY = 10.0f;
    static constexpr float BLOOD_SPEAR_CHARGED = 15.0f;
    static constexpr float CRIMSON_ARSENAL = 20.0f;
    static constexpr float BLOOD_SURGE = 15.0f;
    static constexpr float RISING_CRIMSON = 12.0f;
    static constexpr float CRIMSON_DESCENT = 12.0f;
    static constexpr float BLOOD_GEYSER = 18.0f;
    static constexpr float BLOOD_SPIKE_TRAP = 20.0f;
    static constexpr float HEMORRHAGE_JAVELIN = 25.0f;
    static constexpr float BLOOD_ECHO_STRIKE = 15.0f;

    // Gear Skills
    static constexpr float HEMOMANCER_SIPHON = 30.0f;
    static constexpr float CRIMSON_USURPER = 20.0f;
    static constexpr float CRIMSON_CARAPACE = 30.0f;
    static constexpr int CONSTRUCT_ESSENCE_COST = 5;

    // Blood Essence Costs
    static constexpr int FORM_SWAP_COST = 3;
    static constexpr int FORM_SWAP_ENHANCED = 2;
};

} // namespace ArenaFighter
