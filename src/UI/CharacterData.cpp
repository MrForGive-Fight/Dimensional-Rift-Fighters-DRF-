#include "CharacterData.h"

namespace ArenaFighter {

CharacterData::CharacterData() 
    : m_level(1), 
      m_owned(false), 
      m_portraitTexture(nullptr), 
      m_iconTexture(nullptr),
      m_health(BASE_HEALTH),
      m_mana(BASE_MANA),
      m_defense(BASE_DEFENSE),
      m_speed(BASE_SPEED),
      m_attack(0), 
      m_special(0) {
    m_gears.reserve(4);
}

void CharacterData::initializeDefaultGears() {
    // Each character gets 4 gears with 2 skills each
    for (int i = 0; i < 4; i++) {
        GearData gear;
        gear.m_id = m_id + "_gear" + std::to_string(i + 1);
        gear.m_name = "Gear " + std::to_string(i + 1);
        
        // Add 2 skills per gear
        for (int j = 0; j < 2; j++) {
            SkillData skill;
            skill.m_id = gear.m_id + "_skill" + std::to_string(j + 1);
            skill.m_name = "Skill " + std::to_string(j + 1);
            
            // Assign mana costs based on skill slot
            if (i == 0) {
                // Gear 1: Basic attacks
                skill.m_manaCost = (j == 0) ? 5.0f : 10.0f;
                skill.m_baseDamage = (j == 0) ? 50.0f : 75.0f;
                skill.m_startupFrames = (j == 0) ? 5 : 8;
                skill.m_activeFrames = (j == 0) ? 2 : 3;
                skill.m_recoveryFrames = (j == 0) ? 8 : 10;
            } else if (i == 1) {
                // Gear 2: Medium attacks
                skill.m_manaCost = (j == 0) ? 15.0f : 20.0f;
                skill.m_baseDamage = (j == 0) ? 100.0f : 150.0f;
                skill.m_startupFrames = (j == 0) ? 10 : 12;
                skill.m_activeFrames = (j == 0) ? 3 : 4;
                skill.m_recoveryFrames = (j == 0) ? 15 : 18;
            } else if (i == 2) {
                // Gear 3: Special moves
                skill.m_manaCost = (j == 0) ? 25.0f : 35.0f;
                skill.m_baseDamage = (j == 0) ? 200.0f : 250.0f;
                skill.m_startupFrames = (j == 0) ? 15 : 18;
                skill.m_activeFrames = (j == 0) ? 5 : 6;
                skill.m_recoveryFrames = (j == 0) ? 20 : 25;
            } else {
                // Gear 4: Ultimate skills
                skill.m_manaCost = (j == 0) ? 50.0f : 70.0f;
                skill.m_baseDamage = (j == 0) ? 350.0f : 500.0f;
                skill.m_startupFrames = (j == 0) ? 20 : 25;
                skill.m_activeFrames = (j == 0) ? 8 : 10;
                skill.m_recoveryFrames = (j == 0) ? 30 : 35;
            }
            
            skill.m_range = 100.0f * (i + 1); // Range increases with gear level
            gear.m_skills.push_back(skill);
        }
        
        m_gears.push_back(gear);
    }
}

} // namespace ArenaFighter