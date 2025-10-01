#include "DamageCalculator.h"
#include <cmath>
#include <algorithm>
#include <vector>

namespace ArenaFighter {

// Element types following LSFDC pattern
enum class ElementType {
    Neutral,
    Fire,
    Ice,
    Lightning,
    Earth,
    Wind,
    Light,
    Dark,
    Void
};

// Character states for damage modifiers
enum class CharacterState {
    Normal,
    Defending,
    HitStun,
    KnockedDown,
    GettingUp,
    Airborne,
    Crouching
};

class DamageCalculator::DamageCalculatorImpl {
public:
    std::vector<ElementMatchup> elementTable;
};

DamageCalculator::DamageCalculator() : m_impl(std::make_unique<DamageCalculatorImpl>()) {
}

DamageCalculator::~DamageCalculator() = default;

bool DamageCalculator::Initialize() {
    InitializeElementTable();
    return true;
}

float DamageCalculator::CalculateDamage(const DamageParams& params) {
    // Step 1: Base damage with power modifier
    float damage = CalculateBaseDamage(params.baseDamage, params.attackerPower);
    
    // Step 2: Apply defense reduction (LSFDC formula)
    damage = CalculateDefenseReduction(damage, params.defenderDefense);
    
    // Step 3: Element multiplier
    float elementMult = GetElementMultiplier(params.attackerElement, params.defenderElement);
    damage *= elementMult;
    
    // Step 4: Combo scaling
    damage *= GetComboScaling(params.comboCount);
    
    // Step 5: Counter hit bonus
    if (params.isCounter) {
        damage *= GetCounterBonus();
    }
    
    // Step 6: Critical hit
    if (params.isCritical) {
        damage *= GetCriticalMultiplier();
    }
    
    // Step 7: State-based modifiers
    damage *= GetStateModifier(params.defenderState);
    
    // Step 8: Damage type modifiers
    switch (params.damageType) {
        case DamageType::True:
            // True damage ignores additional reductions
            break;
        case DamageType::Physical:
            // Physical damage can be further reduced by armor
            damage *= (1.0f - params.damageReduction * 0.5f);
            break;
        case DamageType::Magical:
            // Magical damage affected less by armor
            damage *= (1.0f - params.damageReduction * 0.3f);
            break;
    }
    
    // Step 9: Apply general damage reduction
    damage *= (1.0f - params.damageReduction);
    
    // Step 10: Minimum damage guarantee
    damage = std::max(damage, MIN_DAMAGE);
    
    return damage;
}

float DamageCalculator::CalculateBaseDamage(float baseDamage, float powerModifier) {
    return baseDamage * powerModifier;
}

float DamageCalculator::CalculateDefenseReduction(float damage, float defense) {
    // LSFDC defense formula
    return damage * (100.0f / (100.0f + defense));
}

float DamageCalculator::GetElementMultiplier(ElementType attacker, ElementType defender) {
    return LookupElementMultiplier(attacker, defender);
}

float DamageCalculator::GetComboScaling(int hitCount) {
    // LSFDC combo scaling: 0.9^n
    return std::pow(COMBO_SCALING_FACTOR, static_cast<float>(hitCount));
}

float DamageCalculator::GetStateModifier(CharacterState state) {
    switch (state) {
        case CharacterState::Defending:
            return 0.3f;  // 70% damage reduction when blocking
        case CharacterState::HitStun:
            return 1.1f;  // 10% more damage during hitstun
        case CharacterState::KnockedDown:
            return 0.8f;  // 20% less damage when knocked down
        case CharacterState::GettingUp:
            return 0.5f;  // 50% less damage during wakeup
        case CharacterState::Airborne:
            return 1.2f;  // 20% more damage when airborne
        case CharacterState::Crouching:
            return 0.9f;  // 10% less damage when crouching
        default:
            return 1.0f;
    }
}

float DamageCalculator::GetCounterBonus() {
    return COUNTER_MULTIPLIER;
}

float DamageCalculator::GetCriticalMultiplier() {
    return CRITICAL_MULTIPLIER;
}

int DamageCalculator::CalculateHitstun(float finalDamage, bool isCounter) {
    // LSFDC hitstun formula
    int hitstun = static_cast<int>(finalDamage / 10.0f) + BASE_HITSTUN;
    
    // Counter hit adds 50% more hitstun
    if (isCounter) {
        hitstun = static_cast<int>(hitstun * 1.5f);
    }
    
    // Clamp between minimum and maximum values
    return std::clamp(hitstun, MIN_HITSTUN, MAX_HITSTUN);
}

float DamageCalculator::CalculateKnockback(float finalDamage, float targetWeight) {
    // LSFDC knockback formula
    const float BASE_KNOCKBACK = 5.0f;
    const float WEIGHT_FACTOR = 100.0f / targetWeight;
    
    float knockback = BASE_KNOCKBACK + (finalDamage * 0.1f) * WEIGHT_FACTOR;
    
    // Maximum knockback limit
    const float MAX_KNOCKBACK = 30.0f;
    return std::min(knockback, MAX_KNOCKBACK);
}

int DamageCalculator::CalculateBlockstun(AttackType attackType) {
    // Blockstun based on attack type
    switch (attackType) {
        case AttackType::Light:
            return 11;  // Light attacks: less blockstun
        case AttackType::Medium:
            return 16;  // Medium attacks: moderate blockstun
        case AttackType::Heavy:
            return 22;  // Heavy attacks: high blockstun
        case AttackType::Special:
            return 18;  // Special moves: variable blockstun
        case AttackType::Ultimate:
            return 25;  // Ultimate skills: highest blockstun
        default:
            return 11;
    }
}

void DamageCalculator::InitializeElementTable() {
    // LSFDC element effectiveness chart
    // Fire > Ice > Lightning > Earth > Wind > Fire (cycle)
    // Light <-> Dark (mutual effectiveness)
    // Void: neutral to all
    
    m_impl->elementTable = {
        // Strong against (1.5x damage)
        {ElementType::Fire, ElementType::Ice, 1.5f},
        {ElementType::Ice, ElementType::Lightning, 1.5f},
        {ElementType::Lightning, ElementType::Earth, 1.5f},
        {ElementType::Earth, ElementType::Wind, 1.5f},
        {ElementType::Wind, ElementType::Fire, 1.5f},
        {ElementType::Light, ElementType::Dark, 1.5f},
        {ElementType::Dark, ElementType::Light, 1.5f},
        
        // Weak against (0.5x damage)
        {ElementType::Fire, ElementType::Wind, 0.5f},
        {ElementType::Ice, ElementType::Fire, 0.5f},
        {ElementType::Lightning, ElementType::Ice, 0.5f},
        {ElementType::Earth, ElementType::Lightning, 0.5f},
        {ElementType::Wind, ElementType::Earth, 0.5f}
    };
}

float DamageCalculator::LookupElementMultiplier(ElementType attacker, ElementType defender) const {
    // Check for special matchups
    for (const auto& matchup : m_impl->elementTable) {
        if (matchup.attacker == attacker && matchup.defender == defender) {
            return matchup.multiplier;
        }
    }
    
    // Default neutral damage
    return 1.0f;
}

} // namespace ArenaFighter