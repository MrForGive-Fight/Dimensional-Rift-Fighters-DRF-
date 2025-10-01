#pragma once

#include "../CharacterBase.h"
#include "../CharacterFactory.h"

namespace ArenaFighter {

/**
 * @brief Cyber Ninja - System category stealth assassin
 * 
 * Uses mana-based skills with no cooldowns per CLAUDE.md
 * Specializes in stealth, hacking, and precision strikes
 */
class CyberNinja : public CharacterBase {
public:
    CyberNinja();
    ~CyberNinja() = default;
    
    // Override character behavior
    void Initialize() override;
    void Update(float deltaTime) override;
    void OnGearSwitch(int oldGear, int newGear) override;
    void OnSkillUse(int skillIndex) override;
    
    // Cyber Ninja specific abilities
    void EnterStealthMode();
    void ExitStealthMode();
    bool IsStealthed() const { return m_isStealthed; }
    
    // Hacking abilities
    void InitiateHack(CharacterBase* target);
    bool IsHacking() const { return m_hackTarget != nullptr; }
    
private:
    // Initialize gear skills
    void InitializeGearSkills();
    
    // State management
    bool m_isStealthed;
    float m_stealthDuration;
    float m_hackProgress;
    CharacterBase* m_hackTarget;
    
    // Visual effects
    float m_digitalParticleTimer;
    bool m_cloakingActive;
};

} // namespace ArenaFighter