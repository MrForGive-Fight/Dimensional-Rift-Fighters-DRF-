#include "../src/Characters/CharacterBase.h"
#include <iostream>

namespace ArenaFighter {

/**
 * Example character demonstrating the new special move system
 * 
 * This character has:
 * - Standard gear skills with cooldowns (AS, AD, ASD, SD)
 * - Special moves with mana-only cost (S+Direction)
 * - Different special moves for different stances
 */
class SpecialMoveExampleCharacter : public CharacterBase {
public:
    SpecialMoveExampleCharacter() 
        : CharacterBase("ExampleFighter", CharacterCategory::Murim, StatMode::Balanced) {
        
        // Initialize gear skills (with cooldowns)
        InitializeGearSkills();
        
        // Initialize special moves (mana only, no cooldowns)
        InitializeSpecialMoves();
    }
    
    void Initialize() override {
        CharacterBase::Initialize();
        std::cout << "ExampleFighter initialized with special moves!" << std::endl;
    }
    
    void OnSpecialMoveExecute(InputDirection direction) override {
        const SpecialMove* move = GetSpecialMove(direction);
        if (move) {
            std::cout << "Executing special move: " << move->name 
                      << " (Cost: " << move->manaCost << " mana)" << std::endl;
        }
    }
    
    void OnBlockStart() override {
        std::cout << "Started blocking - special moves disabled!" << std::endl;
    }
    
    void OnBlockEnd() override {
        std::cout << "Stopped blocking - special moves enabled!" << std::endl;
    }
    
private:
    void InitializeGearSkills() {
        // Gear 0: Basic Combat
        GearSkill skill1;
        skill1.name = "Quick Strike";
        skill1.manaCost = 15.0f;
        skill1.cooldown = 2.0f;  // 2 second cooldown
        skill1.baseDamage = 80.0f;
        skill1.attackType = AttackType::Light;
        skill1.startupFrames = 6;
        skill1.activeFrames = 3;
        skill1.recoveryFrames = 10;
        SetGearSkill(0, skill1);
        
        GearSkill skill2;
        skill2.name = "Power Blow";
        skill2.manaCost = 25.0f;
        skill2.cooldown = 4.0f;  // 4 second cooldown
        skill2.baseDamage = 150.0f;
        skill2.attackType = AttackType::Heavy;
        skill2.startupFrames = 20;
        skill2.activeFrames = 5;
        skill2.recoveryFrames = 30;
        SetGearSkill(1, skill2);
        
        // Additional gears would be initialized here...
    }
    
    void InitializeSpecialMoves() {
        // S+Up: Rising Dragon
        SpecialMove upSpecial;
        upSpecial.name = "Rising Dragon";
        upSpecial.manaCost = 30.0f;  // Mana only, no cooldown!
        upSpecial.baseDamage = 120.0f;
        upSpecial.attackType = AttackType::Special;
        upSpecial.startupFrames = 8;
        upSpecial.activeFrames = 5;
        upSpecial.recoveryFrames = 15;
        upSpecial.element = ElementType::Wind;
        RegisterSpecialMove(InputDirection::Up, upSpecial);
        
        // S+Down: Earth Crusher
        SpecialMove downSpecial;
        downSpecial.name = "Earth Crusher";
        downSpecial.manaCost = 25.0f;  // Mana only, no cooldown!
        downSpecial.baseDamage = 100.0f;
        downSpecial.attackType = AttackType::Special;
        downSpecial.startupFrames = 15;
        downSpecial.activeFrames = 8;
        downSpecial.recoveryFrames = 20;
        downSpecial.element = ElementType::Earth;
        RegisterSpecialMove(InputDirection::Down, downSpecial);
        
        // S+Left: Shadow Step
        SpecialMove leftSpecial;
        leftSpecial.name = "Shadow Step";
        leftSpecial.manaCost = 20.0f;  // Mana only, no cooldown!
        leftSpecial.baseDamage = 60.0f;
        leftSpecial.attackType = AttackType::Special;
        leftSpecial.startupFrames = 5;
        leftSpecial.activeFrames = 3;
        leftSpecial.recoveryFrames = 8;
        leftSpecial.canCombo = true;
        RegisterSpecialMove(InputDirection::Left, leftSpecial);
        
        // S+Right: Ki Blast
        SpecialMove rightSpecial;
        rightSpecial.name = "Ki Blast";
        rightSpecial.manaCost = 35.0f;  // Mana only, no cooldown!
        rightSpecial.baseDamage = 140.0f;
        rightSpecial.attackType = AttackType::Special;
        rightSpecial.startupFrames = 12;
        rightSpecial.activeFrames = 4;
        rightSpecial.recoveryFrames = 18;
        rightSpecial.isProjectile = true;
        rightSpecial.element = ElementType::Light;
        RegisterSpecialMove(InputDirection::Right, rightSpecial);
    }
};

/**
 * Example of a stance-based character with stance-specific special moves
 */
class StanceCharacterExample : public CharacterBase {
public:
    StanceCharacterExample() 
        : CharacterBase("StanceMaster", CharacterCategory::Murim, StatMode::Technical) {
        m_currentStance = 0;  // Start in stance 0
        InitializeStanceSpecialMoves();
    }
    
