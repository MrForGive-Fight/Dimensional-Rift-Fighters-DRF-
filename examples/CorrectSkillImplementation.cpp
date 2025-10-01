/**
 * @file CorrectSkillImplementation.cpp
 * @brief Example showing the CORRECT way to implement DFR's dual skill system
 */

#include "../src/Characters/CharacterBase.h"
#include "../src/Characters/SkillValidation.h"
#include <iostream>

using namespace ArenaFighter;

/**
 * Example character demonstrating proper skill implementation
 */
class ExampleCharacter : public CharacterBase {
public:
    ExampleCharacter() 
        : CharacterBase("Example Hero", CharacterCategory::System) {
        InitializeSkills();
    }
    
    void InitializeSkills() {
        // =========================================================
        // SPECIAL MOVES (S+Direction) - MANA ONLY, NO COOLDOWNS
        // =========================================================
        
        // S+Up: Vertical Strike
        SpecialMove verticalStrike;
        verticalStrike.name = "Vertical Strike";
        verticalStrike.animation = "VerticalStrike";
        verticalStrike.manaCost = 25.0f;         // MANA COST ONLY
        verticalStrike.baseDamage = 120.0f;
        verticalStrike.attackType = AttackType::Special;
        verticalStrike.startupFrames = 12;
        verticalStrike.activeFrames = 8;
        verticalStrike.recoveryFrames = 15;
        verticalStrike.element = ElementType::Neutral;
        // NO COOLDOWN FIELD - This is correct!
        
        RegisterSpecialMove(InputDirection::Up, verticalStrike);
        
        // S+Right: Dash Attack
        SpecialMove dashAttack;
        dashAttack.name = "Dash Attack";
        dashAttack.animation = "DashAttack";
        dashAttack.manaCost = 20.0f;            // MANA COST ONLY
        dashAttack.baseDamage = 80.0f;
        dashAttack.attackType = AttackType::Light;
        dashAttack.startupFrames = 8;
        dashAttack.activeFrames = 6;
        dashAttack.recoveryFrames = 12;
        dashAttack.isProjectile = false;
        // NO COOLDOWN - Can spam if you have mana!
        
        RegisterSpecialMove(InputDirection::Right, dashAttack);
        
        // S+Left: Counter Strike
        SpecialMove counterStrike;
        counterStrike.name = "Counter Strike";
        counterStrike.animation = "CounterStrike";
        counterStrike.manaCost = 30.0f;         // MANA COST ONLY
        counterStrike.baseDamage = 150.0f;
        counterStrike.attackType = AttackType::Medium;
        counterStrike.startupFrames = 15;
        counterStrike.activeFrames = 4;
        counterStrike.recoveryFrames = 20;
        counterStrike.blockable = false;        // Unblockable
        
        RegisterSpecialMove(InputDirection::Left, counterStrike);
        
        // S+Down: Ground Pound
        SpecialMove groundPound;
        groundPound.name = "Ground Pound";
        groundPound.animation = "GroundPound";
        groundPound.manaCost = 35.0f;           // MANA COST ONLY
        groundPound.baseDamage = 140.0f;
        groundPound.attackType = AttackType::Heavy;
        groundPound.startupFrames = 20;
        groundPound.activeFrames = 10;
        groundPound.recoveryFrames = 25;
        groundPound.element = ElementType::Earth;
        
        RegisterSpecialMove(InputDirection::Down, groundPound);
        
        // =========================================================
        // GEAR SKILLS (AS, AD, ASD, SD) - MANA + COOLDOWNS
        // =========================================================
        
        // Gear 1 - Weapon Skills (SD inputs)
        m_gearSkills[0] = {
            "Lightning Slash",           // name
            "LightningSlashAnim",       // animation
            25.0f,                      // manaCost
            180.0f,                     // baseDamage
            8.0f,                       // COOLDOWN (seconds) - REQUIRED!
            AttackType::Heavy,          // attackType
            15, 12, 20,                // frame data
            true,                       // isProjectile
            true,                       // canCombo
            ElementType::Lightning      // element
        };
        
        m_gearSkills[1] = {
            "Sword Barrier",            // name
            "SwordBarrierAnim",         // animation
            30.0f,                      // manaCost
            0.0f,                       // baseDamage (defensive)
            12.0f,                      // COOLDOWN - Gear skills need cooldowns!
            AttackType::Special,        // attackType
            10, 180, 10,               // frame data (3 sec active)
            false,                      // isProjectile
            false,                      // canCombo
            ElementType::Neutral        // element
        };
        
        // Gear 2 - Helmet Skills (AS inputs)
        m_gearSkills[2] = {
            "Mind Blast",               // name
            "MindBlastAnim",           // animation
            35.0f,                      // manaCost
            150.0f,                     // baseDamage
            15.0f,                      // COOLDOWN - Long cooldown for powerful skill
            AttackType::Special,        // attackType
            20, 8, 25,                 // frame data
            true,                       // isProjectile
            false,                      // canCombo
            ElementType::Void           // element
        };
        
        m_gearSkills[3] = {
            "Focus",                    // name
            "FocusAnim",               // animation
            20.0f,                      // manaCost
            0.0f,                       // baseDamage
            10.0f,                      // COOLDOWN
            AttackType::Special,        // attackType
            15, 60, 0,                 // frame data
            false,                      // isProjectile
            false,                      // canCombo
            ElementType::Light          // element
        };
        
        // Continue with remaining gear skills...
        // All gear skills MUST have cooldowns!
    }
};

