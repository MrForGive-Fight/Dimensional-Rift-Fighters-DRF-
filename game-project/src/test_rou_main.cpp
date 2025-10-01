#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include "../../src/Monsters/Rou/Rou.h"
#include "../../src/Monsters/Rou/Visuals/EvolutionVFX.h"
#include "../../src/Murim/HyukWoonSung/HyukWoonSung.h"
#include "../../src/Murim/HyukWoonSung/Visuals/StanceVFX.h"

using namespace ArenaFighter;

void printHyukStatus(HyukWoonSung* hyuk) {
    std::cout << "\n=== Hyuk Woon Sung Status ===" << std::endl;
    std::cout << "Current Stance: " << (hyuk->GetCurrentStance() == StanceType::LIGHT_STANCE ? "Light (Orthodox Spear)" : "Dark (Heavenly Demon Arts)") << std::endl;
    std::cout << "Qi Energy: " << hyuk->GetQiEnergy() << "/100" << std::endl;
    std::cout << "HP: " << hyuk->GetHPPercent() << "%" << std::endl;
    std::cout << "Tempered Blossom Gauge: " << hyuk->GetTemperedBlossomGauge() << "%" << std::endl;
    std::cout << "Ultimate Status: " << (hyuk->IsInUltimate() ? "ACTIVE" : "Inactive") << std::endl;
    std::cout << "=========================" << std::endl;
}

void testHyukStanceSystem() {
    std::cout << "\n=== Testing Hyuk Woon Sung Stance System ===" << std::endl;
    
    auto hyuk = std::make_unique<HyukWoonSung>();
    auto vfx = std::make_unique<StanceVFX>();
    
    // Initial state
    printHyukStatus(hyuk.get());
    
    // Test stance switching
    std::cout << "\n1. Testing stance switching..." << std::endl;
    hyuk->SwitchStance();
    printHyukStatus(hyuk.get());
    
    hyuk->Update(1.1f); // Wait for cooldown
    hyuk->SwitchStance();
    printHyukStatus(hyuk.get());
    
    // Test Light stance combat
    std::cout << "\n2. Testing Light stance combat..." << std::endl;
    std::cout << "Seven Stars of the Spear:" << std::endl;
    hyuk->SevenStarsOfTheSpear();
    
    std::cout << "Divine Spear of Ending Night:" << std::endl;
    hyuk->DivineSpearOfEndingNight();
    
    std::cout << "S+Direction skills:" << std::endl;
    hyuk->ExecuteDirectionalSkill(Direction::UP);    // Spear Sea Impact
    hyuk->ExecuteDirectionalSkill(Direction::RIGHT); // Divine Wind
    hyuk->ExecuteDirectionalSkill(Direction::LEFT);  // Lightning Stitch
    
    // Test Dark stance combat
    std::cout << "\n3. Testing Dark stance combat..." << std::endl;
    hyuk->SwitchStance();
    hyuk->Update(1.1f);
    printHyukStatus(hyuk.get());
    
    std::cout << "Heavenly Demon Divine Palm:" << std::endl;
    hyuk->HeavenlyDemonDivinePalm();
    
    std::cout << "Red Soul Charge:" << std::endl;
    hyuk->RedSoulCharge();
    
    std::cout << "THE BIG METEOR!" << std::endl;
    hyuk->RainOfStarDestruction();
    printHyukStatus(hyuk.get());
    
    // Test gear skills
    std::cout << "\n4. Testing gear skills..." << std::endl;
    std::cout << "Dark stance gear skills:" << std::endl;
    hyuk->HeavenlyDemonDestroysTheWorld(); // AS
    hyuk->IntimidationDress();             // AD
    hyuk->DarkFlowerRedHeartFlame();       // ASD
    
    // Switch back to light
    hyuk->SwitchStance();
    hyuk->Update(1.1f);
    std::cout << "Light stance gear skills:" << std::endl;
    hyuk->GlassyDeathRain();      // AS
    hyuk->SpearAura();            // AD
    hyuk->FlowOfTheDivineDragon(); // ASD
    hyuk->ThunderousFlyingSpear(); // SD
    
    printHyukStatus(hyuk.get());
    
    // Test Ultimate system
    std::cout << "\n5. Testing Ultimate system..." << std::endl;
    hyuk->Heal(1000.0f);
    hyuk->GenerateQi(100.0f);
    std::cout << "Activating Divine Arts of the Unrecorded..." << std::endl;
    hyuk->ActivateDivineArts();
    printHyukStatus(hyuk.get());
    
    // Test ultimate S skills
    std::cout << "Ultimate S+Direction skills:" << std::endl;
    hyuk->ExecuteDirectionalSkill(Direction::UP);    // Six Bonds United
    hyuk->ExecuteDirectionalSkill(Direction::RIGHT); // Death Moon
    hyuk->ExecuteDirectionalSkill(Direction::LEFT);  // Blue Ocean
    hyuk->ExecuteDirectionalSkill(Direction::DOWN);  // Finger Wind
    
    // Test Master's Vengeance
    std::cout << "\n6. Testing Master's Vengeance..." << std::endl;
    hyuk = std::make_unique<HyukWoonSung>(); // Reset
    std::cout << "Damaging to trigger Master's Vengeance..." << std::endl;
    hyuk->TakeDamage(hyuk->GetMaxHP() * 0.76f);
    hyuk->Update(0.016f);
    std::cout << "Master's Vengeance triggered!" << std::endl;
    printHyukStatus(hyuk.get());
    
    std::cout << "\n=== Hyuk Woon Sung tests completed! ===" << std::endl;
}

