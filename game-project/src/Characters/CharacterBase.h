#pragma once

#include <memory>
#include <vector>
#include <string>
#include <optional>
#include <array>
#include "../Combat/CombatEnums.h"

namespace ArenaFighter {

// Forward declarations
class StanceSystem;
class PetSystem;
class EvolutionSystem;
class TransformationSystem;
class WeaponMasterySystem;
class CultivationSystem;
class BlessingSystem;

// Universal gear skill system
struct GearSkill {
    std::string name;
    std::string animation;
    float manaCost = 20.0f;
    float baseDamage = 100.0f;
    float range = 5.0f;
    int startupFrames = 10;
    int activeFrames = 3;
    int recoveryFrames = 15;
    
    // Special properties
    bool isProjectile = false;
    bool hasInvincibility = false;
    bool canCombo = true;
};

// Base class for all characters
class CharacterBase {
public:
    CharacterBase(const std::string& name, CharacterCategory category);
    virtual ~CharacterBase() = default;
    
    // Core identity
    const std::string& GetName() const { return m_name; }
    CharacterCategory GetCategory() const { return m_category; }
    
    // Universal systems - all characters have these
    std::array<GearSkill, 8>& GetGearSkills() { return m_gearSkills; } // 4 gears x 2 skills
    
    // Specialized systems - only some characters have these
    StanceSystem* GetStanceSystem() { return m_stanceSystem.get(); }
    PetSystem* GetPetSystem() { return m_petSystem.get(); }
    EvolutionSystem* GetEvolutionSystem() { return m_evolutionSystem.get(); }
    TransformationSystem* GetTransformationSystem() { return m_transformationSystem.get(); }
    WeaponMasterySystem* GetWeaponMasterySystem() { return m_weaponMasterySystem.get(); }
    CultivationSystem* GetCultivationSystem() { return m_cultivationSystem.get(); }
    BlessingSystem* GetBlessingSystem() { return m_blessingSystem.get(); }
    
    // Enable specialized systems
    void EnableStanceSystem();
    void EnablePetSystem();
    void EnableEvolutionSystem();
    void EnableTransformationSystem();
    void EnableWeaponMasterySystem();
    void EnableCultivationSystem();
    void EnableBlessingSystem();
    
    // Update
    virtual void Update(float deltaTime);
    
protected:
    std::string m_name;
    CharacterCategory m_category;
    
    // Universal gear skills (4 gears × 2 skills = 8 total)
    std::array<GearSkill, 8> m_gearSkills;
    int m_currentGear = 0;
    
    // Optional specialized systems
    std::unique_ptr<StanceSystem> m_stanceSystem;
    std::unique_ptr<PetSystem> m_petSystem;
    std::unique_ptr<EvolutionSystem> m_evolutionSystem;
    std::unique_ptr<TransformationSystem> m_transformationSystem;
    std::unique_ptr<WeaponMasterySystem> m_weaponMasterySystem;
    std::unique_ptr<CultivationSystem> m_cultivationSystem;
    std::unique_ptr<BlessingSystem> m_blessingSystem;
};

// Character factory for creating specific characters
class CharacterFactory {
public:
    static std::unique_ptr<CharacterBase> CreateCharacter(const std::string& characterType);
    
    // Murim characters
    static std::unique_ptr<CharacterBase> CreateSeoJunho();      // Frost martial artist with stances
    static std::unique_ptr<CharacterBase> CreateNanomancer();    // Tech cultivator with pet nanobots
    
    // Cultivation characters  
    static std::unique_ptr<CharacterBase> CreateSuPing();        // Pet store owner with multiple pets
    static std::unique_ptr<CharacterBase> CreateGaoPeng();       // Monster evolution specialist
    
    // System characters
    static std::unique_ptr<CharacterBase> CreateRou();           // Evolution and devour abilities
    static std::unique_ptr<CharacterBase> CreateSystemUser();    // Generic system abilities
    
    // Gods/Heroes characters
    static std::unique_ptr<CharacterBase> CreateDivineWarrior(); // Blessing and transformation
    static std::unique_ptr<CharacterBase> CreateMythicHero();    // Weapon mastery focus
};

} // namespace ArenaFighter