// Example usage showing the difference
void DemonstrateSkillSystem() {
    ExampleCharacter hero;
    
    std::cout << "=== DFR Skill System Demonstration ===" << std::endl;
    std::cout << "\nSPECIAL MOVES (S+Direction) - MANA ONLY:" << std::endl;
    
    // Special moves can be used repeatedly if you have mana
    for (int i = 0; i < 5; ++i) {
        if (hero.CanExecuteSpecialMove(InputDirection::Up)) {
            std::cout << "Frame " << i * 60 << ": Executing Vertical Strike (25 mana)" << std::endl;
            hero.ExecuteSpecialMove(InputDirection::Up);
        } else {
            std::cout << "Frame " << i * 60 << ": Not enough mana!" << std::endl;
        }
        
        // Simulate 1 second passing (mana regen)
        hero.Update(1.0f);
    }
    
    std::cout << "\nGEAR SKILLS (AS, AD, ASD, SD) - MANA + COOLDOWNS:" << std::endl;
    
    // Gear skills have cooldowns
    for (int i = 0; i < 3; ++i) {
        if (!hero.IsGearSkillOnCooldown(0) && hero.CanAffordSkill(25.0f)) {
            std::cout << "Frame " << i * 300 << ": Executing Lightning Slash (8s cooldown)" << std::endl;
            hero.StartGearSkillCooldown(0);
            hero.ConsumeMana(25.0f);
        } else {
            float cd = hero.GetGearSkillCooldownRemaining(0);
            if (cd > 0) {
                std::cout << "Frame " << i * 300 << ": Lightning Slash on cooldown (" 
                         << cd << "s remaining)" << std::endl;
            }
        }
        
        // Simulate 5 seconds passing
        hero.Update(5.0f);
    }
    
    std::cout << "\nKEY DIFFERENCE:" << std::endl;
    std::cout << "- Special moves: Limited ONLY by mana (5/sec regen)" << std::endl;
    std::cout << "- Gear skills: Limited by BOTH mana AND cooldowns" << std::endl;
}

// Validation example
void ValidateImplementation() {
    ExampleCharacter hero;
    
    try {
        SkillValidation::ValidateCharacterSkills(&hero);
        std::cout << "\n✅ Character skills validated successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "\n❌ Validation failed: " << e.what() << std::endl;
    }
}

int main() {
    DemonstrateSkillSystem();
    ValidateImplementation();
    
    return 0;
}