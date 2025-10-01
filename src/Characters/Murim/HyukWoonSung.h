#pragma once

#include "../CharacterBase.h"
#include "StanceSystem.h"
#include <memory>

namespace ArenaFighter {

/**
 * @brief Hyuk Woon Sung - S-Tier Unique Murim Character
 * 
 * Master martial artist with dual stance system:
 * - Light Stance (Orthodox Spear): Precise, defensive, blue-themed
 * - Dark Stance (Heavenly Demon): Aggressive, powerful, red-themed
 * 
 * Features unique Tempered True Blossom gauge and stance-based movesets
 */
class HyukWoonSung : public CharacterBase {
public:
    HyukWoonSung();
    ~HyukWoonSung() = default;
    
    // Character-specific overrides
    void Initialize() override;
    void Update(float deltaTime) override;
    void OnGearSwitch(int oldGear, int newGear) override;
    void OnSkillUse(int skillIndex) override;
    void OnSpecialMoveExecute(InputDirection direction) override;
    
    // Stance system
    bool HasStanceSystem() const override { return true; }
    void SwitchStance(int stanceIndex) override;
    int GetCurrentStance() const override;
    
    // Special moves (MANA-based, no cooldowns) - DEPRECATED
    // These are now handled through the S+Direction special move system
    void SpearSeaImpact();      // Light stance special
    void DivineWind();           // Light stance movement
    void LightningStitching();   // Light stance combo
    void HeavenlyDemonPalm();    // Dark stance special
    void RedSoulCharge();        // Dark stance buff
    
    // Ultimate (requires full Tempered True Blossom gauge)
    void DivineArtsOfTheUnrecorded();
    
    // Stance-specific attacks
    void ExecuteLightAttack();   // Changes based on stance
    void ExecuteMediumAttack();  // Changes based on stance
    void ExecuteHeavyAttack();   // Changes based on stance
    
    // Unique mechanics
    float GetTemperedGauge() const;
    void BuildGauge(float amount);
    
private:
    std::unique_ptr<StanceSystem> m_stanceSystem;
    
    // Special move mana costs (S+Direction, NO COOLDOWNS)
    static constexpr float SPEAR_SEA_MANA = 25.0f;                  // Light S+↑
    static constexpr float DIVINE_WIND_MANA = 20.0f;                // Light S+→
    static constexpr float LIGHTNING_STITCH_MANA = 30.0f;           // Light S+←
    static constexpr float PIERCING_HEAVEN_MANA = 35.0f;            // Light S+↓
    static constexpr float HEAVENLY_DEMON_DIVINE_MANA = 25.0f;      // Dark S+↑
    static constexpr float BLACK_NIGHT_FOURTH_MOON_MANA = 30.0f;    // Dark S+→
    static constexpr float MIND_SPLIT_DOUBLE_WILL_MANA = 35.0f;     // Dark S+←
    static constexpr float DEMON_GOD_STOMP_MANA = 40.0f;            // Dark S+↓
    static constexpr float ULTIMATE_MANA = 70.0f;
    
    // Special move damage values
    static constexpr float SPEAR_SEA_DAMAGE = 150.0f;
    static constexpr float DIVINE_WIND_DAMAGE = 80.0f;
    static constexpr float LIGHTNING_STITCH_DAMAGE = 120.0f;
    static constexpr float PIERCING_HEAVEN_DAMAGE = 170.0f;
    static constexpr float HEAVENLY_DEMON_DIVINE_DAMAGE = 180.0f;
    static constexpr float BLACK_NIGHT_FOURTH_MOON_DAMAGE = 160.0f;
    static constexpr float MIND_SPLIT_DOUBLE_WILL_DAMAGE = 200.0f;
    static constexpr float DEMON_GOD_STOMP_DAMAGE = 190.0f;
    static constexpr float ULTIMATE_DAMAGE = 350.0f;
    
    // Character-specific properties
    float m_comboMultiplier = 1.0f;
    bool m_isInUltimate = false;
    float m_ultimateTimer = 0.0f;
    
    // Helper methods
    void SetupLightStanceSkills();
    void SetupDarkStanceSkills();
    void InitializeSpecialMoves();  // Setup S+Direction special moves
    void UpdateStanceEffects();
    void ApplyStanceModifiers();
    
    // Special move execution helpers
    void ExecuteSpearSeaImpact();
    void ExecuteDivineWindOfThePast();
    void ExecuteLightningStitchingArt();
    void ExecutePiercingHeavenSpear();
    void ExecuteHeavenlyDemonDivinePower();
    void ExecuteBlackNightOfFourthMoon();
    void ExecuteMindSplitDoubleWill();
    void ExecuteDemonGodStomp();
    
    // Visual effect helpers
    void PlayStanceEffect(const std::string& effect);
    void UpdateAuraVisuals();
};

} // namespace ArenaFighter