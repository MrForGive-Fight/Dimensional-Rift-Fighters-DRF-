#include "LightStance.h"
#include <algorithm>

namespace ArenaFighter {

LightStance::LightStance(HyukWoonSung* owner)
    : m_owner(owner),
      m_isExecutingCombo(false),
      m_currentComboHit(0),
      m_comboTimer(0.0f),
      m_spearAuraActive(false),
      m_spearAuraDuration(0.0f),
      m_isCharging(false),
      m_chargeTime(0.0f) {
}

LightStance::~LightStance() = default;

void LightStance::Enter() {
    ResetComboState();
    CreateBlueEnergyTrail();
    CreateDivineAura();
}

void LightStance::Exit() {
    m_isExecutingCombo = false;
    m_isCharging = false;
    // Clean up visual effects
}

void LightStance::Update(float deltaTime) {
    // Update combo state
    if (m_isExecutingCombo) {
        m_comboTimer -= deltaTime;
        if (m_comboTimer <= 0) {
            ResetComboState();
        }
    }
    
    // Update spear aura buff
    if (m_spearAuraActive) {
        UpdateSpearAura(deltaTime);
    }
    
    // Update charging state
    if (m_isCharging) {
        m_chargeTime += deltaTime;
    }
}

void LightStance::ExecuteBasicCombo() {
    // Seven Stars of the Spear - 7 rapid attacks
    m_isExecutingCombo = true;
    m_currentComboHit = 0;
    m_comboTimer = 2.0f; // Combo window
    
    for (int i = 0; i < 7; i++) {
        float damage = CalculateBaseDamage(i);
        damage = ApplyCombatModifiers(damage);
        
        if (i < 3) {
            // Lightning-fast thrusts
            CreateStarEffect(1);
        } else if (i < 6) {
            // Circular spear spins
            CreateStarEffect(3);
        } else {
            // Final thrust with 7 blue stars
            CreateStarEffect(7);
        }
        
        m_currentComboHit = i + 1;
    }
}

void LightStance::ExecuteChargedAttack() {
    // Divine Spear of Ending Night
    m_isCharging = true;
    m_chargeTime = 0.0f;
    
    // Charge phase (1 second)
    // Visual: Spear overhead, blue energy condenses
    
    // Release phase
    CreateCrescentWave();
    
    float damage = 200.0f;
    damage = ApplyCombatModifiers(damage);
    
    m_isCharging = false;
}

void LightStance::ExecuteDashCombo() {
    // Travel of the Gale - 3-hit dash combo
    for (int i = 0; i < 3; i++) {
        float damage = 50.0f + (i * 10.0f);
        damage = ApplyCombatModifiers(damage);
        
        CreateBlueEnergyTrail();
    }
}

void LightStance::ExecuteAerialAttack() {
    // Aerial Spear Dance
    float damage = 85.0f;
    damage = ApplyCombatModifiers(damage);
    
    CreateStarEffect(5);
}

// S+Direction Skills
void LightStance::ExecuteSpearSeaImpact() {
    // 5th Bond technique - massive spear projection
    if (m_owner->GetQiEnergy() >= 25.0f) {
        m_owner->ConsumeQi(25.0f);
        
        float damage = 180.0f;
        damage = ApplyCombatModifiers(damage);
        
        CreateStarEffect(15);
        // Spear sea visual effect
    }
}

void LightStance::ExecuteDivineWindOfThePast() {
    // 3rd Bond technique - wind-based attack
    if (m_owner->GetQiEnergy() >= 20.0f) {
        m_owner->ConsumeQi(20.0f);
        
        float damage = 120.0f;
        damage = ApplyCombatModifiers(damage);
        
        // Wind spiral effect
    }
}

void LightStance::ExecuteLightningStitching() {
    // Lightning-based precise attack
    if (m_owner->GetQiEnergy() >= 22.0f) {
        m_owner->ConsumeQi(22.0f);
        
        float damage = 140.0f;
        damage = ApplyCombatModifiers(damage);
        
        CreateLightningEffect();
    }
}

// Gear Skills
void LightStance::ExecuteGlassyDeathRain() {
    // AS - Weapon skill
    if (m_owner->GetQiEnergy() >= 35.0f) {
        m_owner->ConsumeQi(35.0f);
        
        float damage = 220.0f;
        damage = ApplyCombatModifiers(damage);
        
        // Rain of glass projectiles
        CreateStarEffect(30);
    }
}

void LightStance::ExecuteSpearAura() {
    // AD - Helmet skill (buff)
    if (m_owner->GetQiEnergy() >= 40.0f) {
        m_owner->ConsumeQi(40.0f);
        
        m_spearAuraActive = true;
        m_spearAuraDuration = 10.0f;
        
        CreateDivineAura();
    }
}

void LightStance::ExecuteDivineDragonFlow() {
    // ASD - Armor skill
    if (m_owner->GetQiEnergy() >= 50.0f) {
        m_owner->ConsumeQi(50.0f);
        
        float damage = 300.0f;
        damage = ApplyCombatModifiers(damage);
        
        // Dragon flow effect
    }
}

void LightStance::ExecuteThunderousSpear() {
    // SD - Trinket skill
    if (m_owner->GetQiEnergy() >= 30.0f) {
        m_owner->ConsumeQi(30.0f);
        
        float damage = 160.0f;
        damage = ApplyCombatModifiers(damage);
        
        CreateLightningEffect();
    }
}

// Helper functions
void LightStance::ResetComboState() {
    m_isExecutingCombo = false;
    m_currentComboHit = 0;
    m_comboTimer = 0.0f;
}

void LightStance::UpdateSpearAura(float deltaTime) {
    m_spearAuraDuration -= deltaTime;
    if (m_spearAuraDuration <= 0) {
        m_spearAuraActive = false;
    }
}

void LightStance::CheckComboExtensions() {
    // Check for combo extensions and cancels
}

float LightStance::CalculateBaseDamage(int hitNumber) const {
    // Seven Stars damage progression
    static const float starDamages[7] = {15.0f, 18.0f, 21.0f, 25.0f, 28.0f, 32.0f, 40.0f};
    if (hitNumber >= 0 && hitNumber < 7) {
        return starDamages[hitNumber];
    }
    return 15.0f;
}

float LightStance::ApplyCombatModifiers(float baseDamage) const {
    float damage = baseDamage;
    
    // Apply light stance damage modifier
    damage *= LightCombatData::DAMAGE_MODIFIER;
    
    // Apply spear aura bonus
    if (m_spearAuraActive) {
        damage *= 1.2f;
    }
    
    return damage;
}

// Visual effect implementations
void LightStance::CreateStarEffect(int count) {
    // Create blue star particles
}

void LightStance::CreateBlueEnergyTrail() {
    // Create blue energy trail behind spear
}

void LightStance::CreateLightningEffect() {
    // Create lightning bolt effect
}

void LightStance::CreateDivineAura() {
    // Create divine blue aura around character
}

void LightStance::CreateCrescentWave() {
    // Create crescent-shaped energy wave
}

// Get property functions
LightStance::LightVisualConfig LightStance::GetVisualConfig() const {
    return LightVisualConfig{};
}

LightStance::LightFrameData LightStance::GetFrameData() const {
    return LightFrameData{};
}

LightStance::LightCombatData LightStance::GetCombatData() const {
    return LightCombatData{};
}

} // namespace ArenaFighter