void printRouStatus(Rou* rou) {
    std::cout << "\n=== Rou Status ===" << std::endl;
    std::cout << "Current Form: ";
    switch (rou->GetCurrentForm()) {
        case RouEvolutionForm::GOBLIN: std::cout << "Goblin"; break;
        case RouEvolutionForm::HOBGOBLIN: std::cout << "Hobgoblin"; break;
        case RouEvolutionForm::OGRE: std::cout << "Ogre"; break;
        case RouEvolutionForm::APOSTLE_LORD: std::cout << "Apostle Lord"; break;
        case RouEvolutionForm::VAJRAYAKSA: std::cout << "Vajrayaksa"; break;
    }
    std::cout << std::endl;
    std::cout << "Evolution Gauge: " << rou->GetEvolutionGauge() << "%" << std::endl;
    std::cout << "HP: " << rou->GetHPPercent() << "%" << std::endl;
    std::cout << "==================" << std::endl;
}

void testEvolutionSystem() {
    std::cout << "=== Testing Rou Evolution System ===" << std::endl;
    
    auto rou = std::make_unique<Rou>();
    auto vfx = std::make_unique<EvolutionVFX>();
    
    // Initial state
    printRouStatus(rou.get());
    
    // Test gauge building through hits
    std::cout << "\n1. Testing hit-based gauge building..." << std::endl;
    for (int i = 0; i < 17; i++) {
        rou->OnHit(50.0f);
        std::cout << "Hit " << (i + 1) << " - Gauge: " << rou->GetEvolutionGauge() << "%" << std::endl;
    }
    printRouStatus(rou.get());
    
    // Test gauge building through damage
    std::cout << "\n2. Testing damage-based gauge building..." << std::endl;
    rou->OnTakeDamage(100.0f);
    std::cout << "Took 100 damage - Gauge: " << rou->GetEvolutionGauge() << "%" << std::endl;
    printRouStatus(rou.get());
    
    // Test kill bonus
    std::cout << "\n3. Testing kill bonus..." << std::endl;
    rou->OnKill();
    std::cout << "Got a kill - Gauge: " << rou->GetEvolutionGauge() << "%" << std::endl;
    printRouStatus(rou.get());
    
    // Test equipment pickup
    std::cout << "\n4. Testing equipment pickup..." << std::endl;
    rou->OnEquipmentPickup();
    std::cout << "Picked up equipment - Gauge: " << rou->GetEvolutionGauge() << "%" << std::endl;
    printRouStatus(rou.get());
    
    // Force evolution to Vajrayaksa
    std::cout << "\n5. Force evolving to Vajrayaksa..." << std::endl;
    rou->UpdateEvolutionGauge(100.0f - rou->GetEvolutionGauge());
    printRouStatus(rou.get());
    
    // Test all special moves
    std::cout << "\n6. Testing all directional specials..." << std::endl;
    std::vector<Direction> directions = {Direction::UP, Direction::LEFT, Direction::RIGHT};
    for (auto dir : directions) {
        std::cout << "Executing special move: ";
        switch (dir) {
            case Direction::UP: std::cout << "UP"; break;
            case Direction::LEFT: std::cout << "LEFT"; break;
            case Direction::RIGHT: std::cout << "RIGHT"; break;
            default: break;
        }
        std::cout << std::endl;
        rou->ExecuteDirectionalSpecial(dir);
    }
    
    // Test emergency protocol
    std::cout << "\n7. Testing emergency protocol..." << std::endl;
    rou = std::make_unique<Rou>(); // Reset
    std::cout << "Damaging Rou to 30% HP..." << std::endl;
    float maxHP = rou->GetMaxHP();
    rou->OnTakeDamage(maxHP * 0.7f);
    rou->Update(0.016f); // One frame
    std::cout << "Emergency Protocol triggered!" << std::endl;
    printRouStatus(rou.get());
    
    // Test death penalty
    std::cout << "\n8. Testing death penalty..." << std::endl;
    rou->UpdateEvolutionGauge(50.0f);
    std::cout << "Gauge before death: " << rou->GetEvolutionGauge() << "%" << std::endl;
    rou->OnDeath();
    std::cout << "Gauge after death: " << rou->GetEvolutionGauge() << "%" << std::endl;
    
    // Test combo system
    std::cout << "\n9. Testing combo system for each form..." << std::endl;
    std::vector<float> gaugeValues = {0.0f, 25.0f, 50.0f, 75.0f, 100.0f};
    std::vector<std::string> formNames = {"Goblin", "Hobgoblin", "Ogre", "Apostle Lord", "Vajrayaksa"};
    
    for (size_t i = 0; i < gaugeValues.size(); i++) {
        rou = std::make_unique<Rou>();
        rou->UpdateEvolutionGauge(gaugeValues[i]);
        std::cout << formNames[i] << " combo:" << std::endl;
        rou->ExecuteBasicCombo();
    }
    
    // Test visual effects
    std::cout << "\n10. Testing visual effects..." << std::endl;
    vfx->PlayEvolutionEffect(RouEvolutionForm::GOBLIN, RouEvolutionForm::HOBGOBLIN);
    vfx->PlayEmergencyProtocolEffect();
    vfx->PlayGaugeThresholdEffect(25.0f);
    vfx->PlayGaugeThresholdEffect(50.0f);
    vfx->PlayGaugeThresholdEffect(75.0f);
    vfx->PlayGaugeThresholdEffect(100.0f);
    
    // Update VFX for a few frames
    for (int i = 0; i < 60; i++) {
        vfx->Update(0.016f);
    }
    
    std::cout << "\n=== All tests completed! ===" << std::endl;
}

int main() {
    try {
        testEvolutionSystem();
        testHyukStanceSystem();
        
        std::cout << "\nPress Enter to exit..." << std::endl;
        std::cin.get();
        
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cin.get();
        return 1;
    }
}