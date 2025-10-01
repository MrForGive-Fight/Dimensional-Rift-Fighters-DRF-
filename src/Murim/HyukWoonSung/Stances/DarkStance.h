#pragma once

#include "../HyukWoonSung.h"

namespace ArenaFighter {

// Forward declaration
class HyukWoonSung;

class DarkStance {
public:
    DarkStance(HyukWoonSung* owner);
    ~DarkStance();
    
    // Core stance operations
    void Enter();
    void Exit();
    void Update(float deltaTime);
    
    // Combat moves
    void ExecuteBasicCombo();          // Heavenly Demon Divine Palm
    void ExecuteChargedAttack();       // Red Soul Charge
    void ExecuteDashCombo();           // Reign of Heavenly Demon Step
    void ExecuteAerialAttack();        // Heavenly Demon's Spiral
    
    // S+Direction skills
    void ExecuteHeavenlyDemonPower();  // S+Up
    void ExecuteBlackNightFourthMoon(); // S+Right (2nd Bond)
    void ExecuteMindSplitDoubleWill(); // S+Left
    
    // Gear skills
    void ExecuteDemonDestroysWorld();  // AS (Weapon)
    void ExecuteIntimidationDress();   // AD (Helmet)
    void ExecuteDarkFlowerFlame();     // ASD (Armor)
    void ExecuteBigMeteor();           // SD (THE BIG METEOR)
    
    // Visual properties
    struct DarkVisualConfig {
        static constexpr float COLOR_R = 0.8f;
        static constexpr float COLOR_G = 0.1f;
        static constexpr float COLOR_B = 0.1f;
        static constexpr float COLOR_A = 0.9f;
        
        static constexpr const char* PARTICLE_TYPE = "DEMON_SOULS";
        static constexpr const char* TRAIL_TYPE = "SCREAMING_FACES";
        static constexpr const char* HIT_EFFECT = "DARK_FLOWER_PETALS";
        static constexpr const char* ENERGY_TYPE = "CHAOTIC_CORRUPTING";
    };
    
    // Frame data
    struct DarkFrameData {
        static constexpr int BASIC_COMBO_STARTUP = 8;
        static constexpr int BASIC_COMBO_ACTIVE = 5;
        static constexpr int BASIC_COMBO_RECOVERY = 12;
        
        static constexpr int CHARGED_STARTUP = 25;
        static constexpr int CHARGED_ACTIVE = 8;
        static constexpr int CHARGED_RECOVERY = 30;
        
        static constexpr int DASH_COMBO_STARTUP = 10;
        static constexpr int DASH_COMBO_ACTIVE = 12;
        static constexpr int DASH_COMBO_RECOVERY = 15;
        
        static constexpr int AERIAL_STARTUP = 12;
        static constexpr int AERIAL_ACTIVE = 10;
        static constexpr int AERIAL_RECOVERY = 18;
    };
    
    // Combat properties
    struct DarkCombatData {
        static constexpr float DAMAGE_MODIFIER = 1.25f;
        static constexpr float SPEED_MODIFIER = 0.9f;
        static constexpr float RANGE_MODIFIER = 1.0f;
        static constexpr float QI_EFFICIENCY = 1.15f; // Uses more Qi
    };
    
    // Big Meteor properties
    struct BigMeteorConfig {
        static constexpr float TRACKING_SPEED = 2.0f;
        static constexpr float IMPACT_RADIUS = 8.0f;
        static constexpr float FALL_DURATION = 3.0f;
        static constexpr bool HAS_DEMON_FACE = true;
        static constexpr bool HAS_FIRE_TRAIL = true;
    };
    
    // Get stance properties
    DarkVisualConfig GetVisualConfig() const;
    DarkFrameData GetFrameData() const;
    DarkCombatData GetCombatData() const;
    BigMeteorConfig GetMeteorConfig() const;
    
private:
    HyukWoonSung* m_owner;
    
    // Combat state
    bool m_isExecutingCombo;
    int m_currentComboHit;
    float m_comboTimer;
    
    // Special state tracking
    bool m_intimidationActive;
    float m_intimidationDuration;
    std::vector<float> m_fearTargets;
    
    // Animation state
    bool m_isCharging;
    float m_chargeTime;
    
    // Big Meteor tracking
    struct MeteorInstance {
        float x, y, z;
        float vx, vy, vz;
        float lifetime;
        bool isTracking;
        void* target;
    };
    std::vector<MeteorInstance> m_activeMeteors;
    
    // Helper functions
    void ResetComboState();
    void UpdateIntimidation(float deltaTime);
    void UpdateMeteors(float deltaTime);
    void CheckComboExtensions();
    
    // Combat calculations
    float CalculateBaseDamage(int hitNumber) const;
    float ApplyCombatModifiers(float baseDamage) const;
    
    // Visual helpers
    void CreateDemonSoulEffect();
    void CreateRedEnergyTrail();
    void CreateDarkExplosion();
    void CreateDemonAura();
    void CreatePalmShockwave();
    void CreateMeteorPortal();
    void CreateScreamingFaceTrail();
};

} // namespace ArenaFighter