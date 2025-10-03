#include "MissBatCrimsonAuthority.h"
#include "../../Combat/DamageCalculator.h"
#include <algorithm>
#include <cmath>

namespace ArenaFighter {

// ============================================================================
// AuthorityGauge Implementation
// ============================================================================

void AuthorityGauge::Generate(float amount) {
    current = std::min(current + amount, maximum);
}

bool AuthorityGauge::CanAfford(float cost) const {
    return current >= cost;
}

void AuthorityGauge::Consume(float cost) {
    current = std::max(0.0f, current - cost);
}

void AuthorityGauge::Update(float deltaTime, bool inCombat) {
    if (inCombat) {
        Generate(COMBAT_PASSIVE * deltaTime);
    }
}

void AuthorityGauge::FillToMax() {
    current = maximum;
}

// ============================================================================
// BloodEssence Implementation
// ============================================================================

void BloodEssence::Generate(int amount) {
    current = std::min(current + amount, maximum);
}

bool BloodEssence::CanAfford(int cost) const {
    return current >= cost;
}

void BloodEssence::Consume(int cost) {
    current = std::max(0, current - cost);
}

// ============================================================================
// BloodResonanceStacks Implementation
// ============================================================================

void BloodResonanceStacks::AddStack() {
    if (current < maximum) {
        current++;
    }
}

void BloodResonanceStacks::AddPermanentStack() {
    if (current < maximum) {
        current++;
        // This stack persists through death
    }
}

void BloodResonanceStacks::Reset() {
    current = 0;
}

// ============================================================================
// BloodPuppet Implementation
// ============================================================================

BloodPuppet::BloodPuppet(float statMultiplier) {
    maxHealth = 200.0f * statMultiplier;
    health = maxHealth;
    damage = 30.0f * statMultiplier;
    isAlive = true;
    x = y = z = 0.0f;
}

void BloodPuppet::Update(float deltaTime) {
    if (!isAlive) return;

    bloodTaxTimer += deltaTime;

    // Blood Tax: Drain nearby enemies
    if (bloodTaxTimer >= BLOOD_TAX_INTERVAL) {
        // TODO: Find nearby enemies and drain HP
        // Convert drained HP to Blood Essence
        bloodTaxTimer = 0.0f;
    }

    // TODO: Puppet AI and attack logic
}

void BloodPuppet::OnDeath() {
    isAlive = false;
    // Explodes for 80 damage in 5m radius
    // TODO: Create explosion effect
}

// ============================================================================
// BloodConstruct Implementation
// ============================================================================

BloodConstruct::BloodConstruct(ConstructType type)
    : type(type)
    , health(BASE_HP)
    , maxHealth(BASE_HP)
    , lifetime(0.0f)
    , isEvolved(false) {
    x = y = z = 0.0f;
}

void BloodConstruct::Update(float deltaTime) {
    lifetime += deltaTime;

    // Check for evolution
    if (!isEvolved && lifetime >= EVOLUTION_TIME) {
        Evolve();
    }

    // Type-specific update logic
    switch (type) {
        case ConstructType::CrimsonBastion:
            // Wall blocks movement, damages touching enemies
            // TODO: Implement collision and damage
            break;

        case ConstructType::HemorrhageSpire:
            // Shoots blood spikes at enemies
            // TODO: Implement attack logic
            break;

        case ConstructType::SanguineNexus:
            // Heals allies, stores healing
            // TODO: Implement heal logic
            break;

        case ConstructType::BloodAnchor:
            // Pulls enemies toward center
            // TODO: Implement gravity well
            break;
    }
}

void BloodConstruct::Evolve() {
    isEvolved = true;
    maxHealth *= 2.0f;
    health = maxHealth;
    // TODO: Apply evolved effects
}

void BloodConstruct::Sacrifice() {
    // Detonate construct for specific effects
    // TODO: Implement sacrifice effects per type
}

// ============================================================================
// MissBatCrimsonAuthority Constructor
// ============================================================================

MissBatCrimsonAuthority::MissBatCrimsonAuthority() {
    InitializeMissBatStats();
}

void MissBatCrimsonAuthority::InitializeMissBatStats() {
    // Very Hard difficulty character - complex mechanics
    stats.maxHealth = 220.0f;
    stats.health = 220.0f;
    stats.attack = 100.0f;
    stats.defense = 85.0f;
    stats.speed = 100.0f;
    stats.maxMana = 100.0f;
    stats.mana = 100.0f;
    stats.manaRegen = 5.0f;

    // Authority starts at 0
    authorityGauge.current = 0.0f;
    authorityGauge.maximum = 100.0f;

    bloodEssence.current = 0;
    bloodEssence.maximum = 10;

    bloodResonance.current = 0;
    bloodResonance.maximum = 20;
}

// ============================================================================
// Core Update Loop
// ============================================================================

void MissBatCrimsonAuthority::Update(float deltaTime) {
    CharacterBase::Update(deltaTime);

    // Update Authority gauge
    bool inCombat = true;  // TODO: Determine combat state
    authorityGauge.Update(deltaTime, inCombat);

    // Update Blood Puppets
    UpdateBloodPuppets(deltaTime);

    // Update Blood Constructs
    UpdateConstructs(deltaTime);

    // Update Blood Form transformation
    if (currentForm != BloodForm::None) {
        UpdateTransformation(deltaTime);
    }

    // Update Crimson Execution
    if (isExecuting) {
        UpdateExecution(deltaTime);
    }

    // Update Ultimate
    if (isInUltimate) {
        UpdateUltimate(deltaTime);
    } else if (ultimateRecovery) {
        ultimateRecoveryTimer -= deltaTime;
        if (ultimateRecoveryTimer <= 0.0f) {
            ultimateRecovery = false;
            // Restore Authority to 30
            authorityGauge.current = 30.0f;
        }
    }
}

// ============================================================================
// Authority Generation
// ============================================================================

void MissBatCrimsonAuthority::GenerateAuthority(float amount) {
    float multiplier = isInUltimate ? UltimateEnhancements::AUTHORITY_MULTIPLIER : 1.0f;
    authorityGauge.Generate(amount * multiplier);
}

void MissBatCrimsonAuthority::OnBasicAttackHit() {
    GenerateAuthority(AuthorityGauge::BASIC_ATTACK);
}

void MissBatCrimsonAuthority::OnDamageTaken(float damage) {
    float authorityGain = (damage / 100.0f) * AuthorityGauge::DAMAGE_TAKEN;
    GenerateAuthority(authorityGain);

    if (isInUltimate) {
        GenerateAuthority(UltimateEnhancements::DAMAGE_TO_AUTHORITY * (damage / 100.0f));
    }
}

void MissBatCrimsonAuthority::OnSpecialMoveUsed() {
    GenerateAuthority(AuthorityGauge::SPECIAL_MOVE);
}

void MissBatCrimsonAuthority::OnJump() {
    GenerateAuthority(AuthorityGauge::JUMP);
}

// ============================================================================
// Blood Puppet System
// ============================================================================

int MissBatCrimsonAuthority::GetMaxPuppets() const {
    if (isInUltimate) return 3;  // Permanent puppets during ultimate
    if (IsAuthorityEnhanced()) return 3;
    return 2;
}

void MissBatCrimsonAuthority::CreateBloodPuppet(float statMultiplier) {
    if (bloodPuppets.size() >= GetMaxPuppets()) {
        // Remove oldest puppet
        bloodPuppets.erase(bloodPuppets.begin());
    }

    auto puppet = std::make_shared<BloodPuppet>(statMultiplier);
    // TODO: Position puppet near Authority
    bloodPuppets.push_back(puppet);
}

void MissBatCrimsonAuthority::UpdateBloodPuppets(float deltaTime) {
    for (auto& puppet : bloodPuppets) {
        if (puppet && puppet->isAlive) {
            puppet->Update(deltaTime);
        }
    }

    // Remove dead puppets
    bloodPuppets.erase(
        std::remove_if(bloodPuppets.begin(), bloodPuppets.end(),
            [](const std::shared_ptr<BloodPuppet>& puppet) {
                return !puppet || !puppet->isAlive;
            }),
        bloodPuppets.end()
    );
}

void MissBatCrimsonAuthority::FusePuppetsToGolem() {
    if (bloodPuppets.size() >= 2) {
        // TODO: Create Blood Golem from 2 puppets
        // Golem has 100% of strongest puppet's stats
        bloodPuppets.clear();
    }
}

// ============================================================================
// Blood Construct System
// ============================================================================

int MissBatCrimsonAuthority::GetMaxConstructs() const {
    if (isInUltimate) return 12;
    if (IsAuthorityMaximum()) return 7;
    if (IsAuthorityEnhanced()) return 5;
    return 3;
}

void MissBatCrimsonAuthority::PlaceConstruct(ConstructType type, float x, float y, float z) {
    if (!bloodEssence.CanAfford(AbilityCosts::CONSTRUCT_ESSENCE_COST)) {
        return;
    }

    if (bloodConstructs.size() >= GetMaxConstructs()) {
        // Cannot place more
        return;
    }

    bloodEssence.Consume(AbilityCosts::CONSTRUCT_ESSENCE_COST);

    auto construct = std::make_shared<BloodConstruct>(type);
    construct->x = x;
    construct->y = y;
    construct->z = z;
    bloodConstructs.push_back(construct);
}

void MissBatCrimsonAuthority::UpdateConstructs(float deltaTime) {
    for (auto& construct : bloodConstructs) {
        if (construct) {
            construct->Update(deltaTime);
        }
    }

    // Check for Construct Resonance
    if (CheckConstructResonance()) {
        // TODO: Apply resonance field effects
    }
}

void MissBatCrimsonAuthority::SacrificeConstruct(int index) {
    if (index >= 0 && index < bloodConstructs.size()) {
        auto& construct = bloodConstructs[index];
        construct->Sacrifice();

        // Refund 3 Blood Essence
        bloodEssence.Generate(3);

        // Remove construct
        bloodConstructs.erase(bloodConstructs.begin() + index);
    }
}

bool MissBatCrimsonAuthority::CheckConstructResonance() {
    // Check if 2+ constructs within 15m create resonance field
    // TODO: Implement distance checking
    return bloodConstructs.size() >= 2;
}

// ============================================================================
// Blood Form Transformation
// ============================================================================

void MissBatCrimsonAuthority::TransformToForm(BloodForm form) {
    currentForm = form;
    formDuration = 15.0f;  // Base duration
    formMasteryPoints = 0.0f;
    isTranscendent = false;

    if (IsAuthorityEnhanced()) {
        formDuration = 20.0f;
    }

    // Apply form-specific effects
    switch (form) {
        case BloodForm::CrimsonReaver:
            // +25% movement speed, offensive bonuses
            // TODO: Apply buffs
            break;

        case BloodForm::SanguineFortress:
            // +50% max HP, defensive bonuses
            stats.maxHealth *= 1.5f;
            stats.health = std::min(stats.health, stats.maxHealth);
            break;

        case BloodForm::HemomagueWraith:
            // +40% movement speed, debuff application
            // TODO: Apply speed buff
            break;

        case BloodForm::VitaeSovereign:
            // Create support aura
            // TODO: Implement domain
            break;

        default:
            break;
    }
}

void MissBatCrimsonAuthority::UpdateTransformation(float deltaTime) {
    formDuration -= deltaTime;
    formMasteryPoints += deltaTime;

    // Check for Transcendent State
    float masteryThreshold = IsAuthorityEnhanced() ? 7.0f : 10.0f;
    if (!isTranscendent && formMasteryPoints >= masteryThreshold) {
        isTranscendent = true;
        // Apply transcendent bonuses
    }

    if (formDuration <= 0.0f) {
        EndTransformation();
    }
}

void MissBatCrimsonAuthority::EndTransformation() {
    // Remove form-specific effects
    if (currentForm == BloodForm::SanguineFortress) {
        // Restore original max HP
        stats.maxHealth = 220.0f;
        stats.health = std::min(stats.health, stats.maxHealth);
    }

    currentForm = BloodForm::None;
    formDuration = 0.0f;
    formMasteryPoints = 0.0f;
    isTranscendent = false;
}

void MissBatCrimsonAuthority::QuickSwapForm(BloodForm newForm) {
    int essenceCost = IsAuthorityMaximum() ? 0 :
                     (IsAuthorityEnhanced() ? AbilityCosts::FORM_SWAP_ENHANCED : AbilityCosts::FORM_SWAP_COST);

    if (bloodEssence.CanAfford(essenceCost)) {
        bloodEssence.Consume(essenceCost);

        // Keep remaining duration
        float remainingDuration = formDuration;

        EndTransformation();
        TransformToForm(newForm);

        formDuration = remainingDuration;
    }
}

// ============================================================================
// Crimson Execution System
// ============================================================================

bool MissBatCrimsonAuthority::CanExecuteTarget(int targetId) const {
    // TODO: Check target HP percentage against various thresholds
    // Different conditions enable execution at different HP percentages
    return false;  // Placeholder
}

void MissBatCrimsonAuthority::StartExecution(int targetId) {
    if (!CanExecuteTarget(targetId)) return;

    isExecuting = true;
    executionTargetId = targetId;
    executionCastTimer = isInUltimate ? UltimateEnhancements::EXECUTION_CAST_TIME : EXECUTION_CAST_TIME;

    // TODO: Play execution windup animation
}

void MissBatCrimsonAuthority::UpdateExecution(float deltaTime) {
    executionCastTimer -= deltaTime;

    if (executionCastTimer <= 0.0f) {
        CompleteExecution();
    }

    // TODO: Check for interruption
}

void MissBatCrimsonAuthority::CompleteExecution() {
    isExecuting = false;

    // Instant kill target
    // TODO: Kill target

    // Rewards
    authorityGauge.Generate(AuthorityGauge::EXECUTION);
    stats.health = std::min(stats.health + 200.0f, stats.maxHealth);
    bloodResonance.AddPermanentStack();

    // Ultimate extension during ultimate
    if (isInUltimate) {
        ultimateTimeRemaining += UltimateEnhancements::EXECUTION_DURATION_EXTENSION;
    }

    // TODO: Check for execution streak bonuses
}

void MissBatCrimsonAuthority::InterruptExecution() {
    isExecuting = false;
    executionTargetId = -1;

    // Refund 20 Authority
    authorityGauge.Generate(20.0f);

    // 15 second cooldown
    // TODO: Apply execution cooldown
}

// ============================================================================
// Ultimate - Sovereign of Blood
// ============================================================================

void MissBatCrimsonAuthority::ActivateUltimate() {
    if (authorityGauge.current < 100.0f) return;

    isInUltimate = true;
    ultimateTimeRemaining = ULTIMATE_DURATION;
    authorityGauge.Consume(100.0f);

    // Instant effects
    bloodResonance.current = UltimateEnhancements::INSTANT_RESONANCE;
    bloodResonance.maximum = UltimateEnhancements::MAX_RESONANCE_ULTIMATE;

    // Apply all stat bonuses
    stats.speed *= (1.0f + UltimateEnhancements::MOVEMENT_SPEED_BONUS);

    // Transform to all 4 forms simultaneously (quad-state)
    // TODO: Implement multi-form state

    // Make all constructs invulnerable
    for (auto& construct : bloodConstructs) {
        if (construct) {
            // TODO: Set invulnerable flag
        }
    }

    // Make puppets permanent
    for (auto& puppet : bloodPuppets) {
        if (puppet) {
            // TODO: Set permanent flag
        }
    }

    // TODO: Play ultimate activation VFX
}

void MissBatCrimsonAuthority::UpdateUltimate(float deltaTime) {
    ultimateTimeRemaining -= deltaTime;

    // Passive Authority generation
    authorityGauge.Generate(UltimateEnhancements::PASSIVE_AUTHORITY_REGEN * deltaTime);

    // Blood Field Aura effects
    // TODO: Apply aura effects to nearby allies/enemies

    if (ultimateTimeRemaining <= 0.0f) {
        EndUltimate();
    }
}

void MissBatCrimsonAuthority::EndUltimate() {
    isInUltimate = false;

    // Remove stat bonuses
    stats.speed /= (1.0f + UltimateEnhancements::MOVEMENT_SPEED_BONUS);

    // Reset resonance maximum
    bloodResonance.maximum = 20;
    bloodResonance.current = std::min(bloodResonance.current, 10);  // Retain 10 stacks

    // Enter recovery state
    ultimateRecovery = true;
    ultimateRecoveryTimer = ULTIMATE_RECOVERY_TIME;

    // TODO: Remove multi-form state
    // TODO: Remove invulnerability from constructs
}

void MissBatCrimsonAuthority::CrimsonCataclysm() {
    if (!isInUltimate) return;

    // Consume remaining ultimate duration
    float baseDamage = 200.0f;
    float bonusDamage = 50.0f * ultimateTimeRemaining;
    float totalDamage = baseDamage + bonusDamage;

    // Massive blood explosion
    // TODO: Create 20m radius explosion
    // TODO: Deal damage to all enemies
    // TODO: Execute enemies below 30% HP after explosion

    // End ultimate immediately
    EndUltimate();
}

// ============================================================================
// Combat Overrides
// ============================================================================

void MissBatCrimsonAuthority::ExecuteSpecialMove(Direction direction) {
    // TODO: Implement directional special moves
    // S+Up, S+Down, etc.
}

void MissBatCrimsonAuthority::ExecuteGearSkill(int index) {
    // TODO: Implement gear skills
    // SD (Weapon), AD (Helmet), ASD (Armor), AS (Cloak)
}

void MissBatCrimsonAuthority::Block() {
    CharacterBase::Block();
    // TODO: Implement Blood Authority-specific block
}

bool MissBatCrimsonAuthority::CanUseSpecialMoves() const {
    return !isExecuting && !ultimateRecovery;
}

// ============================================================================
// Base Kit Abilities (Placeholder implementations)
// ============================================================================

void MissBatCrimsonAuthority::BloodSpearVolley(bool charged) {
    float cost = charged ? AbilityCosts::BLOOD_SPEAR_CHARGED : AbilityCosts::BLOOD_SPEAR_VOLLEY;
    if (!authorityGauge.CanAfford(cost)) return;

    authorityGauge.Consume(cost);
    OnSpecialMoveUsed();

    // TODO: Launch blood spears
}

void MissBatCrimsonAuthority::CrimsonStep() {
    GenerateAuthority(3.0f);
    // TODO: Dash with invulnerability frames
}

void MissBatCrimsonAuthority::CrimsonArsenal() {
    if (!authorityGauge.CanAfford(AbilityCosts::CRIMSON_ARSENAL)) return;

    authorityGauge.Consume(AbilityCosts::CRIMSON_ARSENAL);
    OnSpecialMoveUsed();

    // TODO: Summon 4 floating blood weapons
}

void MissBatCrimsonAuthority::BloodSurge() {
    if (!authorityGauge.CanAfford(AbilityCosts::BLOOD_SURGE)) return;

    authorityGauge.Consume(AbilityCosts::BLOOD_SURGE);
    OnSpecialMoveUsed();

    // TODO: Dash with blood wave trail
}

void MissBatCrimsonAuthority::RisingCrimson() {
    if (!authorityGauge.CanAfford(AbilityCosts::RISING_CRIMSON)) return;

    authorityGauge.Consume(AbilityCosts::RISING_CRIMSON);
    OnSpecialMoveUsed();
    GenerateAuthority(5.0f);

    // TODO: Upward launcher
}

void MissBatCrimsonAuthority::CrimsonDescent() {
    if (!authorityGauge.CanAfford(AbilityCosts::CRIMSON_DESCENT)) return;

    authorityGauge.Consume(AbilityCosts::CRIMSON_DESCENT);
    OnSpecialMoveUsed();

    // TODO: Ground slam with blood pool
}

void MissBatCrimsonAuthority::BloodGeyser() {
    if (!authorityGauge.CanAfford(AbilityCosts::BLOOD_GEYSER)) return;

    authorityGauge.Consume(AbilityCosts::BLOOD_GEYSER);
    OnSpecialMoveUsed();

    // TODO: Create blood geyser at location
}

void MissBatCrimsonAuthority::BloodSpikeTrap() {
    if (!authorityGauge.CanAfford(AbilityCosts::BLOOD_SPIKE_TRAP)) return;

    authorityGauge.Consume(AbilityCosts::BLOOD_SPIKE_TRAP);
    OnSpecialMoveUsed();

    // TODO: Place trap
}

void MissBatCrimsonAuthority::HemorrhageJavelin(float chargeTime) {
    if (!authorityGauge.CanAfford(AbilityCosts::HEMORRHAGE_JAVELIN)) return;

    authorityGauge.Consume(AbilityCosts::HEMORRHAGE_JAVELIN);
    OnSpecialMoveUsed();

    // TODO: Launch javelin based on charge time
    // Applies Hemorrhage debuff for execution setup
}

void MissBatCrimsonAuthority::BloodEchoStrike() {
    if (!authorityGauge.CanAfford(AbilityCosts::BLOOD_ECHO_STRIKE)) return;

    authorityGauge.Consume(AbilityCosts::BLOOD_ECHO_STRIKE);
    OnSpecialMoveUsed();

    // TODO: Teleport with afterimage explosion
}

// ============================================================================
// Gear System Methods (Placeholder)
// ============================================================================

void MissBatCrimsonAuthority::StealGearSkill() {
    // TODO: Scan enemies, steal random gear skill
}

void MissBatCrimsonAuthority::UseStolenSkill(int index) {
    // TODO: Use corrupted version of stolen skill
}

// ============================================================================
// Helper Methods
// ============================================================================

float MissBatCrimsonAuthority::GetAuthorityScaledMultiplier() const {
    if (IsAuthorityMaximum()) return 1.5f;
    if (IsAuthorityEnhanced()) return 1.25f;
    return 1.0f;
}

float MissBatCrimsonAuthority::GetDamageMultiplier() const {
    float multiplier = 1.0f;

    // Blood Resonance bonus
    multiplier += bloodResonance.GetDamageBonus();

    // Ultimate bonus
    if (isInUltimate) {
        multiplier += UltimateEnhancements::DAMAGE_BONUS;
    }

    return multiplier;
}

} // namespace ArenaFighter
