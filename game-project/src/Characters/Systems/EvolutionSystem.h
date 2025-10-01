#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

namespace ArenaFighter {

enum class EvolutionTrigger {
    Level,          // Reach specific level
    Combat,         // Win certain battles
    Devour,         // Consume enemies/items
    Time,           // Time-based evolution
    Damage,         // Take/deal damage threshold
    Special,        // Special conditions
    Item,           // Use evolution item
    Fusion          // Fuse with another entity
};

struct EvolutionRequirement {
    EvolutionTrigger trigger;
    float value;                    // Level needed, battles won, etc.
    std::string specificTarget;     // For devour/item requirements
    bool isMet = false;
};

struct EvolutionPath {
    std::string name;
    std::string description;
    std::vector<EvolutionRequirement> requirements;
    
    // Stat changes
    float healthMultiplier = 1.2f;
    float attackMultiplier = 1.2f;
    float defenseMultiplier = 1.2f;
    float speedMultiplier = 1.1f;
    
    // New abilities unlocked
    std::vector<std::string> newSkills;
    std::vector<std::string> enhancedSkills;
    
    // Visual changes
    std::string newModel;
    std::string effectsVFX;
    float sizeMultiplier = 1.0f;
};

class Evolution {
public:
    Evolution(const std::string& name, int tier);
    
    const std::string& GetName() const { return m_name; }
    int GetTier() const { return m_tier; }
    
    // Add possible evolution paths
    void AddEvolutionPath(const EvolutionPath& path);
    std::vector<EvolutionPath>& GetAvailablePaths() { return m_paths; }
    
    // Check evolution readiness
    bool CanEvolve() const;
    const EvolutionPath* GetReadyPath() const;
    
    // Apply evolution
    void Evolve(const std::string& pathName);
    
private:
    std::string m_name;
    int m_tier;  // Evolution tier/stage
    std::vector<EvolutionPath> m_paths;
    std::string m_currentPath;
};

class EvolutionSystem {
public:
    EvolutionSystem();
    
    // Evolution tree management
    void SetBaseForm(const std::string& name);
    void AddEvolution(std::unique_ptr<Evolution> evolution);
    
    // Current evolution state
    Evolution* GetCurrentEvolution() { return m_currentEvolution; }
    int GetCurrentTier() const { return m_currentEvolution ? m_currentEvolution->GetTier() : 0; }
    
    // Evolution progress
    void UpdateRequirement(EvolutionTrigger trigger, float value, const std::string& target = "");
    bool CheckEvolutionAvailable();
    std::vector<std::string> GetAvailableEvolutions();
    
    // Perform evolution
    bool Evolve(const std::string& evolutionPath);
    void ForceEvolve(const std::string& evolutionName); // For special events
    
    // Devour system (for Rou-like characters)
    void DevourEntity(const std::string& entityType, float power);
    void AddDevourAbility(const std::string& ability);
    std::vector<std::string> GetDevouredAbilities() const { return m_devouredAbilities; }
    
    // Stat tracking
    float GetTotalStatMultiplier(const std::string& stat) const;
    
    // Update
    void Update(float deltaTime);
    
    // Character-specific evolution systems
    static EvolutionSystem* CreateRouEvolutionSystem();      // Re:Monster style
    static EvolutionSystem* CreateMonsterEvolution();       // Generic monster
    static EvolutionSystem* CreateDivineEvolution();        // God transformation
    static EvolutionSystem* CreateMechanicalEvolution();    // Robot/cyborg
    
private:
    std::unordered_map<std::string, std::unique_ptr<Evolution>> m_evolutionTree;
    Evolution* m_currentEvolution = nullptr;
    
    // Devour system
    std::vector<std::string> m_devouredAbilities;
    std::unordered_map<std::string, int> m_devourCounts;
    static constexpr int DEVOUR_THRESHOLD = 10; // Entities needed for ability
    
    // Evolution history
    std::vector<std::string> m_evolutionHistory;
    
    // Temporary evolution (power-ups)
    struct TempEvolution {
        std::string name;
        float duration;
        float remaining;
    };
    std::vector<TempEvolution> m_tempEvolutions;
};

// Example evolutions for Rou (Re:Monster)
namespace RouEvolutions {
    std::unique_ptr<Evolution> CreateGoblin() {
        auto evo = std::make_unique<Evolution>("Goblin", 1);
        
        EvolutionPath toHobgoblin;
        toHobgoblin.name = "Hobgoblin";
        toHobgoblin.requirements = {
            {EvolutionTrigger::Level, 15.0f},
            {EvolutionTrigger::Combat, 50.0f}
        };
        toHobgoblin.healthMultiplier = 1.5f;
        toHobgoblin.attackMultiplier = 1.3f;
        toHobgoblin.newSkills = {"Enhanced Strength", "Battle Roar"};
        
        evo->AddEvolutionPath(toHobgoblin);
        return evo;
    }
    
    std::unique_ptr<Evolution> CreateHobgoblin() {
        auto evo = std::make_unique<Evolution>("Hobgoblin", 2);
        
        EvolutionPath toOgre;
        toOgre.name = "Ogre";
        toOgre.requirements = {
            {EvolutionTrigger::Level, 30.0f},
            {EvolutionTrigger::Devour, 5.0f, "Orc"}
        };
        toOgre.healthMultiplier = 2.0f;
        toOgre.attackMultiplier = 1.8f;
        toOgre.sizeMultiplier = 1.5f;
        toOgre.newSkills = {"Overwhelming Presence", "Earth Shaker"};
        
        EvolutionPath toVampire;
        toVampire.name = "Vampire Lord";
        toVampire.requirements = {
            {EvolutionTrigger::Devour, 3.0f, "Vampire"},
            {EvolutionTrigger::Special, 1.0f, "Night Battle"}
        };
        toVampire.speedMultiplier = 2.0f;
        toVampire.newSkills = {"Blood Drain", "Bat Form", "Charm"};
        
        evo->AddEvolutionPath(toOgre);
        evo->AddEvolutionPath(toVampire);
        return evo;
    }
}

} // namespace ArenaFighter