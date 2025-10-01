#pragma once

#include "../HyukWoonSung.h"

namespace ArenaFighter {

// Forward declaration
class HyukWoonSung;

class LightStance {
public:
    LightStance(HyukWoonSung* owner);
    ~LightStance();
    
    // Core stance operations
    void Enter();
    void Exit();
    void Update(float deltaTime);
    
    // Combat moves
    void ExecuteBasicCombo();          // Seven Stars of the Spear
    void ExecuteChargedAttack();       // Divine Spear of Ending Night
    void ExecuteDashCombo();           // Travel of the Gale
    void ExecuteAerialAttack();        // Aerial Spear Dance
    
    // S+Direction skills
    void ExecuteSpearSeaImpact();      // S+Up (5th Bond)
    void ExecuteDivineWindOfThePast(); // S+Right (3rd Bond)
    void ExecuteLightningStitching();  // S+Left
    
    // Gear skills
    void ExecuteGlassyDeathRain();     // AS (Weapon)
    void ExecuteSpearAura();           // AD (Helmet)
    void ExecuteDivineDragonFlow();    // ASD (Armor)
    void ExecuteThunderousSpear();     // SD (Trinket)
    
    // Visual properties
    struct LightVisualConfig {
        static constexpr float COLOR_R = 0.3f;
        static constexpr float COLOR_G = 0.6f;
        static constexpr float COLOR_B = 1.0f;
        static constexpr float COLOR_A = 0.8f;
        
        static constexpr const char* PARTICLE_TYPE = "STARS_AND_LIGHT";
        static constexpr const char* TRAIL_TYPE = "BLUE_ENERGY_TRAIL";
        static constexpr const char* HIT_EFFECT = "STAR_SPARKLES";
        static constexpr const char* ENERGY_TYPE = "CLEAN_AND_SHARP";
    };
    
    // Frame data
    struct LightFrameData {
        static constexpr int BASIC_COMBO_STARTUP = 5;
        static constexpr int BASIC_COMBO_ACTIVE = 3;
        static constexpr int BASIC_COMBO_RECOVERY = 8;
        
        static constexpr int CHARGED_STARTUP = 20;
        static constexpr int CHARGED_ACTIVE = 10;
        static constexpr int CHARGED_RECOVERY = 25;
        
        static constexpr int DASH_COMBO_STARTUP = 8;
        static constexpr int DASH_COMBO_ACTIVE = 15;
        static constexpr int DASH_COMBO_RECOVERY = 12;
        
        static constexpr int AERIAL_STARTUP = 6;
        static constexpr int AERIAL_ACTIVE = 8;
        static constexpr int AERIAL_RECOVERY = 15;
    };
    
    // Combat properties
    struct LightCombatData {
        static constexpr float DAMAGE_MODIFIER = 1.0f;
        static constexpr float SPEED_MODIFIER = 1.15f;
        static constexpr float RANGE_MODIFIER = 1.2f;
        static constexpr float QI_EFFICIENCY = 0.85f; // Uses less Qi
    };
    
    // Get stance properties
    LightVisualConfig GetVisualConfig() const;
    LightFrameData GetFrameData() const;
    LightCombatData GetCombatData() const;
    
private:
    HyukWoonSung* m_owner;
    
    // Combat state
    bool m_isExecutingCombo;
    int m_currentComboHit;
    float m_comboTimer;
    
    // Special state tracking
    bool m_spearAuraActive;
    float m_spearAuraDuration;
    
    // Animation state
    bool m_isCharging;
    float m_chargeTime;
    
    // Helper functions
    void ResetComboState();
    void UpdateSpearAura(float deltaTime);
    void CheckComboExtensions();
    
    // Combat calculations
    float CalculateBaseDamage(int hitNumber) const;
    float ApplyCombatModifiers(float baseDamage) const;
    
    // Visual helpers
    void CreateStarEffect(int count);
    void CreateBlueEnergyTrail();
    void CreateLightningEffect();
    void CreateDivineAura();
    void CreateCrescentWave();
};

} // namespace ArenaFighter