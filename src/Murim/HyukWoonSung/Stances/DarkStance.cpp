#include "DarkStance.h"
#include <algorithm>
#include <cmath>

namespace ArenaFighter {

DarkStance::DarkStance(HyukWoonSung* owner)
    : m_owner(owner),
      m_isExecutingCombo(false),
      m_currentComboHit(0),
      m_comboTimer(0.0f),
      m_intimidationActive(false),
      m_intimidationDuration(0.0f),
      m_isCharging(false),
      m_chargeTime(0.0f) {
}

DarkStance::~DarkStance() = default;

void DarkStance::Enter() {
    ResetComboState();
    CreateRedEnergyTrail();
    CreateDemonAura();
}

void DarkStance::Exit() {
    m_isExecutingCombo = false;
    m_isCharging = false;
    // Clean up visual effects
}

void DarkStance::Update(float deltaTime) {
    // Update combo state
    if (m_isExecutingCombo) {
        m_comboTimer -= deltaTime;
        if (m_comboTimer <= 0) {
            ResetComboState();
        }
    }
    
    // Update intimidation effect
    if (m_intimidationActive) {
        UpdateIntimidation(deltaTime);
    }
    
    // Update charging state
    if (m_isCharging) {
        m_chargeTime += deltaTime;
    }
    
    // Update active meteors
    UpdateMeteors(deltaTime);
}

void DarkStance::ExecuteBasicCombo() {
    // Heavenly Demon Divine Palm - 4-hit combo
    m_isExecutingCombo = true;
    m_currentComboHit = 0;
    m_comboTimer = 2.5f;
    
    // Palm combo with escalating effects
    float damages[4] = {30.0f, 40.0f, 50.0f, 70.0f};
    
    for (int i = 0; i < 4; i++) {
        float damage = damages[i];
        damage = ApplyCombatModifiers(damage);
        
        switch (i) {
            case 0:
                // Palm strike with red shockwave
                CreatePalmShockwave();
                break;
            case 1:
                // Double palm with explosion
                CreateDarkExplosion();
                break;
            case 2:
                // Spinning back palm
                CreateScreamingFaceTrail();
                break;
            case 3:
                // Two-handed palm with demon face
                CreateDemonSoulEffect();
                // Launch enemy
                break;
        }
        
        m_currentComboHit = i + 1;
    }
}

void DarkStance::ExecuteChargedAttack() {
    // Red Soul Charge
    m_isCharging = true;
    m_chargeTime = 0.0f;
    
    // Charge dark energy (1.5 seconds)
    CreateDemonAura();
    
    // Release phase
    CreateDarkExplosion();
    
    float damage = 240.0f;
    damage = ApplyCombatModifiers(damage);
    
    m_isCharging = false;
}

void DarkStance::ExecuteDashCombo() {
    // Reign of Heavenly Demon Step - 2-hit dash
    for (int i = 0; i < 2; i++) {
        float damage = 60.0f + (i * 20.0f);
        damage = ApplyCombatModifiers(damage);
        
        CreateScreamingFaceTrail();
    }
}

void DarkStance::ExecuteAerialAttack() {
    // Heavenly Demon's Spiral
    float damage = 95.0f;
    damage = ApplyCombatModifiers(damage);
    
    CreateDemonSoulEffect();
}

// S+Direction Skills
void DarkStance::ExecuteHeavenlyDemonPower() {
    if (m_owner->GetQiEnergy() >= 30.0f) {
        m_owner->ConsumeQi(30.0f);
        
        float damage = 200.0f;
        damage = ApplyCombatModifiers(damage);
        
        CreateDemonSoulEffect();
    }
}

void DarkStance::ExecuteBlackNightFourthMoon() {
    // 2nd Bond technique
    if (m_owner->GetQiEnergy() >= 18.0f) {
        m_owner->ConsumeQi(18.0f);
        
        float damage = 110.0f;
        damage = ApplyCombatModifiers(damage);
        
        // Black moon effect
        CreateDarkExplosion();
    }
}

void DarkStance::ExecuteMindSplitDoubleWill() {
    if (m_owner->GetQiEnergy() >= 24.0f) {
        m_owner->ConsumeQi(24.0f);
        
        float damage = 150.0f;
        damage = ApplyCombatModifiers(damage);
        
        // Double strike effect
        CreateScreamingFaceTrail();
    }
}

// Gear Skills
void DarkStance::ExecuteDemonDestroysWorld() {
    // AS - Weapon skill
    if (m_owner->GetQiEnergy() >= 40.0f) {
        m_owner->ConsumeQi(40.0f);
        
        float damage = 260.0f;
        damage = ApplyCombatModifiers(damage);
        
        CreateDemonSoulEffect();
    }
}

void DarkStance::ExecuteIntimidationDress() {
    // AD - Helmet skill (fear effect)
    if (m_owner->GetQiEnergy() >= 60.0f) {
        m_owner->ConsumeQi(60.0f);
        
        m_intimidationActive = true;
        m_intimidationDuration = 8.0f;
        
        CreateDemonAura();
    }
}

void DarkStance::ExecuteDarkFlowerFlame() {
    // ASD - Armor skill
    if (m_owner->GetQiEnergy() >= 55.0f) {
        m_owner->ConsumeQi(55.0f);
        
        float damage = 320.0f;
        damage = ApplyCombatModifiers(damage);
        
        // Dark flower effect
    }
}

void DarkStance::ExecuteBigMeteor() {
    // SD - THE BIG METEOR
    if (m_owner->GetQiEnergy() >= 45.0f) {
        m_owner->ConsumeQi(45.0f);
        
        // Create sky portal
        CreateMeteorPortal();
        
        // Spawn the big meteor
        MeteorInstance meteor;
        meteor.x = 0.0f;
        meteor.y = 20.0f; // High in the sky
        meteor.z = 0.0f;
        meteor.vx = 0.0f;
        meteor.vy = -5.0f; // Falling speed
        meteor.vz = 0.0f;
        meteor.lifetime = BigMeteorConfig::FALL_DURATION;
        meteor.isTracking = true;
        meteor.target = nullptr; // Will track nearest enemy
        
        m_activeMeteors.push_back(meteor);
        
        // 40% max HP damage on impact
        float damage = m_owner->GetMaxHP() * 0.4f;
    }
}

// Helper functions
void DarkStance::ResetComboState() {
    m_isExecutingCombo = false;
    m_currentComboHit = 0;
    m_comboTimer = 0.0f;
}

void DarkStance::UpdateIntimidation(float deltaTime) {
    m_intimidationDuration -= deltaTime;
    if (m_intimidationDuration <= 0) {
        m_intimidationActive = false;
        m_fearTargets.clear();
    }
}

void DarkStance::UpdateMeteors(float deltaTime) {
    for (auto it = m_activeMeteors.begin(); it != m_activeMeteors.end();) {
        it->lifetime -= deltaTime;
        
        // Update position
        it->x += it->vx * deltaTime;
        it->y += it->vy * deltaTime;
        it->z += it->vz * deltaTime;
        
        // Check for ground impact
        if (it->y <= 0.0f || it->lifetime <= 0) {
            // Create impact explosion
            CreateDarkExplosion();
            
            // Apply damage
            float damage = m_owner->GetMaxHP() * 0.4f;
            
            it = m_activeMeteors.erase(it);
        } else {
            ++it;
        }
    }
}

void DarkStance::CheckComboExtensions() {
    // Check for combo extensions and cancels
}

float DarkStance::CalculateBaseDamage(int hitNumber) const {
    // Divine Palm damage progression
    static const float palmDamages[4] = {30.0f, 40.0f, 50.0f, 70.0f};
    if (hitNumber >= 0 && hitNumber < 4) {
        return palmDamages[hitNumber];
    }
    return 30.0f;
}

float DarkStance::ApplyCombatModifiers(float baseDamage) const {
    float damage = baseDamage;
    
    // Apply dark stance damage modifier
    damage *= DarkCombatData::DAMAGE_MODIFIER;
    
    // Apply intimidation bonus
    if (m_intimidationActive) {
        damage *= 1.3f;
    }
    
    return damage;
}

// Visual effect implementations
void DarkStance::CreateDemonSoulEffect() {
    // Create dark demon soul particles
}

void DarkStance::CreateRedEnergyTrail() {
    // Create red energy trail with screaming faces
}

void DarkStance::CreateDarkExplosion() {
    // Create dark explosion with demon energy
}

void DarkStance::CreateDemonAura() {
    // Create intimidating demon aura
}

void DarkStance::CreatePalmShockwave() {
    // Create shockwave from palm strike
}

void DarkStance::CreateMeteorPortal() {
    // Create portal in sky for meteor
}

void DarkStance::CreateScreamingFaceTrail() {
    // Create trail of screaming demon faces
}

// Get property functions
DarkStance::DarkVisualConfig DarkStance::GetVisualConfig() const {
    return DarkVisualConfig{};
}

DarkStance::DarkFrameData DarkStance::GetFrameData() const {
    return DarkFrameData{};
}

DarkStance::DarkCombatData DarkStance::GetCombatData() const {
    return DarkCombatData{};
}

DarkStance::BigMeteorConfig DarkStance::GetMeteorConfig() const {
    return BigMeteorConfig{};
}

} // namespace ArenaFighter