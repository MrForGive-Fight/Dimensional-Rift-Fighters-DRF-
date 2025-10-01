#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace ArenaFighter {

enum class StanceType {
    Neutral,
    Offensive,    // High damage, low defense
    Defensive,    // High defense, low damage  
    Balanced,     // Balanced stats
    Counter,      // Counter-attack focused
    Speed,        // High speed, low power
    Power,        // High power, low speed
    Custom        // Character-specific stances
};

struct StanceModifiers {
    float damageMultiplier = 1.0f;
    float defenseMultiplier = 1.0f;
    float speedMultiplier = 1.0f;
    float manaRegenMultiplier = 1.0f;
    
    // Special properties
    bool hasCounterFrames = false;
    bool hasHyperArmor = false;
    bool drainsStamina = false;
    float staminaDrainRate = 0.0f;
};

class Stance {
public:
    Stance(const std::string& name, StanceType type, const StanceModifiers& mods)
        : m_name(name), m_type(type), m_modifiers(mods) {}
    
    const std::string& GetName() const { return m_name; }
    StanceType GetType() const { return m_type; }
    const StanceModifiers& GetModifiers() const { return m_modifiers; }
    
    // Stance-specific abilities
    void SetSpecialAbility(std::function<void()> ability) { m_specialAbility = ability; }
    void ActivateSpecialAbility() { if (m_specialAbility) m_specialAbility(); }
    
private:
    std::string m_name;
    StanceType m_type;
    StanceModifiers m_modifiers;
    std::function<void()> m_specialAbility;
};

class StanceSystem {
public:
    StanceSystem();
    
    // Stance management
    void AddStance(const std::string& name, const Stance& stance);
    bool SwitchStance(const std::string& stanceName);
    void SwitchToNextStance();
    void SwitchToPreviousStance();
    
    // Current stance info
    const Stance* GetCurrentStance() const;
    const StanceModifiers& GetCurrentModifiers() const;
    
    // Stance transitions
    void SetTransitionTime(float time) { m_transitionTime = time; }
    bool IsTransitioning() const { return m_transitionTimer > 0; }
    
    // Special mechanics
    void EnableStanceCombo(bool enable) { m_canComboStances = enable; }
    void SetStanceChain(const std::vector<std::string>& chain) { m_stanceChain = chain; }
    
    void Update(float deltaTime);
    
    // Character-specific stance examples
    static StanceSystem* CreateFrostStanceSystem();     // For Seo Jun-ho
    static StanceSystem* CreateSwordStanceSystem();     // For sword masters
    static StanceSystem* CreateMartialArtsSystem();    // For hand-to-hand fighters
    
private:
    std::unordered_map<std::string, Stance> m_stances;
    std::string m_currentStanceName = "Neutral";
    
    // Transition system
    float m_transitionTime = 0.5f;
    float m_transitionTimer = 0.0f;
    std::string m_nextStance;
    
    // Advanced mechanics
    bool m_canComboStances = false;
    std::vector<std::string> m_stanceChain;
    int m_currentChainIndex = 0;
    
    // Stamina system for stance maintenance
    float m_stanceStamina = 100.0f;
    float m_maxStanceStamina = 100.0f;
};

// Example implementation for Seo Jun-ho's Frost Stances
namespace FrostStances {
    const Stance IceWall("Ice Wall", StanceType::Defensive, {
        0.7f,   // damageMultiplier
        1.5f,   // defenseMultiplier  
        0.9f,   // speedMultiplier
        1.0f,   // manaRegenMultiplier
        false,  // hasCounterFrames
        true,   // hasHyperArmor
        false,  // drainsStamina
        0.0f    // staminaDrainRate
    });
    
    const Stance FrostBlade("Frost Blade", StanceType::Offensive, {
        1.4f,   // damageMultiplier
        0.8f,   // defenseMultiplier
        1.1f,   // speedMultiplier
        0.8f,   // manaRegenMultiplier
        false,  // hasCounterFrames
        false,  // hasHyperArmor
        true,   // drainsStamina
        2.0f    // staminaDrainRate
    });
    
    const Stance AbsoluteZero("Absolute Zero", StanceType::Counter, {
        1.2f,   // damageMultiplier
        1.0f,   // defenseMultiplier
        0.7f,   // speedMultiplier
        1.5f,   // manaRegenMultiplier
        true,   // hasCounterFrames
        false,  // hasHyperArmor
        true,   // drainsStamina
        3.0f    // staminaDrainRate
    });
}

} // namespace ArenaFighter