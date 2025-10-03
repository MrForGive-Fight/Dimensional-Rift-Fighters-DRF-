#include "GobTheGoodGoblin.h"
#include "../../Combat/DamageCalculator.h"
#include <algorithm>
#include <cmath>

namespace ArenaFighter {

// ============================================================================
// EvolutionGauge Implementation
// ============================================================================

void EvolutionGauge::Generate(float amount) {
    current = std::min(current + amount, maximum);
}

bool EvolutionGauge::CanAfford(float cost) const {
    return current >= cost;
}

void EvolutionGauge::Consume(float cost) {
    current = std::max(0.0f, current - cost);
}

void EvolutionGauge::Update(float deltaTime, bool isVajrayaksa) {
    // Vajrayaksa drains meter over time
    if (isVajrayaksa) {
        current -= VAJRAYAKSA_DRAIN_RATE * deltaTime;
        current = std::max(0.0f, current);
    }
}

void EvolutionGauge::ApplyDeathPenalty() {
    current *= (1.0f - DEATH_PENALTY);  // Lose 35% of current meter
}

// ============================================================================
// GobTheGoodGoblin Constructor
// ============================================================================

GobTheGoodGoblin::GobTheGoodGoblin() {
    InitializeGobStats();
    TransformToGoblin();  // Start in Goblin form
}

void GobTheGoodGoblin::InitializeGobStats() {
    // Base stats (before form modifiers)
    baseStats.maxHealth = 200.0f;
    baseStats.attack = 100.0f;
    baseStats.defense = 80.0f;
    baseStats.speed = 100.0f;

    stats.maxMana = 100.0f;
    stats.mana = 100.0f;
    stats.manaRegen = 5.0f;

    // Evolution gauge starts at 0
    evolutionGauge.current = 0.0f;
    evolutionGauge.maximum = 100.0f;
}

// ============================================================================
// Core Update Loop
// ============================================================================

void GobTheGoodGoblin::Update(float deltaTime) {
    CharacterBase::Update(deltaTime);

    // Update evolution gauge (with Vajrayaksa drain if applicable)
    if (!vajrayaksaMeterDrainPaused) {
        evolutionGauge.Update(deltaTime, currentForm == EvolutionForm::Vajrayaksa);
    }

    // Handle evolution animation
    if (isInEvolutionAnimation) {
        evolutionAnimationTimer += deltaTime;
        if (evolutionAnimationTimer >= EVOLUTION_ANIMATION_DURATION) {
            isInEvolutionAnimation = false;
            evolutionAnimationTimer = 0.0f;
        }
        return;  // Vulnerable during evolution
    }

    // Check for evolution
    CheckEvolution();

    // Check emergency protocol
    if (!emergencyProtocolUsed) {
        CheckEmergencyProtocol();
    }
}

// ============================================================================
// Evolution Gauge Generation
// ============================================================================

void GobTheGoodGoblin::GenerateEvolutionEnergy(float amount) {
    evolutionGauge.Generate(amount);
}

void GobTheGoodGoblin::OnBasicAttackHit() {
    GenerateEvolutionEnergy(EvolutionGauge::BASIC_HIT);
}

void GobTheGoodGoblin::OnDamageTaken(float damage) {
    float meterGain = (damage / 10.0f) * EvolutionGauge::DAMAGE_TAKEN;
    GenerateEvolutionEnergy(meterGain);
}

void GobTheGoodGoblin::OnKill() {
    GenerateEvolutionEnergy(EvolutionGauge::ON_KILL);
}

void GobTheGoodGoblin::OnEquipmentPickup() {
    GenerateEvolutionEnergy(EvolutionGauge::EQUIPMENT_PICKUP);
}

void GobTheGoodGoblin::OnDeath() {
    evolutionGauge.ApplyDeathPenalty();  // -35% current meter
}

// ============================================================================
// Evolution System
// ============================================================================

void GobTheGoodGoblin::CheckEvolution() {
    EvolutionForm targetForm = DetermineFormFromGauge();

    if (targetForm != currentForm) {
        EvolveToForm(targetForm);
    }
}

EvolutionForm GobTheGoodGoblin::DetermineFormFromGauge() const {
    float percentage = evolutionGauge.GetPercentage();

    if (percentage >= 100.0f) {
        return EvolutionForm::Vajrayaksa;
    } else if (percentage >= 75.0f) {
        return EvolutionForm::ApostleLord;
    } else if (percentage >= 50.0f) {
        return EvolutionForm::Ogre;
    } else if (percentage >= 25.0f) {
        return EvolutionForm::Hobgoblin;
    } else {
        return EvolutionForm::Goblin;
    }
}

void GobTheGoodGoblin::EvolveToForm(EvolutionForm newForm) {
    if (currentForm == newForm) return;

    // Start evolution animation (vulnerable for 2 seconds)
    isInEvolutionAnimation = true;
    evolutionAnimationTimer = 0.0f;

    // TODO: Play evolution VFX and sound

    // Transform to new form
    switch (newForm) {
        case EvolutionForm::Goblin:
            TransformToGoblin();
            break;
        case EvolutionForm::Hobgoblin:
            TransformToHobgoblin();
            break;
        case EvolutionForm::Ogre:
            TransformToOgre();
            break;
        case EvolutionForm::ApostleLord:
            TransformToApostleLord();
            break;
        case EvolutionForm::Vajrayaksa:
            TransformToVajrayaksa();
            break;
    }

    currentForm = newForm;
}

// ============================================================================
// Form Transformations
// ============================================================================

void GobTheGoodGoblin::TransformToGoblin() {
    stats.maxHealth = FormStatModifiers::Goblin::HP;
    stats.health = std::min(stats.health, stats.maxHealth);
    ApplyFormStatModifications();
}

void GobTheGoodGoblin::TransformToHobgoblin() {
    stats.maxHealth = FormStatModifiers::Hobgoblin::HP;
    stats.health = std::min(stats.health, stats.maxHealth);
    ApplyFormStatModifications();
}

void GobTheGoodGoblin::TransformToOgre() {
    stats.maxHealth = FormStatModifiers::Ogre::HP;
    stats.health = std::min(stats.health, stats.maxHealth);
    ApplyFormStatModifications();
    vulcanusForgeStacks = 0;
}

void GobTheGoodGoblin::TransformToApostleLord() {
    stats.maxHealth = FormStatModifiers::ApostleLord::HP;
    stats.health = std::min(stats.health, stats.maxHealth);
    ApplyFormStatModifications();
}

void GobTheGoodGoblin::TransformToVajrayaksa() {
    stats.maxHealth = FormStatModifiers::Vajrayaksa::HP;
    stats.health = std::min(stats.health, stats.maxHealth);
    ApplyFormStatModifications();

    // Vajrayaksa drains meter over time
    vajrayaksaMeterDrainPaused = false;
}

void GobTheGoodGoblin::ApplyFormStatModifications() {
    // Apply form-specific stat multipliers
    stats.attack = baseStats.attack * GetCurrentDamageMultiplier();
    stats.defense = baseStats.defense / GetCurrentDefenseMultiplier();  // Inverted for damage taken
    stats.speed = baseStats.speed * GetCurrentSpeedMultiplier();
}

// ============================================================================
// Emergency Protocol - Survival Evolution
// ============================================================================

void GobTheGoodGoblin::CheckEmergencyProtocol() {
    float healthPercent = stats.health / stats.maxHealth;

    if (healthPercent <= 0.30f && !emergencyProtocolUsed) {
        TriggerEmergencyEvolution();
    }
}

void GobTheGoodGoblin::TriggerEmergencyEvolution() {
    emergencyProtocolUsed = true;

    // Instant evolution to next form (no 2-second vulnerability)
    EvolutionForm nextForm = currentForm;
    switch (currentForm) {
        case EvolutionForm::Goblin:
            nextForm = EvolutionForm::Hobgoblin;
            break;
        case EvolutionForm::Hobgoblin:
            nextForm = EvolutionForm::Ogre;
            break;
        case EvolutionForm::Ogre:
            nextForm = EvolutionForm::ApostleLord;
            break;
        case EvolutionForm::ApostleLord:
            nextForm = EvolutionForm::Vajrayaksa;
            break;
        case EvolutionForm::Vajrayaksa:
            // Special: Full heal + meter drain stops
            stats.health = stats.maxHealth;
            vajrayaksaMeterDrainPaused = true;
            // Schedule resume after 10 seconds
            // TODO: Implement timed event system
            return;
    }

    // Instant transformation (skip animation)
    isInEvolutionAnimation = false;
    EvolveToForm(nextForm);

    // Healing burst: +15% HP
    stats.health += stats.maxHealth * 0.15f;
    stats.health = std::min(stats.health, stats.maxHealth);

    // Temporary 30% damage reduction for 3 seconds
    // TODO: Implement buff system
}

// ============================================================================
// Stat Multiplier Helpers
// ============================================================================

float GobTheGoodGoblin::GetCurrentDamageMultiplier() const {
    switch (currentForm) {
        case EvolutionForm::Goblin:
            return FormStatModifiers::Goblin::DAMAGE_DEALT;
        case EvolutionForm::Hobgoblin:
            return FormStatModifiers::Hobgoblin::DAMAGE_DEALT;
        case EvolutionForm::Ogre:
            return FormStatModifiers::Ogre::DAMAGE_DEALT;
        case EvolutionForm::ApostleLord:
            return FormStatModifiers::ApostleLord::DAMAGE_DEALT;
        case EvolutionForm::Vajrayaksa:
            return FormStatModifiers::Vajrayaksa::DAMAGE_DEALT;
    }
    return 1.0f;
}

float GobTheGoodGoblin::GetCurrentDefenseMultiplier() const {
    switch (currentForm) {
        case EvolutionForm::Goblin:
            return FormStatModifiers::Goblin::DAMAGE_TAKEN;
        case EvolutionForm::Hobgoblin:
            return FormStatModifiers::Hobgoblin::DAMAGE_TAKEN;
        case EvolutionForm::Ogre:
            return FormStatModifiers::Ogre::DAMAGE_TAKEN;
        case EvolutionForm::ApostleLord:
            return FormStatModifiers::ApostleLord::DAMAGE_TAKEN;
        case EvolutionForm::Vajrayaksa:
            return FormStatModifiers::Vajrayaksa::DAMAGE_TAKEN;
    }
    return 1.0f;
}

float GobTheGoodGoblin::GetCurrentSpeedMultiplier() const {
    switch (currentForm) {
        case EvolutionForm::Goblin:
            return FormStatModifiers::Goblin::SPEED;
        case EvolutionForm::Hobgoblin:
            return FormStatModifiers::Hobgoblin::SPEED;
        case EvolutionForm::Ogre:
            return FormStatModifiers::Ogre::SPEED;
        case EvolutionForm::ApostleLord:
            return FormStatModifiers::ApostleLord::SPEED;
        case EvolutionForm::Vajrayaksa:
            return FormStatModifiers::Vajrayaksa::SPEED;
    }
    return 1.0f;
}

float GobTheGoodGoblin::GetCurrentSizeMultiplier() const {
    switch (currentForm) {
        case EvolutionForm::Goblin:
            return FormStatModifiers::Goblin::SIZE;
        case EvolutionForm::Hobgoblin:
            return FormStatModifiers::Hobgoblin::SIZE;
        case EvolutionForm::Ogre:
            return FormStatModifiers::Ogre::SIZE;
        case EvolutionForm::ApostleLord:
            return FormStatModifiers::ApostleLord::SIZE;
        case EvolutionForm::Vajrayaksa:
            return FormStatModifiers::Vajrayaksa::SIZE;
    }
    return 1.0f;
}

// ============================================================================
// Combat Overrides
// ============================================================================

void GobTheGoodGoblin::ExecuteSpecialMove(Direction direction) {
    if (!CanUseSpecialMoves()) {
        return;
    }

    // Route to appropriate form
    switch (currentForm) {
        case EvolutionForm::Goblin:
            GoblinAbilities(direction);
            break;
        case EvolutionForm::Hobgoblin:
            HobgoblinAbilities(direction);
            break;
        case EvolutionForm::Ogre:
            OgreAbilities(direction);
            break;
        case EvolutionForm::ApostleLord:
            ApostleLordAbilities(direction);
            break;
        case EvolutionForm::Vajrayaksa:
            VajrayaksaAbilities(direction);
            break;
    }
}

void GobTheGoodGoblin::ExecuteGearSkill(int index) {
    switch (currentForm) {
        case EvolutionForm::Goblin:
            GoblinGearSkills(index);
            break;
        case EvolutionForm::Hobgoblin:
            HobgoblinGearSkills(index);
            break;
        case EvolutionForm::Ogre:
            OgreGearSkills(index);
            break;
        case EvolutionForm::ApostleLord:
            ApostleLordGearSkills(index);
            break;
        case EvolutionForm::Vajrayaksa:
            VajrayaksaGearSkills(index);
            break;
    }
}

void GobTheGoodGoblin::Block() {
    switch (currentForm) {
        case EvolutionForm::Goblin:
            GoblinBlock();
            break;
        case EvolutionForm::Hobgoblin:
            HobgoblinBlock();
            break;
        case EvolutionForm::Ogre:
            OgreBlock();
            break;
        case EvolutionForm::ApostleLord:
            ApostleLordBlock();
            break;
        case EvolutionForm::Vajrayaksa:
            VajrayaksaBlock();
            break;
    }
}

bool GobTheGoodGoblin::CanUseSpecialMoves() const {
    return !isInEvolutionAnimation;  // Can't use moves during evolution
}

// ============================================================================
// GOBLIN FORM ABILITIES (0-24%)
// ============================================================================

void GobTheGoodGoblin::GoblinAbilities(Direction direction) {
    switch (direction) {
        case Direction::Up:
            // Panic Jump: Quick vertical escape, invincible frames
            // TODO: Implement
            break;
        case Direction::Left:
            // Survival Bite: Counter stance → bite heal 30 HP
            // TODO: Implement
            break;
        case Direction::Right:
            // Goblin Rush: Fast roll through enemies, steals 5% meter
            GenerateEvolutionEnergy(5.0f);
            // TODO: Implement roll mechanics
            break;
        default:
            break;
    }
}

void GobTheGoodGoblin::GoblinBlock() {
    CharacterBase::Block();
    // Basic Guard: Standard block with fear animation
}

void GobTheGoodGoblin::GoblinGearSkills(int index) {
    switch (index) {
        case 0: // SD - Desperate Bite
            // Heals 30 HP + 7% meter
            stats.health = std::min(stats.health + 30.0f, stats.maxHealth);
            GenerateEvolutionEnergy(7.0f);
            // TODO: Implement bite attack
            break;
        case 1: // AS - Survival Instinct
            // 3s speed boost + 10% meter
            GenerateEvolutionEnergy(10.0f);
            // TODO: Implement speed buff
            break;
        case 2: // AD - Screech
            // Small frontal stun 0.5s
            // TODO: Implement stun
            break;
        case 3: // Space - Hungry
            // Small drain field, heals 40 HP over 2s
            // TODO: Implement drain field
            break;
    }
}

// ============================================================================
// HOBGOBLIN FORM ABILITIES (25-49%)
// ============================================================================

void GobTheGoodGoblin::HobgoblinAbilities(Direction direction) {
    switch (direction) {
        case Direction::Up:
            // Shadow Upper: Rising uppercut → air combo starter
            // TODO: Implement
            break;
        case Direction::Left:
            // Dark Counter: Counter stance → shadow explosion
            // TODO: Implement
            break;
        case Direction::Right:
            // Phantom Strike: Teleport behind enemy → backstab
            // TODO: Implement
            break;
        default:
            break;
    }
}

void GobTheGoodGoblin::HobgoblinBlock() {
    CharacterBase::Block();
    // Shadow Guard: 10% chance to phase through attacks
    // TODO: Implement phase mechanic
}

void GobTheGoodGoblin::HobgoblinGearSkills(int index) {
    switch (index) {
        case 0: // SD - Shadow Drain
            // Grab enemy, steal one buff + 12% meter
            GenerateEvolutionEnergy(12.0f);
            // TODO: Implement buff theft
            break;
        case 1: // AS - Shadow Evolution
            // Brief super armor + 20% meter
            GenerateEvolutionEnergy(20.0f);
            // TODO: Implement super armor
            break;
        case 2: // AD - Fear Aura
            // 360° fear for 1s
            // TODO: Implement fear
            break;
        case 3: // Space - Hunger Field
            // Medium drain + slow enemies 3s
            // TODO: Implement drain field
            break;
    }
}

// ============================================================================
// OGRE FORM ABILITIES (50-74%)
// ============================================================================

void GobTheGoodGoblin::OgreAbilities(Direction direction) {
    switch (direction) {
        case Direction::Up:
            // Ogre Slam: Jump → crash with shockwave, breaks guard
            // TODO: Implement
            break;
        case Direction::Left:
            // Ground Quake: Stomp creating earth spikes forward
            // TODO: Implement
            break;
        case Direction::Right:
            // Brutal Charge: Armored rush grabbing first enemy
            // TODO: Implement
            break;
        default:
            break;
    }
}

void GobTheGoodGoblin::OgreBlock() {
    CharacterBase::Block();
    // Brutal Guard: Super armor while blocking
    // TODO: Implement super armor block
}

void GobTheGoodGoblin::OgreGearSkills(int index) {
    switch (index) {
        case 0: // SD - Crushing Grasp
            // Unblockable grab, copies enemy skill + 18% meter
            GenerateEvolutionEnergy(18.0f);
            // TODO: Implement skill copy
            break;
        case 1: // AS - Berserker Mode
            // Uninterruptible attacks + 25% meter
            GenerateEvolutionEnergy(25.0f);
            // TODO: Implement berserker state
            break;
        case 2: // AD - Intimidating Roar
            // AoE fear 1.5s + enemies drop equipment
            // TODO: Implement equipment drop
            break;
        case 3: // Space - Gluttony Vortex
            // Large pull + HP drain 4s
            // TODO: Implement vortex
            break;
    }
}

// ============================================================================
// APOSTLE LORD FORM ABILITIES (75-99%)
// ============================================================================

void GobTheGoodGoblin::ApostleLordAbilities(Direction direction) {
    switch (direction) {
        case Direction::Up:
            // Demon Ascension: Fly up → rain 5 demon orbs
            // TODO: Implement
            break;
        case Direction::Left:
            // Lord's Territory: Create demon field buffing allies
            // TODO: Implement
            break;
        case Direction::Right:
            // Orb Barrage: Fire 3 homing demon orbs
            // TODO: Implement
            break;
        default:
            break;
    }
}

void GobTheGoodGoblin::ApostleLordBlock() {
    CharacterBase::Block();
    // Demon Guard: Reflects projectiles as demon orbs
    // TODO: Implement projectile reflection
}

void GobTheGoodGoblin::ApostleLordGearSkills(int index) {
    switch (index) {
        case 0: // SD - Soul Devour
            // Mid-range vacuum, steals equipped skill + 22% meter
            GenerateEvolutionEnergy(22.0f);
            // TODO: Implement skill steal
            break;
        case 1: // AS - Demon Form
            // Manifest 4 arms for 5s + 30% meter
            GenerateEvolutionEnergy(30.0f);
            // TODO: Implement four-arm mode
            break;
        case 2: // AD - Demon Command
            // Team gets 20% damage boost 8s
            // TODO: Implement team buff
            break;
        case 3: // Space - Soul Feast
            // Steal meter from all nearby enemies 5s
            // TODO: Implement meter drain
            break;
    }
}

// ============================================================================
// VAJRAYAKSA OVERLORD FORM ABILITIES (100%)
// ============================================================================

void GobTheGoodGoblin::VajrayaksaAbilities(Direction direction) {
    switch (direction) {
        case Direction::Up:
            // Heaven Splitter: All 4 arms create energy pillar
            // TODO: Implement
            break;
        case Direction::Left:
            // Overlord's Decree: AOE fear + reset ally cooldowns
            // TODO: Implement
            break;
        case Direction::Right:
            // Thousand Arms Rush: Teleport → 20-hit barrage
            // TODO: Implement
            break;
        default:
            break;
    }
}

void GobTheGoodGoblin::VajrayaksaBlock() {
    CharacterBase::Block();
    // Overlord Guard: Attacks while blocking with spare arms
    // TODO: Implement simultaneous block/attack
}

void GobTheGoodGoblin::VajrayaksaGearSkills(int index) {
    switch (index) {
        case 0: // SD - World Devourer
            // AOE grab copying full combos + 30% meter
            GenerateEvolutionEnergy(30.0f);
            // TODO: Implement combo copy
            break;
        case 1: // AS - Apex Predator
            // All attacks unblockable + lifesteal 7s
            // TODO: Implement unblockable + lifesteal
            break;
        case 2: // AD - Divine Authority
            // Disable enemy skills 5s + team immunity
            // TODO: Implement skill disable
            break;
        case 3: // Space - Black Hole
            // Massive pull redistributing all buffs to team 6s
            // TODO: Implement black hole
            break;
    }
}

} // namespace ArenaFighter