    bool HasStanceSystem() const override { return true; }
    int GetCurrentStance() const override { return m_currentStance; }
    
    void SwitchStance(int stanceIndex) override {
        if (stanceIndex >= 0 && stanceIndex < 3) {
            m_currentStance = stanceIndex;
            std::cout << "Switched to stance " << stanceIndex << std::endl;
        }
    }
    
private:
    int m_currentStance;
    
    void InitializeStanceSpecialMoves() {
        // Stance 0: Offensive stance special moves
        SpecialMove offensiveUp;
        offensiveUp.name = "Heavenly Strike";
        offensiveUp.manaCost = 40.0f;
        offensiveUp.baseDamage = 180.0f;
        offensiveUp.attackType = AttackType::Special;
        offensiveUp.requiredStance = 0;  // Only in offensive stance
        offensiveUp.startupFrames = 10;
        offensiveUp.activeFrames = 6;
        offensiveUp.recoveryFrames = 20;
        RegisterSpecialMove(InputDirection::Up, offensiveUp);
        
        // Stance 1: Defensive stance special moves
        SpecialMove defensiveDown;
        defensiveDown.name = "Iron Wall";
        defensiveDown.manaCost = 15.0f;
        defensiveDown.baseDamage = 0.0f;  // Defensive move
        defensiveDown.attackType = AttackType::Special;
        defensiveDown.requiredStance = 1;  // Only in defensive stance
        defensiveDown.blockable = false;   // Creates a shield
        defensiveDown.startupFrames = 3;
        defensiveDown.activeFrames = 60;   // 1 second of protection
        defensiveDown.recoveryFrames = 10;
        RegisterSpecialMove(InputDirection::Down, defensiveDown);
        
        // Add more stance-specific moves as needed...
    }
};

// Example usage and testing
void DemonstrateSpecialMoveSystem() {
    std::cout << "=== Special Move System Demonstration ===" << std::endl;
    
    SpecialMoveExampleCharacter fighter;
    fighter.Initialize();
    
    // Show initial state
    std::cout << "\nInitial Mana: " << fighter.GetCurrentMana() << "/" << fighter.GetMaxMana() << std::endl;
    
    // Test special move execution
    std::cout << "\n--- Testing Special Moves (Mana Only) ---" << std::endl;
    if (fighter.CanExecuteSpecialMove(InputDirection::Up)) {
        fighter.ExecuteSpecialMove(InputDirection::Up);
        std::cout << "Remaining Mana: " << fighter.GetCurrentMana() << std::endl;
    }
    
    // Test blocking prevents special moves
    std::cout << "\n--- Testing Blocking ---" << std::endl;
    fighter.StartBlocking();
    if (!fighter.CanExecuteSpecialMove(InputDirection::Down)) {
        std::cout << "Cannot use special moves while blocking!" << std::endl;
    }
    fighter.StopBlocking();
    
    // Test gear skills with cooldowns
    std::cout << "\n--- Testing Gear Skills (With Cooldowns) ---" << std::endl;
    const GearSkill& skill1 = fighter.GetGearSkill1();
    std::cout << "Gear Skill 1: " << skill1.name 
              << " (Mana: " << skill1.manaCost 
              << ", Cooldown: " << skill1.cooldown << "s)" << std::endl;
    
    // Simulate using the skill
    if (!fighter.IsGearSkillOnCooldown(0)) {
        std::cout << "Using " << skill1.name << "..." << std::endl;
        fighter.StartGearSkillCooldown(0);
        std::cout << "Skill is now on cooldown for " << skill1.cooldown << " seconds" << std::endl;
    }
    
    // Test stance character
    std::cout << "\n--- Testing Stance-Based Special Moves ---" << std::endl;
    StanceCharacterExample stanceMaster;
    stanceMaster.Initialize();
    
    // Try to use stance-specific move
    if (!stanceMaster.CanExecuteSpecialMove(InputDirection::Down)) {
        std::cout << "Cannot use defensive stance move in offensive stance!" << std::endl;
    }
    
    // Switch stance and try again
    stanceMaster.SwitchStance(1);  // Switch to defensive stance
    if (stanceMaster.CanExecuteSpecialMove(InputDirection::Down)) {
        std::cout << "Can now use defensive stance special move!" << std::endl;
    }
}

} // namespace ArenaFighter

// Main function for testing
int main() {
    ArenaFighter::DemonstrateSpecialMoveSystem();
    return 0;
}