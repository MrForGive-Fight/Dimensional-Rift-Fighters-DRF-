#pragma once

#include "../CharacterBase.h"
#include <memory>
#include <vector>

namespace ArenaFighter {

// ============================================================================
// STOLEN PANTHEON GAUGE - Divine Power Resource
// ============================================================================

struct StolenPantheonGauge {
    float current = 0.0f;
    float maximum = 100.0f;

    // Generation rates
    static constexpr float PASSIVE_REGEN = 1.5f;       // Per second
    static constexpr float ON_BASIC_HIT = 4.0f;        // Per hit
    static constexpr float ON_DIVINE_THEFT = 8.0f;     // Special move hit
    static constexpr float ON_DAMAGE_TAKEN = 0.15f;    // Per damage point
    static constexpr float ON_KILL = 25.0f;            // Per enemy defeated

    // Corruption costs
    static constexpr float VULCANUS_COST = 30.0f;      // Fire/Tank form
    static constexpr float MERCURIUS_COST = 35.0f;     // Speed/Theft form
    static constexpr float DIANA_COST = 40.0f;         // Range/Debuff form
    static constexpr float PLUTO_COST = 100.0f;        // Ultimate form

    void Generate(float amount);
    bool CanAfford(float cost) const;
    void Consume(float cost);
    void Update(float deltaTime);
    void Reset();
};

// ============================================================================
// CORRUPTION FORMS - God Transformations
// ============================================================================

enum class CorruptionForm {
    None,               // Base Divine Thief form
    Vulcanus,          // Corrupted Hephaestus - Fire/Tank (Warhammer)
    Mercurius,         // Corrupted Hermes - Speed/Theft (Twin Blades)
    Diana,             // Corrupted Artemis - Range/Debuff (Bow)
    CorruptedPluto     // Ultimate - Death God (All weapons + 3 clones)
};

// ============================================================================
// GOD CLONE - Autonomous AI God Entity
// ============================================================================

enum class GodType {
    Vulcanus,   // Aggressive tank, fire attacks
    Mercurius,  // Fast striker, theft mechanics
    Diana       // Ranged support, debuffs
};

class GodClone {
public:
    GodClone(GodType type);
    virtual ~GodClone() = default;

    GodType type;
    float health;
    float maxHealth;
    float damage[4];
    bool isAlive;
    float x, y, z;  // Position
    float speedMultiplier;

    // AI behavior
    virtual void UpdateAI(float deltaTime, float playerX, float playerY, float playerZ) = 0;
    virtual void Attack() = 0;
    virtual void OnDeath() = 0;

    bool IsAlive() const { return isAlive; }
};

// Vulcanus Clone - Fire Titan
class VulcanusClone : public GodClone {
public:
    VulcanusClone();
    void UpdateAI(float deltaTime, float playerX, float playerY, float playerZ) override;
    void Attack() override;
    void OnDeath() override;

    void HammerSlam();      // Ground pound AOE
    void ForgeStrike();     // Single target heavy hit
    void MoltenShield();    // Temporary invulnerability

private:
    float attackTimer = 0.0f;
    float slamTimer = 0.0f;
    float shieldTimer = 0.0f;
    bool isShielded = false;
};

// Mercurius Clone - Swift Thief
class MercuriusClone : public GodClone {
public:
    MercuriusClone();
    void UpdateAI(float deltaTime, float playerX, float playerY, float playerZ) override;
    void Attack() override;
    void OnDeath() override;

    void BlitzStrike();         // Rapid dash attack
    void StealBuff();           // Steal enemy buffs
    void WindStep();            // Teleport dodge

private:
    float attackTimer = 0.0f;
    float stealTimer = 0.0f;
    float dodgeTimer = 0.0f;
    int consecutiveHits = 0;
};

// Diana Clone - Moonlight Huntress
class DianaClone : public GodClone {
public:
    DianaClone();
    void UpdateAI(float deltaTime, float playerX, float playerY, float playerZ) override;
    void Attack() override;
    void OnDeath() override;

    void MoonlightArrow();      // Ranged attack
    void CurseShot();           // Debuff projectile
    void HuntersMark();         // Mark enemy for bonus damage

private:
    float attackTimer = 0.0f;
    float curseTimer = 0.0f;
    float markTimer = 0.0f;
    float maintainDistance = 150.0f;
};

// ============================================================================
// HYOUDOU KOTETSU - The God Thief
// ============================================================================

class HyoudouKotetsu : public CharacterBase {
public:
    HyoudouKotetsu();

    // Core Update
    void Update(float deltaTime) override;

    // Resource Management
    StolenPantheonGauge pantheonGauge;
    void GeneratePantheonPower(float amount);
    void OnBasicAttackHit();
    void OnDivineTheftHit();
    void OnDamageTaken(float damage);
    void OnKill();

    // Corruption System
    CorruptionForm currentForm = CorruptionForm::None;
    float corruptionTimeRemaining = 0.0f;

    bool TransformToVulcanus();
    bool TransformToMercurius();
    bool TransformToDiana();
    bool TransformToCorruptedPluto();

    void EndCorruption();
    void UpdateCorruption(float deltaTime);
    bool IsCorrupted() const { return currentForm != CorruptionForm::None; }

    // God Clone Management (Pluto form only)
    std::vector<std::shared_ptr<GodClone>> godClones;
    void SummonGodClones();
    void UpdateGodClones(float deltaTime);
    void DismissGodClones();

    // Emergency Protocol
    bool pantheonEndUsed = false;
    void CheckPantheonEnd();
    void TriggerPantheonEnd();  // At 25% HP

    // Combat Overrides
    void ExecuteSpecialMove(Direction direction) override;
    void ExecuteGearSkill(int index) override;
    void Block() override;
    bool CanUseSpecialMoves() const override;

    // Form-Specific Abilities
    // Base Form - Divine Thief
    void BaseDivineTheftAbilities(Direction direction);
    void BaseDivineTheftGearSkills(int index);
    void BaseDivineTheftBlock();

    // Vulcanus Form - Fire Titan
    void VulcanusAbilities(Direction direction);
    void VulcanusGearSkills(int index);
    void VulcanusBlock();

    // Mercurius Form - Swift Thief
    void MercuriusAbilities(Direction direction);
    void MercuriusGearSkills(int index);
    void MercuriusBlock();

    // Diana Form - Moonlight Huntress
    void DianaAbilities(Direction direction);
    void DianaGearSkills(int index);
    void DianaBlock();

    // Corrupted Pluto Form - Death God
    void PlutoAbilities(Direction direction);
    void PlutoGearSkills(int index);
    void PlutoBlock();

private:
    void InitializeHyoudouStats();
    void SetupBaseGearSkills();

    // Corruption durations
    static constexpr float VULCANUS_DURATION = 12.0f;
    static constexpr float MERCURIUS_DURATION = 10.0f;
    static constexpr float DIANA_DURATION = 15.0f;
    static constexpr float PLUTO_DURATION = 20.0f;
    static constexpr float PANTHEON_END_DURATION = 25.0f;

    // Form-specific state
    int vulcanusStackCount = 0;      // Forge stacks for damage
    int mercuriusStolenBuffs = 0;    // Stolen buff count
    int dianaMarkedEnemies = 0;      // Marked enemy count
};

} // namespace ArenaFighter
