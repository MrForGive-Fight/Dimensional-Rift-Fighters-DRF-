#include "CharacterFactory.h"
#include "CharacterCategory.h"
#include "System/CyberNinja.h"
#include <iostream>
#include <cassert>

namespace ArenaFighter {

void TestCharacterSystem() {
    std::cout << "=== Character System Test ===" << std::endl;
    
    // Test 1: Category Manager
    auto& categoryMgr = CharacterCategoryManager::GetInstance();
    std::cout << "\nTesting Category Manager..." << std::endl;
    
    // Test category traits
    const auto& systemTraits = categoryMgr.GetCategoryTraits(CharacterCategory::System);
    std::cout << "System Category Theme: " << systemTraits.themeDescription << std::endl;
    std::cout << "Primary Color: " << systemTraits.primaryColor << std::endl;
    std::cout << "Mana Modifier: " << systemTraits.manaModifier << std::endl;
    
    // Test 2: Character Factory
    auto& factory = CharacterFactory::GetInstance();
    std::cout << "\nTesting Character Factory..." << std::endl;
    
    // Create Cyber Ninja
    auto cyberNinja = factory.CreateCharacter(103);
    if (cyberNinja) {
        std::cout << "Successfully created: " << cyberNinja->GetName() << std::endl;
        std::cout << "Category: " << static_cast<int>(cyberNinja->GetCategory()) << std::endl;
        std::cout << "Max Health: " << cyberNinja->GetMaxHealth() << std::endl;
        std::cout << "Max Mana: " << cyberNinja->GetMaxMana() << std::endl;
        std::cout << "Defense: " << cyberNinja->GetDefense() << std::endl;
        std::cout << "Speed: " << cyberNinja->GetSpeed() << std::endl;
        
        // Test gear skills
        const auto& skills = cyberNinja->GetGearSkills();
        std::cout << "\nGear Skills:" << std::endl;
        for (int i = 0; i < 8; ++i) {
            std::cout << "  Skill " << i+1 << ": " << skills[i].name 
                     << " (Mana: " << skills[i].manaCost << ")" << std::endl;
        }
    }
    
    // Test 3: Character Roster
    std::cout << "\nCharacter Roster by Category:" << std::endl;
    for (int cat = 0; cat < 7; ++cat) {
        auto category = static_cast<CharacterCategory>(cat);
        auto characters = factory.GetCharactersByCategory(category);
        std::cout << "\n" << categoryMgr.GetCategoryDescription(category) << ":" << std::endl;
        for (const auto& info : characters) {
            std::cout << "  - " << info.name << " (ID: " << info.id << ")" << std::endl;
        }
    }
    
    // Test 4: Stat Modes
    std::cout << "\nTesting Stat Modes..." << std::endl;
    float health = 1000, mana = 100, defense = 100, speed = 100, power = 1;
    
    // Test Attack mode
    categoryMgr.ApplyStatModeModifiers(StatMode::Attack, health, mana, defense, speed, power);
    std::cout << "Attack Mode - Power: " << power << ", Defense: " << defense << std::endl;
    
    // Reset and test Defense mode
    health = 1000; mana = 100; defense = 100; speed = 100; power = 1;
    categoryMgr.ApplyStatModeModifiers(StatMode::Defense, health, mana, defense, speed, power);
    std::cout << "Defense Mode - Defense: " << defense << ", Speed: " << speed << std::endl;
    
    std::cout << "\n=== Test Complete ===" << std::endl;
}

} // namespace ArenaFighter

// Simple main for testing
#ifdef CHARACTER_SYSTEM_TEST
int main() {
    ArenaFighter::CharacterFactory::GetInstance().InitializeDefaultCharacters();
    ArenaFighter::TestCharacterSystem();
    return 0;
}
#endif