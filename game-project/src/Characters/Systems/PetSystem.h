#pragma once

#include <memory>
#include <vector>
#include <string>
#include <DirectXMath.h>

namespace ArenaFighter {

enum class PetType {
    Combat,       // Direct damage dealers
    Support,      // Healing/buffing pets
    Tank,         // Defensive pets
    Utility,      // Special ability pets
    Swarm,        // Multiple weak units
    Legendary,    // Powerful single pets
    Mechanical,   // Robot/tech pets
    Elemental     // Element-based pets
};

enum class PetBehavior {
    Aggressive,   // Attacks nearest enemy
    Defensive,    // Protects owner
    Follow,       // Stays close to owner
    Patrol,       // Patrols area
    Manual,       // Player-controlled
    Smart         // AI-driven tactics
};

struct PetStats {
    float health = 500.0f;
    float maxHealth = 500.0f;
    float attackPower = 50.0f;
    float defense = 30.0f;
    float speed = 120.0f;
    float attackRange = 5.0f;
    float detectionRange = 15.0f;
};

class Pet {
public:
    Pet(const std::string& name, PetType type);
    
    // Basic info
    const std::string& GetName() const { return m_name; }
    PetType GetType() const { return m_type; }
    
    // Stats
    PetStats& GetStats() { return m_stats; }
    bool IsAlive() const { return m_stats.health > 0; }
    
    // Behavior
    void SetBehavior(PetBehavior behavior) { m_behavior = behavior; }
    PetBehavior GetBehavior() const { return m_behavior; }
    
    // Combat
    void Attack(DirectX::XMFLOAT3 target);
    void TakeDamage(float damage);
    void Heal(float amount);
    
    // Movement
    void MoveTo(const DirectX::XMFLOAT3& position);
    void FollowOwner(const DirectX::XMFLOAT3& ownerPos);
    
    // Special abilities
    void UseSpecialAbility();
    void SetSpecialAbility(std::function<void()> ability) { m_specialAbility = ability; }
    
    // Update
    void Update(float deltaTime);
    
private:
    std::string m_name;
    PetType m_type;
    PetStats m_stats;
    PetBehavior m_behavior = PetBehavior::Follow;
    
    // Position and movement
    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT3 m_targetPosition;
    DirectX::XMFLOAT3 m_velocity;
    
    // Combat
    float m_attackCooldown = 0.0f;
    static constexpr float ATTACK_COOLDOWN = 1.5f;
    
    // Special ability
    std::function<void()> m_specialAbility;
    float m_abilityCooldown = 0.0f;
    static constexpr float ABILITY_COOLDOWN = 10.0f;
};

class PetSystem {
public:
    PetSystem();
    
    // Pet management
    void AddPet(std::unique_ptr<Pet> pet);
    void RemovePet(const std::string& name);
    void SummonPet(const std::string& name, const DirectX::XMFLOAT3& position);
    void RecallPet(const std::string& name);
    void RecallAllPets();
    
    // Active pets
    std::vector<Pet*> GetActivePets();
    Pet* GetPet(const std::string& name);
    int GetActivePetCount() const { return m_activePets.size(); }
    int GetMaxPets() const { return m_maxActivePets; }
    
    // Commands
    void CommandAttack(const DirectX::XMFLOAT3& target);
    void CommandDefend();
    void CommandFollow();
    void SetFormation(const std::string& formation);
    
    // Pet fusion/evolution
    bool CanFusePets(const std::string& pet1, const std::string& pet2);
    std::unique_ptr<Pet> FusePets(const std::string& pet1, const std::string& pet2);
    
    // Update
    void Update(float deltaTime, const DirectX::XMFLOAT3& ownerPosition);
    
    // Character-specific pet systems
    static PetSystem* CreateSuPingPetSystem();      // Pet store with variety
    static PetSystem* CreateNanomancerSystem();     // Nanobot swarm
    static PetSystem* CreateSummonerSystem();       // Traditional summoner
    static PetSystem* CreateBeastMasterSystem();    // Animal companions
    
private:
    std::vector<std::unique_ptr<Pet>> m_ownedPets;
    std::vector<Pet*> m_activePets;
    int m_maxActivePets = 3;
    
    // Formation system
    std::string m_currentFormation = "Follow";
    void ApplyFormation();
    
    // Pet synergy bonuses
    void CalculateSynergyBonuses();
    float m_synergyMultiplier = 1.0f;
};

// Example pets for Su Ping's Pet Store
namespace PetStoreCreatures {
    std::unique_ptr<Pet> CreatePurplePython() {
        auto pet = std::make_unique<Pet>("Purple Python", PetType::Combat);
        pet->GetStats().attackPower = 80.0f;
        pet->GetStats().health = 600.0f;
        pet->SetSpecialAbility([]() {
            // Constriction attack
        });
        return pet;
    }
    
    std::unique_ptr<Pet> CreateLightningRat() {
        auto pet = std::make_unique<Pet>("Lightning Rat", PetType::Utility);
        pet->GetStats().speed = 200.0f;
        pet->GetStats().attackPower = 40.0f;
        pet->SetSpecialAbility([]() {
            // Lightning dash
        });
        return pet;
    }
    
    std::unique_ptr<Pet> CreateVoidBeast() {
        auto pet = std::make_unique<Pet>("Void Beast", PetType::Legendary);
        pet->GetStats().health = 1000.0f;
        pet->GetStats().attackPower = 120.0f;
        pet->SetSpecialAbility([]() {
            // Void consume
        });
        return pet;
    }
}

} // namespace ArenaFighter