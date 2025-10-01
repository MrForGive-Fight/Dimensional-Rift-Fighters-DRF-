#include "HyukWoonSung.h"
#include <iostream>

using namespace ArenaFighter;

int main() {
    // Create instance of Hyuk Woon Sung
    HyukWoonSung character;
    
    // Test initialization
    character.Initialize();
    
    // Display character info
    std::cout << "Character: " << character.GetName() << std::endl;
    std::cout << "Category: " << static_cast<int>(character.GetCategory()) << std::endl;
    std::cout << "Health: " << character.GetMaxHealth() << std::endl;
    std::cout << "Mana: " << character.GetMaxMana() << std::endl;
    std::cout << "Has Stance System: " << (character.HasStanceSystem() ? "Yes" : "No") << std::endl;
    
    // Test stance switching
    std::cout << "\nCurrent Stance: " << character.GetCurrentStance() << std::endl;
    character.SwitchStance(0);
    std::cout << "After Switch: " << character.GetCurrentStance() << std::endl;
    
    // Test skills
    std::cout << "\nGear Skills:" << std::endl;
    const auto& skills = character.GetGearSkills();
    for (int i = 0; i < 8; ++i) {
        std::cout << "  " << i << ": " << skills[i].name 
                  << " (Mana: " << skills[i].manaCost << ")" << std::endl;
    }
    
    // Test special moves
    std::cout << "\nTesting Special Moves..." << std::endl;
    if (character.CanAffordSkill(25.0f)) {
        character.SpearSeaImpact();
        std::cout << "  Executed Spear Sea Impact" << std::endl;
    }
    
    // Test gauge
    std::cout << "\nTempered Gauge: " << character.GetTemperedGauge() << std::endl;
    character.BuildGauge(50.0f);
    std::cout << "After building: " << character.GetTemperedGauge() << std::endl;
    
    // Test update
    character.Update(0.016f); // 60 FPS frame
    
    std::cout << "\nCharacter test completed successfully!" << std::endl;
    
    return 0;
}