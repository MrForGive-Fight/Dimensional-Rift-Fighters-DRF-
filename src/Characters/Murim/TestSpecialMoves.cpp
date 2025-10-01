#include "HyukWoonSung.h"
#include <iostream>
#include <iomanip>

using namespace ArenaFighter;

void PrintSpecialMove(const SpecialMove* move) {
    if (!move) {
        std::cout << "  No special move registered\n";
        return;
    }
    
    std::cout << "  Name: " << move->name << "\n";
    std::cout << "  Mana Cost: " << move->manaCost << " (NO COOLDOWN)\n";
    std::cout << "  Damage: " << move->baseDamage << "\n";
    std::cout << "  Frames: " << move->startupFrames << "/" 
              << move->activeFrames << "/" << move->recoveryFrames << "\n";
    std::cout << "  Properties: ";
    if (move->isProjectile) std::cout << "Projectile ";
    if (!move->canCombo) std::cout << "NoCombo ";
    if (!move->blockable) std::cout << "Unblockable ";
    std::cout << "\n";
}

void TestHyukWoonSungSpecialMoves() {
    std::cout << "=== Testing HyukWoonSung Special Move System ===\n\n";
    
    HyukWoonSung character;
    character.Initialize();
    
    // Test Light Stance moves
    std::cout << "LIGHT STANCE Special Moves (S+Direction):\n";
    std::cout << "---------------------------------------\n";
    
    std::cout << "S+UP:\n";
    PrintSpecialMove(character.GetSpecialMove(InputDirection::Up));
    
    std::cout << "\nS+RIGHT:\n";
    PrintSpecialMove(character.GetSpecialMove(InputDirection::Right));
    
    std::cout << "\nS+LEFT:\n";
    PrintSpecialMove(character.GetSpecialMove(InputDirection::Left));
    
    std::cout << "\nS+DOWN:\n";
    PrintSpecialMove(character.GetSpecialMove(InputDirection::Down));
    
    // Switch to Dark Stance
    std::cout << "\n\nSwitching to Dark Stance...\n";
    character.SwitchStance(1);
    
    std::cout << "\nDARK STANCE Special Moves (S+Direction):\n";
    std::cout << "---------------------------------------\n";
    
    std::cout << "S+UP:\n";
    PrintSpecialMove(character.GetSpecialMove(InputDirection::Up));
    
    std::cout << "\nS+RIGHT:\n";
    PrintSpecialMove(character.GetSpecialMove(InputDirection::Right));
    
    std::cout << "\nS+LEFT:\n";
    PrintSpecialMove(character.GetSpecialMove(InputDirection::Left));
    
    std::cout << "\nS+DOWN:\n";
    PrintSpecialMove(character.GetSpecialMove(InputDirection::Down));
    
    // Test mana consumption
    std::cout << "\n\nTesting Mana Consumption:\n";
    std::cout << "------------------------\n";
    std::cout << "Current Mana: " << character.GetCurrentMana() << "/" 
              << character.GetMaxMana() << "\n";
    
    if (character.CanExecuteSpecialMove(InputDirection::Up)) {
        std::cout << "Executing Heavenly Demon Divine Power (S+UP)...\n";
        character.ExecuteSpecialMove(InputDirection::Up);
        std::cout << "Mana after execution: " << character.GetCurrentMana() << "/" 
                  << character.GetMaxMana() << "\n";
    }
    
    // Test stance requirements
    std::cout << "\n\nTesting Stance Requirements:\n";
    std::cout << "---------------------------\n";
    std::cout << "Current Stance: Dark (" << character.GetCurrentStance() << ")\n";
    std::cout << "Can execute Dark stance moves: " 
              << (character.CanExecuteSpecialMove(InputDirection::Up) ? "YES" : "NO") << "\n";
    
    // Switch back to Light
    character.SwitchStance(0);
    std::cout << "\nSwitched to Light Stance (" << character.GetCurrentStance() << ")\n";
    std::cout << "Can execute previous Dark move: " 
              << (character.CanExecuteSpecialMove(InputDirection::Up) ? "YES" : "NO") << "\n";
    std::cout << "Move is now: " << character.GetSpecialMove(InputDirection::Up)->name << "\n";
    
    std::cout << "\n=== Test Complete ===\n";
}

int main() {
    TestHyukWoonSungSpecialMoves();
    return 0;
}