// LSFDC Combat Damage Calculation Formula
// Extracted from Lost Saga reference implementation

#include <cmath>
#include <algorithm>

namespace LSFDC {
namespace Combat {

struct AttackData {
    float baseDamage;
    float powerModifier;
    int comboCount;
    ElementType element;
    AttackProperty properties;
    bool isCounter;
    bool isCritical;
};

struct DefenseData {
    float defensePower;
    float defenseModifier;
    ElementType element;
    CharacterState state;
    float damageReduction;
};

// Core damage calculation following LSFDC standard
float CalculateDamage(const AttackData& attack, const DefenseData& defense) {
    // Step 1: Base damage calculation
    float damage = attack.baseDamage * attack.powerModifier;
    
    // Step 2: Defense calculation (LSFDC formula)
    float defenseValue = defense.defensePower * defense.defenseModifier;
    damage = damage * (100.0f / (100.0f + defenseValue));
    
    // Step 3: Element multiplier
    float elementMultiplier = GetElementMultiplier(attack.element, defense.element);
    damage *= elementMultiplier;
    
    // Step 4: Combo scaling (LSFDC standard: 0.9 per hit)
    const float COMBO_SCALING = 0.9f;
    damage *= std::pow(COMBO_SCALING, attack.comboCount);
    
    // Step 5: Counter hit bonus
    if (attack.isCounter) {
        damage *= 1.5f; // LSFDC counter multiplier
    }
    
    // Step 6: Critical hit
    if (attack.isCritical) {
        damage *= 2.0f; // LSFDC critical multiplier
    }
    
    // Step 7: State-based modifiers
    damage *= GetStateModifier(defense.state);
    
    // Step 8: Property modifiers
    if (attack.properties & AttackProperty::Piercing) {
        damage *= 1.25f; // Ignores 25% of defense
    }
    
    // Step 9: Damage reduction
    damage *= (1.0f - defense.damageReduction);
    
    // Step 10: Minimum damage guarantee
    const float MIN_DAMAGE = 1.0f;
    damage = std::max(damage, MIN_DAMAGE);
    
    return damage;
}

// Element effectiveness chart (LSFDC standard)
float GetElementMultiplier(ElementType attack, ElementType defense) {
    // Fire > Ice > Lightning > Earth > Wind > Fire (cycle)
    // Light <-> Dark (mutual effectiveness)
    // Void: neutral to all
    
    struct ElementMatch {
        ElementType attacker;
        ElementType defender;
        float multiplier;
    };
    
    static const ElementMatch effectiveness[] = {
        // Strong against
        {ElementType::Fire, ElementType::Ice, 1.5f},
        {ElementType::Ice, ElementType::Lightning, 1.5f},
        {ElementType::Lightning, ElementType::Earth, 1.5f},
        {ElementType::Earth, ElementType::Wind, 1.5f},
        {ElementType::Wind, ElementType::Fire, 1.5f},
        {ElementType::Light, ElementType::Dark, 1.5f},
        {ElementType::Dark, ElementType::Light, 1.5f},
        
        // Weak against
        {ElementType::Fire, ElementType::Wind, 0.5f},
        {ElementType::Ice, ElementType::Fire, 0.5f},
        {ElementType::Lightning, ElementType::Ice, 0.5f},
        {ElementType::Earth, ElementType::Lightning, 0.5f},
        {ElementType::Wind, ElementType::Earth, 0.5f},
    };
    
    for (const auto& match : effectiveness) {
        if (match.attacker == attack && match.defender == defense) {
            return match.multiplier;
        }
    }
    
    return 1.0f; // Neutral damage
}

// State-based damage modifiers
float GetStateModifier(CharacterState state) {
    switch (state) {
        case CharacterState::Defending:
            return 0.3f;  // 70% damage reduction when blocking
        case CharacterState::Hit_Stun:
            return 1.1f;  // 10% more damage during hitstun
        case CharacterState::Knocked_Down:
            return 0.8f;  // 20% less damage when knocked down
        case CharacterState::Getting_Up:
            return 0.5f;  // 50% less damage during wakeup
        default:
            return 1.0f;
    }
}

// Hitstun calculation (LSFDC formula)
int CalculateHitstun(float damage, bool isCounter) {
    // Base hitstun formula: damage / 10 + 12 frames
    int hitstun = static_cast<int>(damage / 10.0f) + 12;
    
    // Counter hit adds 50% more hitstun
    if (isCounter) {
        hitstun = static_cast<int>(hitstun * 1.5f);
    }
    
    // Clamp between minimum and maximum values
    const int MIN_HITSTUN = 10;
    const int MAX_HITSTUN = 60;
    hitstun = std::clamp(hitstun, MIN_HITSTUN, MAX_HITSTUN);
    
    return hitstun;
}

// Knockback calculation
float CalculateKnockback(float damage, float weight) {
    // LSFDC knockback formula
    const float BASE_KNOCKBACK = 5.0f;
    const float WEIGHT_FACTOR = 100.0f / weight;
    
    float knockback = BASE_KNOCKBACK + (damage * 0.1f) * WEIGHT_FACTOR;
    
    // Maximum knockback limit
    const float MAX_KNOCKBACK = 30.0f;
    return std::min(knockback, MAX_KNOCKBACK);
}

} // namespace Combat
} // namespace LSFDC