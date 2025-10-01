#pragma once

#include "../CharacterBase.h"
#include "../../Combat/CombatEnums.h"
#include <vector>
#include <memory>

namespace ArenaFighter {

// Forward declarations
class Pet;
enum class PetType;
enum class FusionForm;

// Contract Mana System - Different from regular mana
struct ContractMana {
    float current = 0.0f;
    float maximum = 100.0f;

    // Generation rates
    static constexpr float PASSIVE_REGEN = 2.0f;      // per second
    static constexpr float ON_BASIC_HIT = 3.0f;
    static constexpr float ON_PET_KILL = 15.0f;
    static constexpr float ON_DAMAGE_TAKEN = 0.2f;    // per HP lost
    static constexpr float ON_SUCCESSFUL_BLOCK = 3.0f;

    void Generate(float amount);
    bool CanAfford(float cost) const;
    void Consume(float cost);
    void Update(float deltaTime);
};

// Pet Tiers
enum class PetTier {
    Tier1 = 1,  // 30 mana, cannot fuse
    Tier2 = 2,  // 40 mana, fusion available
    Tier3 = 3   // 60 mana, fusion available
};

// Pet Types (4 gear slots)
enum class PetType {
    Undead,   // Weapon slot [S+D]
    Dragon,   // Helmet slot [A+D]
    Beast,    // Armor slot [A+S]
    Mythic    // Trinket slot [A+S+D]
};

// Fusion Forms
enum class FusionForm {
    None,
    SkeletonWarrior,    // Little Skeleton fusion
    UndeadOverlord,     // Skeleton King fusion
    DragonKnight,       // Inferno Dragon fusion
    ChaosDragonGod,     // Chaos Dragon fusion
    StormBeast,         // Thunder Tiger fusion
    VoidWalker,         // Void Beast fusion
    PhoenixAvatar,      // Phoenix fusion
    TitanDestroyer      // Chaos Titan fusion
};

// Pet Base Class (AI-controlled)
class Pet {
public:
    Pet(PetType type, PetTier tier);
    virtual ~Pet() = default;

    // Core properties
    float health;
    float maxHealth;
    float damage[4];  // Combo damage values
    bool canFuse;
    bool isAlive;
    PetType type;
    PetTier tier;

    // Fusion tracking
    bool hasBeenUsedForFusion = false;

    // AI behavior (fully autonomous)
    virtual void UpdateAI(float deltaTime) = 0;
    virtual void Attack() = 0;
    virtual void OnDeath() = 0;

    // Position and movement
    float x, y, z;
    float speedMultiplier = 1.0f;

    // Check if can be fused
    bool CanBeFused() const;
    void MarkUsedForFusion();
};

// Yuito - AI Pet Master
class Yuito : public CharacterBase {
public:
    Yuito();
    virtual ~Yuito() = default;

    // Character info
    std::string GetName() const override { return "Yuito"; }
    std::string GetCategory() const override { return "System"; }
    std::string GetTier() const override { return "A"; }

    // Core update
    void Update(float deltaTime) override;

    // Contract Mana System
    ContractMana contractMana;
    void GenerateContractMana(float amount);
    void OnBasicAttackHit();
    void OnPetKill();
    void OnDamageTaken(float damage);
    void OnSuccessfulBlock();

    // Pet Management
    std::vector<std::shared_ptr<Pet>> activePets;
    void SummonPet(PetType type, PetTier tier);
    void UpdatePets(float deltaTime);
    void RemoveDeadPets();

    // Fusion System
    FusionForm currentFusion = FusionForm::None;
    float fusionTimeRemaining = 0.0f;
    static constexpr float FUSION_DURATION = 20.0f;
    static constexpr float EMERGENCY_FUSION_DURATION = 25.0f;

    bool TryFusion(std::shared_ptr<Pet> pet);
    void StartFusion(FusionForm form, float duration);
    void EndFusion();
    void UpdateFusion(float deltaTime);
    bool IsFused() const { return currentFusion != FusionForm::None; }

    // Emergency Protocol - At 30% HP
    bool emergencyProtocolUsed = false;
    void CheckEmergencyProtocol();
    void TriggerEmergencyProtocol();

    // Combat overrides
    void ExecuteSpecialMove(Direction direction) override;
    void ExecuteGearSkill(int index) override;
    void Block() override;

    // Base form (no fusion) - extremely weak
    void BaseFormAttack();
    bool CanUseSpecialMoves() const;

    // Fusion-specific abilities
    void SkeletonWarriorAbilities(Direction direction);
    void UndeadOverlordAbilities(Direction direction);
    void DragonKnightAbilities(Direction direction);
    void ChaosDragonGodAbilities(Direction direction);
    void StormBeastAbilities(Direction direction);
    void VoidWalkerAbilities(Direction direction);
    void PhoenixAvatarAbilities(Direction direction);
    void TitanDestroyerAbilities(Direction direction);

    // Enhanced gear skills (fusion-specific)
    void SkeletonWarriorGearSkills(int index);
    void UndeadOverlordGearSkills(int index);
    void DragonKnightGearSkills(int index);
    void ChaosDragonGodGearSkills(int index);
    void StormBeastGearSkills(int index);
    void VoidWalkerGearSkills(int index);
    void PhoenixAvatarGearSkills(int index);
    void TitanDestroyerGearSkills(int index);

    // Enhanced block (fusion-specific)
    void SkeletonWarriorBlock();
    void UndeadOverlordBlock();
    void DragonKnightBlock();
    void ChaosDragonGodBlock();
    void StormBeastBlock();
    void VoidWalkerBlock();
    void PhoenixAvatarBlock();
    void TitanDestroyerBlock();

private:
    void InitializeYuitoStats();
    void SetupBaseGearSkills();

    // Helper to find nearest pet for fusion
    std::shared_ptr<Pet> FindNearestFusablePet();

    // Upgrade pet tier (for emergency protocol)
    void UpgradePetTier(std::shared_ptr<Pet> pet);
};

} // namespace ArenaFighter
