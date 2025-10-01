#pragma once

#include "../../src/Characters/CharacterBase.h"
#include "../../src/Combat/CombatEnums.h"

namespace ArenaFighter {

/**
 * @class {CHARACTER_NAME}
 * @brief {CHARACTER_DESCRIPTION}
 * 
 * Category: {CATEGORY}
 * Specialization: {SPECIALIZATION}
 * 
 * Character Overview:
 * - Primary playstyle: {PLAYSTYLE}
 * - Strengths: {STRENGTHS}
 * - Weaknesses: {WEAKNESSES}
 */
class {CHARACTER_NAME} : public CharacterBase {
public:
    {CHARACTER_NAME}();
    virtual ~{CHARACTER_NAME}() = default;
    
    // Core gameplay overrides
    virtual void Update(float deltaTime) override;
    virtual void OnDamageDealt(float damage, CharacterBase* target) override;
    virtual void OnDamageTaken(float damage, CharacterBase* attacker) override;
    virtual void OnComboExtend(int comboCount) override;
    virtual void OnStateChange(CharacterState newState, CharacterState oldState) override;
    
    // Character-specific mechanics
    void Initialize{SPECIALIZATION}System();
    void Update{SPECIALIZATION}State(float deltaTime);
    
protected:
    // Configure gear skills according to LSFDC standards
    void InitializeGearSkills();
    
    // Character-specific state
    struct {SPECIALIZATION}State {
        // Add specialization-specific variables
    } m_{SPECIALIZATION_LOWER}State;
    
private:
    // Gear 1: {GEAR_1_NAME}
    void ConfigureGear1Skills();
    // Skill 1: {GEAR_1_SKILL_1} - {DESCRIPTION}
    // Skill 2: {GEAR_1_SKILL_2} - {DESCRIPTION}
    
    // Gear 2: {GEAR_2_NAME}
    void ConfigureGear2Skills();
    // Skill 1: {GEAR_2_SKILL_1} - {DESCRIPTION}
    // Skill 2: {GEAR_2_SKILL_2} - {DESCRIPTION}
    
    // Gear 3: {GEAR_3_NAME}
    void ConfigureGear3Skills();
    // Skill 1: {GEAR_3_SKILL_1} - {DESCRIPTION}
    // Skill 2: {GEAR_3_SKILL_2} - {DESCRIPTION}
    
    // Gear 4: {GEAR_4_NAME}
    void ConfigureGear4Skills();
    // Skill 1: {GEAR_4_SKILL_1} - {DESCRIPTION}
    // Skill 2: {GEAR_4_SKILL_2} - {DESCRIPTION}
};

} // namespace ArenaFighter