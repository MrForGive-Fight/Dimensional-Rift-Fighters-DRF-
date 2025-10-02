#include "HyoudouKotetsu.h"
#include "../../Combat/DamageCalculator.h"
#include <algorithm>
#include <cmath>

namespace ArenaFighter {

// ============================================================================
// StolenPantheonGauge Implementation
// ============================================================================

void StolenPantheonGauge::Generate(float amount) {
    current = std::min(current + amount, maximum);
}

bool StolenPantheonGauge::CanAfford(float cost) const {
    return current >= cost;
}

void StolenPantheonGauge::Consume(float cost) {
    current = std::max(0.0f, current - cost);
}

void StolenPantheonGauge::Update(float deltaTime) {
    // Passive regeneration
    Generate(PASSIVE_REGEN * deltaTime);
}

void StolenPantheonGauge::Reset() {
    current = 0.0f;
}

// ============================================================================
// GodClone Base Implementation
// ============================================================================

GodClone::GodClone(GodType type)
    : type(type)
    , isAlive(true)
    , x(0.0f)
    , y(0.0f)
    , z(0.0f) {
}

// ============================================================================
// Vulcanus Clone - Fire Titan
// ============================================================================

VulcanusClone::VulcanusClone() : GodClone(GodType::Vulcanus) {
    maxHealth = 120.0f;
    health = 120.0f;
    damage[0] = 14.0f;  // Basic hammer
    damage[1] = 16.0f;  // Forge strike
    damage[2] = 20.0f;  // Slam
    damage[3] = 0.0f;
    speedMultiplier = 0.9f;  // Slow but powerful
}

void VulcanusClone::UpdateAI(float deltaTime, float playerX, float playerY, float playerZ) {
    if (!isAlive) return;

    attackTimer += deltaTime;
    slamTimer += deltaTime;
    shieldTimer += deltaTime;

    // Aggressive tank behavior - rushes enemies
    // TODO: Movement logic to engage closest enemy

    // Regular attacks every 1.5 seconds
    if (attackTimer >= 1.5f) {
        Attack();
        attackTimer = 0.0f;
    }

    // Ground slam every 5 seconds
    if (slamTimer >= 5.0f) {
        HammerSlam();
        slamTimer = 0.0f;
    }

    // Molten shield when HP below 50%
    if (!isShielded && health < maxHealth * 0.5f && shieldTimer >= 8.0f) {
        MoltenShield();
        shieldTimer = 0.0f;
    }
}

void VulcanusClone::Attack() {
    // Basic hammer swings
    // TODO: Implement damage dealing
}

void VulcanusClone::OnDeath() {
    isAlive = false;
    // Explodes for fire damage
    // TODO: Create explosion effect
}

void VulcanusClone::HammerSlam() {
    // AOE ground pound (20 damage)
    // TODO: Implement area attack
}

void VulcanusClone::ForgeStrike() {
    // Heavy single target (16 damage)
    // TODO: Implement
}

void VulcanusClone::MoltenShield() {
    isShielded = true;
    // 3 seconds of invulnerability
    // TODO: Add timed buff system
}

// ============================================================================
// Mercurius Clone - Swift Thief
// ============================================================================

MercuriusClone::MercuriusClone() : GodClone(GodType::Mercurius) {
    maxHealth = 80.0f;
    health = 80.0f;
    damage[0] = 10.0f;  // Quick slash
    damage[1] = 11.0f;  // Combo hit
    damage[2] = 13.0f;  // Blitz strike
    damage[3] = 0.0f;
    speedMultiplier = 1.5f;  // Very fast
}

void MercuriusClone::UpdateAI(float deltaTime, float playerX, float playerY, float playerZ) {
    if (!isAlive) return;

    attackTimer += deltaTime;
    stealTimer += deltaTime;
    dodgeTimer += deltaTime;

    // Hit-and-run tactics with high mobility
    // TODO: Implement dash movement

    // Fast attack speed
    if (attackTimer >= 0.8f) {
        Attack();
        attackTimer = 0.0f;
        consecutiveHits++;
    }

    // Steal buffs every 6 seconds
    if (stealTimer >= 6.0f) {
        StealBuff();
        stealTimer = 0.0f;
    }

    // Dodge on danger detection
    if (dodgeTimer >= 2.0f && /* danger detected */ false) {
        WindStep();
        dodgeTimer = 0.0f;
    }
}

void MercuriusClone::Attack() {
    // Rapid slashes
    // TODO: Implement combo system
}

void MercuriusClone::OnDeath() {
    isAlive = false;
}

void MercuriusClone::BlitzStrike() {
    // Dash attack (13 damage)
    // TODO: Implement
}

void MercuriusClone::StealBuff() {
    // Removes enemy buffs and applies to self
    // TODO: Implement buff system
}

void MercuriusClone::WindStep() {
    // Teleport dodge
    // TODO: Implement
}

// ============================================================================
// Diana Clone - Moonlight Huntress
// ============================================================================

DianaClone::DianaClone() : GodClone(GodType::Diana) {
    maxHealth = 70.0f;
    health = 70.0f;
    damage[0] = 12.0f;  // Arrow
    damage[1] = 14.0f;  // Charged shot
    damage[2] = 16.0f;  // Curse shot
    damage[3] = 0.0f;
    speedMultiplier = 1.2f;  // Mobile ranged
}

void DianaClone::UpdateAI(float deltaTime, float playerX, float playerY, float playerZ) {
    if (!isAlive) return;

    attackTimer += deltaTime;
    curseTimer += deltaTime;
    markTimer += deltaTime;

    // Kiting behavior - maintain distance
    // TODO: Movement logic to stay away from enemies

    // Regular arrows every 1.2 seconds
    if (attackTimer >= 1.2f) {
        Attack();
        attackTimer = 0.0f;
    }

    // Curse shot every 7 seconds
    if (curseTimer >= 7.0f) {
        CurseShot();
        curseTimer = 0.0f;
    }

    // Hunter's mark every 10 seconds
    if (markTimer >= 10.0f) {
        HuntersMark();
        markTimer = 0.0f;
    }
}

void DianaClone::Attack() {
    // Moonlight arrows (12 damage)
    // TODO: Implement projectile
}

void DianaClone::OnDeath() {
    isAlive = false;
}

void DianaClone::MoonlightArrow() {
    // Basic ranged attack
    // TODO: Implement
}

void DianaClone::CurseShot() {
    // Debuff arrow (16 damage + slow)
    // TODO: Implement debuff system
}

void DianaClone::HuntersMark() {
    // Marks enemy for +30% damage from all sources
    // TODO: Implement marking system
}

// ============================================================================
// Hyoudou Kotetsu Constructor
// ============================================================================

HyoudouKotetsu::HyoudouKotetsu() {
    InitializeHyoudouStats();
    SetupBaseGearSkills();
}

void HyoudouKotetsu::InitializeHyoudouStats() {
    // S-Tier Stats - Balanced Divine Thief
    stats.maxHealth = 250.0f;
    stats.health = 250.0f;
    stats.attack = 110.0f;      // High base damage
    stats.defense = 85.0f;      // Moderate defense
    stats.speed = 105.0f;       // Above average speed
    stats.maxMana = 100.0f;
    stats.mana = 100.0f;
    stats.manaRegen = 5.0f;

    // Pantheon gauge starts at 0
    pantheonGauge.current = 0.0f;
    pantheonGauge.maximum = 100.0f;
}

void HyoudouKotetsu::SetupBaseGearSkills() {
    // Gear skills change based on corruption form
    // Base form focuses on divine theft mechanics
}

// ============================================================================
// Core Update Loop
// ============================================================================

void HyoudouKotetsu::Update(float deltaTime) {
    CharacterBase::Update(deltaTime);

    // Update pantheon gauge
    pantheonGauge.Update(deltaTime);

    // Update corruption timer
    if (IsCorrupted()) {
        UpdateCorruption(deltaTime);
    }

    // Update god clones (Pluto form only)
    if (currentForm == CorruptionForm::CorruptedPluto) {
        UpdateGodClones(deltaTime);
    }

    // Check emergency protocol
    if (!pantheonEndUsed) {
        CheckPantheonEnd();
    }
}

// ============================================================================
// Pantheon Gauge Generation
// ============================================================================

void HyoudouKotetsu::GeneratePantheonPower(float amount) {
    pantheonGauge.Generate(amount);
}

void HyoudouKotetsu::OnBasicAttackHit() {
    GeneratePantheonPower(StolenPantheonGauge::ON_BASIC_HIT);
}

void HyoudouKotetsu::OnDivineTheftHit() {
    GeneratePantheonPower(StolenPantheonGauge::ON_DIVINE_THEFT);
}

void HyoudouKotetsu::OnDamageTaken(float damage) {
    GeneratePantheonPower(damage * StolenPantheonGauge::ON_DAMAGE_TAKEN);
}

void HyoudouKotetsu::OnKill() {
    GeneratePantheonPower(StolenPantheonGauge::ON_KILL);
}

// ============================================================================
// Corruption Transformation System
// ============================================================================

bool HyoudouKotetsu::TransformToVulcanus() {
    if (!pantheonGauge.CanAfford(StolenPantheonGauge::VULCANUS_COST)) {
        return false;
    }

    pantheonGauge.Consume(StolenPantheonGauge::VULCANUS_COST);
    currentForm = CorruptionForm::Vulcanus;
    corruptionTimeRemaining = VULCANUS_DURATION;
    vulcanusStackCount = 0;

    // Stat modifications: +30% attack, +20% defense, -10% speed
    stats.attack *= 1.3f;
    stats.defense *= 1.2f;
    stats.speed *= 0.9f;

    return true;
}

bool HyoudouKotetsu::TransformToMercurius() {
    if (!pantheonGauge.CanAfford(StolenPantheonGauge::MERCURIUS_COST)) {
        return false;
    }

    pantheonGauge.Consume(StolenPantheonGauge::MERCURIUS_COST);
    currentForm = CorruptionForm::Mercurius;
    corruptionTimeRemaining = MERCURIUS_DURATION;
    mercuriusStolenBuffs = 0;

    // Stat modifications: +50% speed, +15% attack, -20% defense
    stats.speed *= 1.5f;
    stats.attack *= 1.15f;
    stats.defense *= 0.8f;

    return true;
}

bool HyoudouKotetsu::TransformToDiana() {
    if (!pantheonGauge.CanAfford(StolenPantheonGauge::DIANA_COST)) {
        return false;
    }

    pantheonGauge.Consume(StolenPantheonGauge::DIANA_COST);
    currentForm = CorruptionForm::Diana;
    corruptionTimeRemaining = DIANA_DURATION;
    dianaMarkedEnemies = 0;

    // Stat modifications: +25% attack, +30% speed, unchanged defense
    stats.attack *= 1.25f;
    stats.speed *= 1.3f;

    return true;
}

bool HyoudouKotetsu::TransformToCorruptedPluto() {
    if (!pantheonGauge.CanAfford(StolenPantheonGauge::PLUTO_COST)) {
        return false;
    }

    pantheonGauge.Consume(StolenPantheonGauge::PLUTO_COST);
    currentForm = CorruptionForm::CorruptedPluto;
    corruptionTimeRemaining = PLUTO_DURATION;

    // Massive stat boost
    stats.attack *= 1.5f;
    stats.defense *= 1.3f;
    stats.speed *= 1.4f;

    // Summon god clones
    SummonGodClones();

    return true;
}

void HyoudouKotetsu::EndCorruption() {
    // Dismiss clones if active
    if (currentForm == CorruptionForm::CorruptedPluto) {
        DismissGodClones();
    }

    currentForm = CorruptionForm::None;
    corruptionTimeRemaining = 0.0f;

    // Reset to base stats
    InitializeHyoudouStats();
}

void HyoudouKotetsu::UpdateCorruption(float deltaTime) {
    corruptionTimeRemaining -= deltaTime;

    if (corruptionTimeRemaining <= 0.0f) {
        EndCorruption();
    }
}

// ============================================================================
// God Clone Management
// ============================================================================

void HyoudouKotetsu::SummonGodClones() {
    godClones.clear();

    // Summon all 3 god clones
    godClones.push_back(std::make_shared<VulcanusClone>());
    godClones.push_back(std::make_shared<MercuriusClone>());
    godClones.push_back(std::make_shared<DianaClone>());

    // Position them around Hyoudou
    // TODO: Set spawn positions
}

void HyoudouKotetsu::UpdateGodClones(float deltaTime) {
    for (auto& clone : godClones) {
        if (clone && clone->IsAlive()) {
            clone->UpdateAI(deltaTime, x, y, z);
        }
    }

    // Remove dead clones
    godClones.erase(
        std::remove_if(godClones.begin(), godClones.end(),
            [](const std::shared_ptr<GodClone>& clone) {
                return !clone || !clone->IsAlive();
            }),
        godClones.end()
    );
}

void HyoudouKotetsu::DismissGodClones() {
    godClones.clear();
}

// ============================================================================
// Emergency Protocol - Pantheon's End
// ============================================================================

void HyoudouKotetsu::CheckPantheonEnd() {
    float healthPercent = stats.health / stats.maxHealth;

    if (healthPercent <= 0.25f && !pantheonEndUsed) {
        TriggerPantheonEnd();
    }
}

void HyoudouKotetsu::TriggerPantheonEnd() {
    pantheonEndUsed = true;

    // Force Pluto transformation regardless of gauge
    pantheonGauge.current = pantheonGauge.maximum;
    currentForm = CorruptionForm::CorruptedPluto;
    corruptionTimeRemaining = PANTHEON_END_DURATION;  // Extended duration

    // Massive stat boost (even higher than normal Pluto)
    stats.attack *= 1.8f;
    stats.defense *= 1.5f;
    stats.speed *= 1.6f;

    // Heal 50% HP
    stats.health += stats.maxHealth * 0.5f;
    stats.health = std::min(stats.health, stats.maxHealth);

    // Summon enhanced god clones
    SummonGodClones();

    // TODO: Add invulnerability frames during transformation
}

// ============================================================================
// Combat Overrides
// ============================================================================

void HyoudouKotetsu::ExecuteSpecialMove(Direction direction) {
    if (!CanUseSpecialMoves()) {
        return;
    }

    // Route to appropriate form
    switch (currentForm) {
        case CorruptionForm::None:
            BaseDivineTheftAbilities(direction);
            break;
        case CorruptionForm::Vulcanus:
            VulcanusAbilities(direction);
            break;
        case CorruptionForm::Mercurius:
            MercuriusAbilities(direction);
            break;
        case CorruptionForm::Diana:
            DianaAbilities(direction);
            break;
        case CorruptionForm::CorruptedPluto:
            PlutoAbilities(direction);
            break;
    }
}

void HyoudouKotetsu::ExecuteGearSkill(int index) {
    switch (currentForm) {
        case CorruptionForm::None:
            BaseDivineTheftGearSkills(index);
            break;
        case CorruptionForm::Vulcanus:
            VulcanusGearSkills(index);
            break;
        case CorruptionForm::Mercurius:
            MercuriusGearSkills(index);
            break;
        case CorruptionForm::Diana:
            DianaGearSkills(index);
            break;
        case CorruptionForm::CorruptedPluto:
            PlutoGearSkills(index);
            break;
    }
}

void HyoudouKotetsu::Block() {
    switch (currentForm) {
        case CorruptionForm::None:
            BaseDivineTheftBlock();
            break;
        case CorruptionForm::Vulcanus:
            VulcanusBlock();
            break;
        case CorruptionForm::Mercurius:
            MercuriusBlock();
            break;
        case CorruptionForm::Diana:
            DianaBlock();
            break;
        case CorruptionForm::CorruptedPluto:
            PlutoBlock();
            break;
    }
}

bool HyoudouKotetsu::CanUseSpecialMoves() const {
    return true;  // Hyoudou always has special moves
}

// ============================================================================
// Base Form - Divine Thief Abilities
// ============================================================================

void HyoudouKotetsu::BaseDivineTheftAbilities(Direction direction) {
    switch (direction) {
        case Direction::Up:
            // Ascending Theft (14 damage, launches enemy, steals buff)
            // TODO: Implement
            break;
        case Direction::Down:
            // Divine Snatch (15 damage ground slam, AOE theft)
            // TODO: Implement
            break;
        case Direction::Left:
            // Pantheon Surge (12 damage dash, generates +5 gauge)
            // TODO: Implement
            break;
        case Direction::Right:
            // God Breaker (18 damage heavy strike, breaks guard)
            // TODO: Implement
            break;
        default:
            break;
    }
}

void HyoudouKotetsu::BaseDivineTheftBlock() {
    CharacterBase::Block();
    // Divine Guard - reflects 20% of blocked damage
    // TODO: Add reflection mechanic
}

void HyoudouKotetsu::BaseDivineTheftGearSkills(int index) {
    // TODO: Implement base form gear skills
}

// ============================================================================
// Form-Specific Abilities (Placeholders)
// ============================================================================

void HyoudouKotetsu::VulcanusAbilities(Direction direction) { /* TODO */ }
void HyoudouKotetsu::VulcanusBlock() { CharacterBase::Block(); }
void HyoudouKotetsu::VulcanusGearSkills(int index) { /* TODO */ }

void HyoudouKotetsu::MercuriusAbilities(Direction direction) { /* TODO */ }
void HyoudouKotetsu::MercuriusBlock() { CharacterBase::Block(); }
void HyoudouKotetsu::MercuriusGearSkills(int index) { /* TODO */ }

void HyoudouKotetsu::DianaAbilities(Direction direction) { /* TODO */ }
void HyoudouKotetsu::DianaBlock() { CharacterBase::Block(); }
void HyoudouKotetsu::DianaGearSkills(int index) { /* TODO */ }

void HyoudouKotetsu::PlutoAbilities(Direction direction) { /* TODO */ }
void HyoudouKotetsu::PlutoBlock() { CharacterBase::Block(); }
void HyoudouKotetsu::PlutoGearSkills(int index) { /* TODO */ }

} // namespace ArenaFighter
