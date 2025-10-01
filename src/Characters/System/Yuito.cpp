#include "Yuito.h"
#include "../../Combat/DamageCalculator.h"
#include <algorithm>
#include <cmath>

namespace ArenaFighter {

// ============================================================================
// ContractMana Implementation
// ============================================================================

void ContractMana::Generate(float amount) {
    current = std::min(current + amount, maximum);
}

bool ContractMana::CanAfford(float cost) const {
    return current >= cost;
}

void ContractMana::Consume(float cost) {
    current = std::max(0.0f, current - cost);
}

void ContractMana::Update(float deltaTime) {
    // Passive regeneration
    Generate(PASSIVE_REGEN * deltaTime);
}

// ============================================================================
// Pet Base Implementation
// ============================================================================

Pet::Pet(PetType type, PetTier tier)
    : type(type)
    , tier(tier)
    , isAlive(true)
    , x(0.0f)
    , y(0.0f)
    , z(0.0f) {

    // Tier 1 pets cannot be fused
    canFuse = (tier != PetTier::Tier1);
}

bool Pet::CanBeFused() const {
    return canFuse && isAlive && !hasBeenUsedForFusion;
}

void Pet::MarkUsedForFusion() {
    hasBeenUsedForFusion = true;
}

// ============================================================================
// Yuito Constructor
// ============================================================================

Yuito::Yuito() {
    InitializeYuitoStats();
    SetupBaseGearSkills();
}

void Yuito::InitializeYuitoStats() {
    // Yuito has the WORST stats in the game (base form)
    stats.maxHealth = 210.0f;  // Lowest
    stats.health = 210.0f;
    stats.attack = 5.0f;        // Weakest
    stats.defense = 60.0f;      // Paper thin
    stats.speed = 95.0f;        // Below average
    stats.maxMana = 100.0f;
    stats.mana = 100.0f;
    stats.manaRegen = 5.0f;

    // Contract mana starts at 0
    contractMana.current = 0.0f;
    contractMana.maximum = 100.0f;
}

void Yuito::SetupBaseGearSkills() {
    // Gear skills summon pets (when not fused)
    // [S+D] - Undead Contracts
    // [A+D] - Dragon Contracts
    // [A+S] - Beast Contracts
    // [A+S+D] - Mythic Contracts

    // These will be handled in ExecuteGearSkill based on fusion state
}

// ============================================================================
// Core Update Loop
// ============================================================================

void Yuito::Update(float deltaTime) {
    CharacterBase::Update(deltaTime);

    // Update contract mana
    contractMana.Update(deltaTime);

    // Update all pets
    UpdatePets(deltaTime);

    // Update fusion timer
    if (IsFused()) {
        UpdateFusion(deltaTime);
    }

    // Check emergency protocol
    if (!emergencyProtocolUsed) {
        CheckEmergencyProtocol();
    }
}

// ============================================================================
// Contract Mana Generation
// ============================================================================

void Yuito::GenerateContractMana(float amount) {
    contractMana.Generate(amount);
}

void Yuito::OnBasicAttackHit() {
    GenerateContractMana(ContractMana::ON_BASIC_HIT);
}

void Yuito::OnPetKill() {
    GenerateContractMana(ContractMana::ON_PET_KILL);
}

void Yuito::OnDamageTaken(float damage) {
    GenerateContractMana(damage * ContractMana::ON_DAMAGE_TAKEN);
}

void Yuito::OnSuccessfulBlock() {
    GenerateContractMana(ContractMana::ON_SUCCESSFUL_BLOCK);
}

// ============================================================================
// Pet Management
// ============================================================================

void Yuito::SummonPet(PetType type, PetTier tier) {
    // Cost varies by tier
    float cost = 0.0f;
    switch (tier) {
        case PetTier::Tier1: cost = 30.0f; break;
        case PetTier::Tier2: cost = 40.0f; break;
        case PetTier::Tier3: cost = 60.0f; break;
    }

    if (!contractMana.CanAfford(cost)) {
        return;  // Not enough mana
    }

    contractMana.Consume(cost);

    // Create specific pet based on type and tier
    // TODO: Implement specific pet classes
    // For now, placeholder
}

void Yuito::UpdatePets(float deltaTime) {
    // Update all active pets' AI
    for (auto& pet : activePets) {
        if (pet && pet->isAlive) {
            pet->UpdateAI(deltaTime);
        }
    }

    // Remove dead pets
    RemoveDeadPets();
}

void Yuito::RemoveDeadPets() {
    activePets.erase(
        std::remove_if(activePets.begin(), activePets.end(),
            [](const std::shared_ptr<Pet>& pet) {
                return !pet || !pet->isAlive;
            }),
        activePets.end()
    );
}

// ============================================================================
// Fusion System
// ============================================================================

bool Yuito::TryFusion(std::shared_ptr<Pet> pet) {
    if (!pet || !pet->CanBeFused()) {
        return false;
    }

    // Fusion is FREE (no mana cost)
    // But each pet can only be used once

    // Determine fusion form based on pet type and tier
    FusionForm form = FusionForm::None;

    switch (pet->type) {
        case PetType::Undead:
            if (pet->tier == PetTier::Tier2) {
                form = FusionForm::SkeletonWarrior;
            } else if (pet->tier == PetTier::Tier3) {
                form = FusionForm::UndeadOverlord;
            }
            break;

        case PetType::Dragon:
            if (pet->tier == PetTier::Tier2) {
                form = FusionForm::DragonKnight;
            } else if (pet->tier == PetTier::Tier3) {
                form = FusionForm::ChaosDragonGod;
            }
            break;

        case PetType::Beast:
            if (pet->tier == PetTier::Tier2) {
                form = FusionForm::StormBeast;
            } else if (pet->tier == PetTier::Tier3) {
                form = FusionForm::VoidWalker;
            }
            break;

        case PetType::Mythic:
            if (pet->tier == PetTier::Tier2) {
                form = FusionForm::PhoenixAvatar;
            } else if (pet->tier == PetTier::Tier3) {
                form = FusionForm::TitanDestroyer;
            }
            break;
    }

    if (form == FusionForm::None) {
        return false;
    }

    // Mark pet as used
    pet->MarkUsedForFusion();

    // Start fusion
    StartFusion(form, FUSION_DURATION);

    return true;
}

void Yuito::StartFusion(FusionForm form, float duration) {
    currentFusion = form;
    fusionTimeRemaining = duration;

    // Boost stats based on fusion form
    // TODO: Add form-specific stat modifications
}

void Yuito::EndFusion() {
    currentFusion = FusionForm::None;
    fusionTimeRemaining = 0.0f;

    // Restore base stats
    InitializeYuitoStats();
}

void Yuito::UpdateFusion(float deltaTime) {
    fusionTimeRemaining -= deltaTime;

    if (fusionTimeRemaining <= 0.0f) {
        EndFusion();
    }
}

// ============================================================================
// Emergency Protocol
// ============================================================================

void Yuito::CheckEmergencyProtocol() {
    float healthPercent = stats.health / stats.maxHealth;

    if (healthPercent <= 0.30f && !emergencyProtocolUsed) {
        TriggerEmergencyProtocol();
    }
}

void Yuito::TriggerEmergencyProtocol() {
    emergencyProtocolUsed = true;

    // Find nearest pet
    auto nearestPet = FindNearestFusablePet();

    if (nearestPet) {
        // Upgrade pet one tier
        UpgradePetTier(nearestPet);

        // Instant fusion with extended duration
        StartFusion(FusionForm::TitanDestroyer, EMERGENCY_FUSION_DURATION);

        // 50% damage reduction for 3 seconds
        // TODO: Add temporary buff system
    }
}

std::shared_ptr<Pet> Yuito::FindNearestFusablePet() {
    std::shared_ptr<Pet> nearest = nullptr;
    float minDistance = std::numeric_limits<float>::max();

    for (auto& pet : activePets) {
        if (pet && pet->CanBeFused()) {
            // Calculate distance
            float dx = pet->x - x;
            float dy = pet->y - y;
            float dz = pet->z - z;
            float distance = std::sqrt(dx*dx + dy*dy + dz*dz);

            if (distance < minDistance) {
                minDistance = distance;
                nearest = pet;
            }
        }
    }

    return nearest;
}

void Yuito::UpgradePetTier(std::shared_ptr<Pet> pet) {
    if (!pet) return;

    if (pet->tier == PetTier::Tier1) {
        pet->tier = PetTier::Tier2;
        pet->canFuse = true;
    } else if (pet->tier == PetTier::Tier2) {
        pet->tier = PetTier::Tier3;
    }

    // TODO: Update pet stats based on new tier
}

// ============================================================================
// Combat Overrides
// ============================================================================

void Yuito::ExecuteSpecialMove(Direction direction) {
    // Base Yuito has NO special moves
    if (!CanUseSpecialMoves()) {
        return;  // Cannot use special moves without fusion
    }

    // Route to appropriate fusion form
    switch (currentFusion) {
        case FusionForm::SkeletonWarrior:
            SkeletonWarriorAbilities(direction);
            break;
        case FusionForm::UndeadOverlord:
            UndeadOverlordAbilities(direction);
            break;
        case FusionForm::DragonKnight:
            DragonKnightAbilities(direction);
            break;
        case FusionForm::ChaosDragonGod:
            ChaosDragonGodAbilities(direction);
            break;
        case FusionForm::StormBeast:
            StormBeastAbilities(direction);
            break;
        case FusionForm::VoidWalker:
            VoidWalkerAbilities(direction);
            break;
        case FusionForm::PhoenixAvatar:
            PhoenixAvatarAbilities(direction);
            break;
        case FusionForm::TitanDestroyer:
            TitanDestroyerAbilities(direction);
            break;
        default:
            break;
    }
}

void Yuito::ExecuteGearSkill(int index) {
    if (IsFused()) {
        // Enhanced gear skills during fusion
        switch (currentFusion) {
            case FusionForm::SkeletonWarrior:
                SkeletonWarriorGearSkills(index);
                break;
            case FusionForm::UndeadOverlord:
                UndeadOverlordGearSkills(index);
                break;
            case FusionForm::DragonKnight:
                DragonKnightGearSkills(index);
                break;
            case FusionForm::ChaosDragonGod:
                ChaosDragonGodGearSkills(index);
                break;
            case FusionForm::StormBeast:
                StormBeastGearSkills(index);
                break;
            case FusionForm::VoidWalker:
                VoidWalkerGearSkills(index);
                break;
            case FusionForm::PhoenixAvatar:
                PhoenixAvatarGearSkills(index);
                break;
            case FusionForm::TitanDestroyer:
                TitanDestroyerGearSkills(index);
                break;
            default:
                break;
        }
    } else {
        // Base form - summon pets
        // index 0: [S+D] Undead
        // index 1: [A+D] Dragon
        // index 2: [A+S] Beast
        // index 3: [A+S+D] Mythic

        PetType type = static_cast<PetType>(index);
        // Default to Tier 1, player can upgrade
        SummonPet(type, PetTier::Tier1);
    }
}

void Yuito::Block() {
    if (IsFused()) {
        // Enhanced block based on fusion form
        switch (currentFusion) {
            case FusionForm::SkeletonWarrior:
                SkeletonWarriorBlock();
                break;
            case FusionForm::UndeadOverlord:
                UndeadOverlordBlock();
                break;
            case FusionForm::DragonKnight:
                DragonKnightBlock();
                break;
            case FusionForm::ChaosDragonGod:
                ChaosDragonGodBlock();
                break;
            case FusionForm::StormBeast:
                StormBeastBlock();
                break;
            case FusionForm::VoidWalker:
                VoidWalkerBlock();
                break;
            case FusionForm::PhoenixAvatar:
                PhoenixAvatarBlock();
                break;
            case FusionForm::TitanDestroyer:
                TitanDestroyerBlock();
                break;
            default:
                CharacterBase::Block();  // Standard block
                break;
        }
    } else {
        // Base form - standard block
        CharacterBase::Block();
        OnSuccessfulBlock();  // Generate contract mana
    }
}

bool Yuito::CanUseSpecialMoves() const {
    return IsFused();  // Only when fused
}

void Yuito::BaseFormAttack() {
    // Extremely weak attacks
    // D: 5 damage
    // D-D: 5+6 damage
    // D-D-D: 5+6+7 damage
    // D-D-D-D: 5+6+7+8 damage

    // TODO: Implement basic combo system
}

// ============================================================================
// Fusion-Specific Abilities (Placeholders - will implement each)
// ============================================================================

// Skeleton Warrior (Little Skeleton Fusion)
void Yuito::SkeletonWarriorAbilities(Direction direction) {
    switch (direction) {
        case Direction::Up:
            // Rising Bones (12 damage uppercut, launches enemy)
            // TODO: Implement
            break;
        case Direction::Left:
            // Bone Spear (13 damage piercing projectile)
            // TODO: Implement
            break;
        case Direction::Right:
            // Bone Rush (multi-hit charge, 3x5 damage)
            // TODO: Implement
            break;
        default:
            break;
    }
}

void Yuito::SkeletonWarriorBlock() {
    // Bone Guard (enhanced block, reflects projectiles)
    CharacterBase::Block();
    // TODO: Add projectile reflection
}

void Yuito::SkeletonWarriorGearSkills(int index) {
    // Enhanced gear skills for Skeleton Warrior
    // TODO: Implement each gear skill
}

// Additional fusion forms will be implemented similarly
// For brevity, adding stubs for now

void Yuito::UndeadOverlordAbilities(Direction direction) { /* TODO */ }
void Yuito::UndeadOverlordBlock() { CharacterBase::Block(); }
void Yuito::UndeadOverlordGearSkills(int index) { /* TODO */ }

void Yuito::DragonKnightAbilities(Direction direction) { /* TODO */ }
void Yuito::DragonKnightBlock() { CharacterBase::Block(); }
void Yuito::DragonKnightGearSkills(int index) { /* TODO */ }

void Yuito::ChaosDragonGodAbilities(Direction direction) { /* TODO */ }
void Yuito::ChaosDragonGodBlock() { CharacterBase::Block(); }
void Yuito::ChaosDragonGodGearSkills(int index) { /* TODO */ }

void Yuito::StormBeastAbilities(Direction direction) { /* TODO */ }
void Yuito::StormBeastBlock() { CharacterBase::Block(); }
void Yuito::StormBeastGearSkills(int index) { /* TODO */ }

void Yuito::VoidWalkerAbilities(Direction direction) { /* TODO */ }
void Yuito::VoidWalkerBlock() { CharacterBase::Block(); }
void Yuito::VoidWalkerGearSkills(int index) { /* TODO */ }

void Yuito::PhoenixAvatarAbilities(Direction direction) { /* TODO */ }
void Yuito::PhoenixAvatarBlock() { CharacterBase::Block(); }
void Yuito::PhoenixAvatarGearSkills(int index) { /* TODO */ }

void Yuito::TitanDestroyerAbilities(Direction direction) { /* TODO */ }
void Yuito::TitanDestroyerBlock() { CharacterBase::Block(); }
void Yuito::TitanDestroyerGearSkills(int index) { /* TODO */ }

} // namespace ArenaFighter
