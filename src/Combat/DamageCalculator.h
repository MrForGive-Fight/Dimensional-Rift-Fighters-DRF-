#pragma once

#include "CombatEnums.h"

namespace ArenaFighter {

// Forward declarations
enum class ElementType;
enum class CharacterState;

/**
 * @brief Handles all damage calculations following LSFDC formula
 * 
 * Implements the core damage formula:
 * damage = baseDamage * (100.0f / (100.0f + defense)) * elementMultiplier * pow(0.9f, comboCount)
 */
class DamageCalculator {
public:
    struct DamageParams {
        float baseDamage;
        float attackerPower;
        float defenderDefense;
        DamageType damageType;
        AttackType attackType;
        int comboCount;
        bool isCounter;
        bool isCritical;
        ElementType attackerElement;
        ElementType defenderElement;
        CharacterState defenderState;
        float damageReduction = 0.0f;
    };

    DamageCalculator();
    ~DamageCalculator();

    bool Initialize();
    
    // Core damage calculation
    float CalculateDamage(const DamageParams& params);
    
    // Component calculations
    float CalculateBaseDamage(float baseDamage, float powerModifier);
    float CalculateDefenseReduction(float damage, float defense);
    float GetElementMultiplier(ElementType attacker, ElementType defender);
    float GetComboScaling(int hitCount);
    float GetStateModifier(CharacterState state);
    float GetCounterBonus();
    float GetCriticalMultiplier();
    
    // Hitstun and knockback calculations
    int CalculateHitstun(float finalDamage, bool isCounter);
    float CalculateKnockback(float finalDamage, float targetWeight);
    int CalculateBlockstun(AttackType attackType);
    
    // LSFDC constants
    static constexpr float COMBO_SCALING_FACTOR = 0.9f;
    static constexpr float COUNTER_MULTIPLIER = 1.5f;
    static constexpr float CRITICAL_MULTIPLIER = 2.0f;
    static constexpr float MIN_DAMAGE = 1.0f;
    static constexpr int BASE_HITSTUN = 12;
    static constexpr int MIN_HITSTUN = 10;
    static constexpr int MAX_HITSTUN = 60;
    static constexpr float HITSTUN_DECAY = 0.95f;

private:
    struct ElementMatchup {
        ElementType attacker;
        ElementType defender;
        float multiplier;
    };
    
    // Element effectiveness table
    void InitializeElementTable();
    float LookupElementMultiplier(ElementType attacker, ElementType defender) const;
    
    class DamageCalculatorImpl;
    std::unique_ptr<DamageCalculatorImpl> m_impl;
};

} // namespace ArenaFighter