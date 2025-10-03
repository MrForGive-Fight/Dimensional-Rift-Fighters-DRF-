#include "Seraphina.h"
#include "../../Combat/DamageCalculator.h"
#include <algorithm>
#include <cmath>

namespace ArenaFighter {

// ============================================================================
// CultivationEssence Implementation
// ============================================================================

void CultivationEssence::Generate(float amount) {
    current = std::min(current + amount, maximum);
}

bool CultivationEssence::CanAfford(float cost) const {
    return current >= cost;
}

void CultivationEssence::Consume(float cost) {
    current = std::max(0.0f, current - cost);
}

void CultivationEssence::Update(float deltaTime) {
    // Natural regeneration: +1 per second
    Generate(NATURAL_REGEN * deltaTime);
}

void CultivationEssence::FillToMax() {
    current = maximum;
}

// ============================================================================
// ConvergenceMeter Implementation
// ============================================================================

void ConvergenceMeter::Generate(float amount) {
    if (!isLocked) {
        current = std::min(current + amount, maximum);
    }
}

void ConvergenceMeter::Update(float deltaTime, bool inCombat) {
    if (!isLocked && inCombat) {
        // Passive generation during combat: +1 per second
        Generate(COMBAT_PASSIVE * deltaTime);
    }
}

void ConvergenceMeter::FillToMax() {
    current = maximum;
}

// ============================================================================
// Seraphina Constructor
// ============================================================================

Seraphina::Seraphina() {
    InitializeSeraphinaStats();
}

void Seraphina::InitializeSeraphinaStats() {
    // S-Tier Cultivation stats
    stats.maxHealth = 240.0f;
    stats.health = 240.0f;
    stats.attack = 105.0f;
    stats.defense = 90.0f;
    stats.speed = 110.0f;       // High mobility with wings
    stats.maxMana = 100.0f;
    stats.mana = 100.0f;
    stats.manaRegen = 5.0f;

    // Cultivation resources start full
    cultivationEssence.current = 100.0f;
    cultivationEssence.maximum = 100.0f;

    convergenceMeter.current = 0.0f;
    convergenceMeter.maximum = 100.0f;

    // Start with Ice Dao
    currentDao = DaoPath::Ice;
}

// ============================================================================
// Core Update Loop
// ============================================================================

void Seraphina::Update(float deltaTime) {
    CharacterBase::Update(deltaTime);

    // Update resources
    cultivationEssence.Update(deltaTime);

    // TODO: Determine if in combat based on recent damage/hits
    bool inCombat = true;  // Placeholder
    convergenceMeter.Update(deltaTime, inCombat);

    // Check for Convergence activation
    CheckConvergenceActivation();

    // Update Convergence state
    if (isInConvergenceState) {
        UpdateConvergence(deltaTime);
    }

    // Update Dao Awakening
    if (isDaoAwakened) {
        daoAwakeningTimer -= deltaTime;
        if (daoAwakeningTimer <= 0.0f) {
            isDaoAwakened = false;
        }
    }

    // Update skill timers
    if (celestialAuthorityActive) {
        celestialAuthorityTimer -= deltaTime;
        if (celestialAuthorityTimer <= 0.0f) {
            celestialAuthorityActive = false;
        }
    }

    if (iceMirrorBombActive || toxicShadowBombActive) {
        cloneBombTimer -= deltaTime;
        if (cloneBombTimer <= 0.0f) {
            // TODO: Trigger clone explosion
            iceMirrorBombActive = false;
            toxicShadowBombActive = false;
        }
    }

    if (glacialSymbolActive || toxicSymbolActive) {
        elementSymbolTimer -= deltaTime;
        if (elementSymbolTimer <= 0.0f) {
            glacialSymbolActive = false;
            toxicSymbolActive = false;
        }
    }

    if (heavensDominionActive) {
        heavensDominionTimer -= deltaTime;
        if (heavensDominionTimer <= 0.0f) {
            heavensDominionActive = false;
        }
    }

    // Check emergency protocol
    if (!angelsDesperationUsed) {
        CheckEmergencyProtocol();
    }
}

// ============================================================================
// Resource Management
// ============================================================================

void Seraphina::GenerateCultivationEssence(float amount) {
    cultivationEssence.Generate(amount);
}

void Seraphina::OnSuccessfulHit() {
    GenerateCultivationEssence(CultivationEssence::ON_HIT);
}

void Seraphina::OnSuccessfulDefense() {
    GenerateCultivationEssence(CultivationEssence::ON_DEFENSE);
}

void Seraphina::OnTechniqueUsed() {
    convergenceMeter.Generate(ConvergenceMeter::PER_TECHNIQUE);
}

// ============================================================================
// Dao Control System
// ============================================================================

void Seraphina::ToggleDao() {
    // S+Down switches between Ice ↔ Poison
    if (currentDao == DaoPath::Ice) {
        currentDao = DaoPath::Poison;
    } else {
        currentDao = DaoPath::Ice;
    }

    // 0.5s toggle animation
    // TODO: Play toggle VFX/animation
}

// ============================================================================
// Heavenly Convergence State
// ============================================================================

void Seraphina::CheckConvergenceActivation() {
    if (!isInConvergenceState && convergenceMeter.IsMaxed()) {
        ActivateConvergence(false);
    }
}

void Seraphina::ActivateConvergence(bool enhanced) {
    isInConvergenceState = true;
    convergenceEnhanced = enhanced;

    // Lock meter at 100 during Convergence
    convergenceMeter.Lock();

    // Set duration
    if (enhanced) {
        convergenceTimeRemaining = ENHANCED_CONVERGENCE_DURATION;
        ApplyConvergenceEnhancements();
    } else {
        convergenceTimeRemaining = NORMAL_CONVERGENCE_DURATION;
    }

    // TODO: Play Convergence activation VFX
    // Wings transform: both left blue, both right green
}

void Seraphina::UpdateConvergence(float deltaTime) {
    convergenceTimeRemaining -= deltaTime;

    if (convergenceTimeRemaining <= 0.0f) {
        EndConvergence();
    }
}

void Seraphina::EndConvergence() {
    isInConvergenceState = false;
    convergenceEnhanced = false;

    // Unlock and reset meter
    convergenceMeter.Unlock();
    convergenceMeter.current = 0.0f;

    // Reset stats if enhanced
    if (convergenceEnhanced) {
        InitializeSeraphinaStats();
    }

    // TODO: Play end VFX
}

void Seraphina::ApplyConvergenceEnhancements() {
    // Emergency Protocol enhanced Convergence bonuses applied
    // Damage boost, essence efficiency, cooldown reduction, etc.
    // These are applied via multiplier getters
}

// ============================================================================
// Emergency Protocol - Angel's Desperate Ascension
// ============================================================================

void Seraphina::CheckEmergencyProtocol() {
    float healthPercent = stats.health / stats.maxHealth;

    if (healthPercent <= 0.30f && !angelsDesperationUsed) {
        TriggerAngelsDesperateAscension();
    }
}

void Seraphina::TriggerAngelsDesperateAscension() {
    angelsDesperationUsed = true;

    // Instant fills
    cultivationEssence.FillToMax();
    convergenceMeter.FillToMax();

    // All cooldowns reset
    // TODO: Implement cooldown system reset

    // 3 seconds invincibility
    // TODO: Implement invincibility frames

    // Activate enhanced Convergence (25s instead of 20s)
    ActivateConvergence(true);

    // TODO: Play dramatic ascension VFX
    // All four wings transform, divine light, etc.
}

// ============================================================================
// Helper Methods
// ============================================================================

float Seraphina::GetDamageMultiplier() const {
    if (convergenceEnhanced) {
        return ConvergenceEnhancements::Emergency::DAMAGE_BOOST;
    }
    return 1.0f;
}

float Seraphina::GetEssenceCostMultiplier() const {
    if (convergenceEnhanced) {
        return ConvergenceEnhancements::Emergency::ESSENCE_EFFICIENCY;
    }
    return 1.0f;
}

float Seraphina::GetCooldownReduction() const {
    if (convergenceEnhanced) {
        return ConvergenceEnhancements::Emergency::COOLDOWN_REDUCTION;
    }
    return 0.0f;
}

// ============================================================================
// Combat Overrides
// ============================================================================

void Seraphina::ExecuteSpecialMove(Direction direction) {
    if (!CanUseSpecialMoves()) {
        return;
    }

    if (isInConvergenceState) {
        ConvergenceAbilities(direction);
    } else if (currentDao == DaoPath::Ice) {
        IceDaoAbilities(direction);
    } else {
        PoisonDaoAbilities(direction);
    }
}

void Seraphina::ExecuteGearSkill(int index) {
    if (isInConvergenceState) {
        ConvergenceGearSkills(index);
    } else if (currentDao == DaoPath::Ice) {
        IceDaoGearSkills(index);
    } else {
        PoisonDaoGearSkills(index);
    }
}

void Seraphina::Block() {
    if (isInConvergenceState) {
        ConvergenceBlock();
    } else if (currentDao == DaoPath::Ice) {
        IceDaoBlock();
    } else {
        PoisonDaoBlock();
    }

    OnSuccessfulDefense();
}

bool Seraphina::CanUseSpecialMoves() const {
    return true;  // Always can use moves
}

// ============================================================================
// ICE DAO TECHNIQUES
// ============================================================================

void Seraphina::IceDaoAbilities(Direction direction) {
    switch (direction) {
        case Direction::Up:
            // Glacial Feather Storm: 6 ice feathers seeking enemies
            if (cultivationEssence.CanAfford(TechniqueCosts::ASCENDING_TECHNIQUE * GetEssenceCostMultiplier())) {
                cultivationEssence.Consume(TechniqueCosts::ASCENDING_TECHNIQUE * GetEssenceCostMultiplier());
                OnTechniqueUsed();
                // TODO: Implement ice feather storm
                // Auto-freeze summons on hit
            }
            break;

        case Direction::Left:
            // Frozen Wing Sanctuary: Defensive ice barrier
            if (cultivationEssence.CanAfford(TechniqueCosts::RETREAT_TECHNIQUE * GetEssenceCostMultiplier())) {
                cultivationEssence.Consume(TechniqueCosts::RETREAT_TECHNIQUE * GetEssenceCostMultiplier());
                OnTechniqueUsed();
                // TODO: Implement ice barrier
                // Reflects projectiles, freezes melee attackers
                // 3% HP regen per second
            }
            break;

        case Direction::Right:
            // Blizzard Wing Rush: Ice storm charge
            if (cultivationEssence.CanAfford(TechniqueCosts::FORWARD_TECHNIQUE * GetEssenceCostMultiplier())) {
                cultivationEssence.Consume(TechniqueCosts::FORWARD_TECHNIQUE * GetEssenceCostMultiplier());
                OnTechniqueUsed();
                // TODO: Implement blizzard rush
                // 80% slow path, freeze final target 4s
            }
            break;

        case Direction::Down:
            // Toggle Dao to Poison
            ToggleDao();
            break;

        default:
            break;
    }
}

void Seraphina::IceDaoBlock() {
    CharacterBase::Block();
    // Ice Dao standard block
    // TODO: Apply frost effects on successful block
}

void Seraphina::IceDaoGearSkills(int index) {
    switch (index) {
        case 0: // SD - Celestial Dao Awakening (Ice version)
            ActivateDaoAwakening(false);
            break;

        case 1: // AS - Celestial Authority (Ice version)
            // Field slows 50%, freezes after 3s
            celestialAuthorityActive = true;
            celestialAuthorityTimer = 6.0f;
            // TODO: Implement ice field
            break;

        case 2: // AD - Frozen Mirror Bomb
            iceMirrorBombActive = true;
            cloneBombTimer = 15.0f;
            // TODO: Spawn ice clone
            break;

        case 3: // ASD - Glacial Symbol Barrage
            glacialSymbolActive = true;
            elementSymbolTimer = 12.0f;
            // TODO: Spawn ice symbol, shoots 4 homing rays
            break;
    }
}

// ============================================================================
// POISON DAO TECHNIQUES
// ============================================================================

void Seraphina::PoisonDaoAbilities(Direction direction) {
    switch (direction) {
        case Direction::Up:
            // Toxic Wing Ascension: Diving poison strike
            if (cultivationEssence.CanAfford(TechniqueCosts::ASCENDING_TECHNIQUE * GetEssenceCostMultiplier())) {
                cultivationEssence.Consume(TechniqueCosts::ASCENDING_TECHNIQUE * GetEssenceCostMultiplier());
                OnTechniqueUsed();
                // TODO: Implement poison dive
                // Ignores defenses, applies 3 poison stacks
            }
            break;

        case Direction::Left:
            // Toxic Wing Escape: Untargetable poison mist
            if (cultivationEssence.CanAfford(TechniqueCosts::RETREAT_TECHNIQUE * GetEssenceCostMultiplier())) {
                cultivationEssence.Consume(TechniqueCosts::RETREAT_TECHNIQUE * GetEssenceCostMultiplier());
                OnTechniqueUsed();
                // TODO: Implement poison stealth
                // Move through enemies, 1 stack each
                // Next attack +200% damage + 5 stacks
            }
            break;

        case Direction::Right:
            // Toxic Wing Dash: Teleport poison line
            if (cultivationEssence.CanAfford(TechniqueCosts::FORWARD_TECHNIQUE * GetEssenceCostMultiplier())) {
                cultivationEssence.Consume(TechniqueCosts::FORWARD_TECHNIQUE * GetEssenceCostMultiplier());
                OnTechniqueUsed();
                // TODO: Implement poison dash
                // Each enemy: poison damage + 2 stacks
                // Final position: next attack +3 stacks
            }
            break;

        case Direction::Down:
            // Toggle Dao to Ice
            ToggleDao();
            break;

        default:
            break;
    }
}

void Seraphina::PoisonDaoBlock() {
    CharacterBase::Block();
    // Poison Dao standard block
    // TODO: Apply toxic counter on successful block
}

void Seraphina::PoisonDaoGearSkills(int index) {
    switch (index) {
        case 0: // SD - Celestial Dao Awakening (Poison version)
            ActivateDaoAwakening(false);
            break;

        case 1: // AS - Celestial Authority (Poison version)
            // Field marks enemies, 1 poison stack per second
            celestialAuthorityActive = true;
            celestialAuthorityTimer = 6.0f;
            // TODO: Implement poison field
            break;

        case 2: // AD - Toxic Shadow Bomb
            toxicShadowBombActive = true;
            cloneBombTimer = 15.0f;
            // TODO: Spawn poison clone
            break;

        case 3: // ASD - Toxic Symbol Assault
            toxicSymbolActive = true;
            elementSymbolTimer = 12.0f;
            // TODO: Spawn poison symbol, fires 8 beams
            break;
    }
}

// ============================================================================
// CONVERGENCE STATE TECHNIQUES
// ============================================================================

void Seraphina::ConvergenceAbilities(Direction direction) {
    switch (direction) {
        case Direction::Up:
            // Convergence version of ascending technique
            // TODO: Implement dual-element version
            break;

        case Direction::Left:
            // Convergence version of retreat technique
            // TODO: Implement dual-element version
            break;

        case Direction::Right:
            // Dimensional Rift Gate: Portal with dual effects
            if (cultivationEssence.CanAfford(TechniqueCosts::DIMENSIONAL_RIFT_GATE * GetEssenceCostMultiplier())) {
                cultivationEssence.Consume(TechniqueCosts::DIMENSIONAL_RIFT_GATE * GetEssenceCostMultiplier());
                OnTechniqueUsed();
                // TODO: Create dual-element portal
                // Left side freezes, right side poisons
                // Persists for Convergence duration
            }
            break;

        case Direction::Down:
            // Can't toggle during Convergence
            break;

        default:
            break;
    }
}

void Seraphina::ConvergenceBlock() {
    CharacterBase::Block();
    // Convergence enhanced block
    // TODO: Both ice and poison effects
}

void Seraphina::ConvergenceGearSkills(int index) {
    switch (index) {
        case 0: // SD - Dual Awakening
            ActivateDaoAwakening(true);  // Both elements
            break;

        case 1: // AS - Combined field
            // Both slow and poison effects
            celestialAuthorityActive = true;
            celestialAuthorityTimer = 10.0f;  // Extended if enhanced
            // TODO: Implement combined field
            break;

        case 2: // AD - Both clones
            iceMirrorBombActive = true;
            toxicShadowBombActive = true;
            cloneBombTimer = 15.0f;
            // TODO: Spawn both clones
            break;

        case 3: // ASD - Both symbols
            glacialSymbolActive = true;
            toxicSymbolActive = true;
            elementSymbolTimer = 12.0f;
            // TODO: Spawn both symbols
            break;
    }
}

// ============================================================================
// DAO AWAKENING (SD Weapon Skill)
// ============================================================================

void Seraphina::ActivateDaoAwakening(bool enhancedDual) {
    isDaoAwakened = true;

    if (enhancedDual) {
        // Essence enhancement: Both Ice and Poison effects
        daoAwakeningTimer = 18.0f;  // Extended duration
        // TODO: Apply both awakening effects
    } else {
        daoAwakeningTimer = 10.0f;  // Normal duration
        // TODO: Apply current Dao awakening
        // Ice: All attacks 40% slow, 4th hit guaranteed freeze
        // Poison: All attacks apply 1 stack, increased max stacks to 6
    }
}

} // namespace ArenaFighter
