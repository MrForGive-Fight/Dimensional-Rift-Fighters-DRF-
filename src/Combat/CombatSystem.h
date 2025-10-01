#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include "CombatEnums.h"
#include "SpecialMoveSystem.h"

namespace ArenaFighter {

// Forward declarations
class Character;
class DamageCalculator;
class HitDetection;
class ComboSystem;
class SpecialMoveSystem;
struct FrameData;

// Combat state for tracking character states
struct CombatState {
    int hitstunFrames = 0;
    int blockstunFrames = 0;
    bool isBlocking = false;
    float blockDamageReduction = 0.0f;
};

/**
 * @brief Core combat system managing all combat-related calculations and mechanics
 * 
 * Implements LSFDC combat patterns including:
 * - Frame-perfect hit detection
 * - Damage calculation with proper scaling
 * - Combo system with limits and scaling
 * - Dual skill system:
 *   - Special Moves (S+Direction): MANA ONLY, NO COOLDOWNS
 *   - Gear Skills (AS, AD, ASD, SD): BOTH Mana AND Cooldowns
 */
class CombatSystem {
public:
    CombatSystem();
    ~CombatSystem();

    // System lifecycle
    bool Initialize();
    void Shutdown();
    void Update(float deltaTime);

    // Combat calculations
    float ProcessDamage(Character* attacker, Character* defender, 
                       float baseDamage, DamageType damageType, 
                       AttackType attackType, int comboCount = 0);

    // Hit detection
    bool CheckHit(const class HitBox& attackBox, const class HurtBox& defenseBox,
                  float activeFrames, float currentFrame);

    // Combo management
    void RegisterHit(int attackerId, int defenderId, AttackType type, float damage);
    void ResetCombo(int attackerId);
    int GetComboCount(int attackerId) const;
    float GetComboScaling(int attackerId) const;
    bool IsValidCombo(int attackerId, float timeSinceLastHit) const;

    // Frame data management
    void RegisterFrameData(const std::string& characterName, 
                          const std::string& skillName, 
                          const FrameData& frameData);
    const FrameData* GetFrameData(const std::string& characterName,
                                 const std::string& skillName) const;

    // Mana management
    bool CanAffordSkill(Character* character, float manaCost) const;
    void ConsumeMana(Character* character, float manaCost);

    // State queries
    bool IsInHitstun(Character* character) const;
    bool IsInBlockstun(Character* character) const;
    bool CanAct(Character* character) const;
    int GetRemainingHitstun(Character* character) const;

    // Input handling for special moves
    void HandleSpecialInput(int playerId, InputDirection direction, bool sPressed);
    void ProcessBlockingState(int playerId, float deltaTime);
    
    // Blocking queries
    bool IsBlocking(int playerId) const;
    float GetBlockDamageReduction(int playerId) const;
    
    // Special move management
    void RegisterSpecialMoveSystem(int playerId, SpecialMoveSystem* system);
    SpecialMoveSystem* GetSpecialMoveSystem(int playerId) const;
    
    // Constants from CLAUDE.md
    static constexpr float BASE_HEALTH = 1000.0f;
    static constexpr float BASE_MANA = 100.0f;
    static constexpr float COMBO_SCALING = 0.9f;
    static constexpr int MAX_COMBO_HITS = 15;
    static constexpr float MAX_COMBO_DAMAGE_PERCENT = 0.6f;
    
    // Blocking constants
    static constexpr float BLOCK_DAMAGE_REDUCTION = 0.5f; // 50% damage reduction when blocking
    static constexpr float CHIP_DAMAGE_MULTIPLIER = 0.25f; // 25% chip damage through block

private:
    struct CombatSystemImpl;
    std::unique_ptr<CombatSystemImpl> m_impl;

    // Internal update methods
    void UpdateCombatStates(float deltaTime);
    void UpdateManaRegeneration(float deltaTime);
    void ProcessActiveHitboxes(float deltaTime);
    void CleanExpiredCombos(float deltaTime);
};

} // namespace ArenaFighter