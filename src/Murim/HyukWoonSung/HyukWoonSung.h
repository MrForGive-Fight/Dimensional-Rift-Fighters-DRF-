#pragma once

#include "../../../game-project/src/Characters/CharacterBase.h"
#include "../../../game-project/src/Combat/CombatEnums.h"
#include <memory>
#include <vector>

namespace ArenaFighter {

enum class StanceType {
    LIGHT_STANCE,  // Orthodox Spear (Blue)
    DARK_STANCE    // Heavenly Demon Arts (Red)
};

enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

enum class GearSlot {
    WEAPON,
    HELMET,
    ARMOR,
    TRINKET
};

struct StanceVisuals;
struct FrameData;
class StanceVFX;
class DragonGauge;

class HyukWoonSung : public CharacterBase {
public:
    HyukWoonSung();
    ~HyukWoonSung();
    
    // Core Systems
    void Update(float deltaTime) override;
    virtual void Render();
    
    // Stance System
    void SwitchStance();  // Down+S trigger
    StanceType GetCurrentStance() const { return m_currentStance; }
    
    // Combat System
    virtual void ExecuteBasicCombo();
    virtual void ExecuteSpecialMove(int skillId);
    
    // Universal Combat
    void BasicSpearChain();     // DDDD
    void PowerThrust();          // D Hold
    void RushStrikes();          // Dash DD
    void AerialSpin();           // Jump D
    void DivingDragon();         // Jump Hold D
    
    // Light Stance Combat
    void SevenStarsOfTheSpear(); // Light DDDD (7 hits)
    void DivineSpearOfEndingNight(); // Light D Hold
    void TravelOfTheGale();      // Light Dash DDD
    void AerialSpearDance();     // Light Jump D
    
    // Dark Stance Combat
    void HeavenlyDemonDivinePalm(); // Dark DDDD
    void RedSoulCharge();         // Dark D Hold
    void ReignOfHeavenlyDemonStep(); // Dark Dash DD
    void HeavenlyDemonsSpiral();  // Dark Jump D
    
    // S+Direction Skills
    void ExecuteDirectionalSkill(Direction dir);
    
    // Light S Skills
    void SpearSeaImpact();       // S+Up (5th Bond)
    void DivineWindOfThePast();  // S+Right (3rd Bond)
    void LightningStitchingArt(); // S+Left
    
    // Dark S Skills
    void HeavenlyDemonDivinePower(); // S+Up
    void BlackNightOfFourthMoon();   // S+Right (2nd Bond)
    void MindSplitDoubleWill();      // S+Left
    
    // Gear Skills
    void ExecuteGearSkill(GearSlot slot);
    
    // Light Gear Skills
    void GlassyDeathRain();      // AS (Weapon)
    void SpearAura();            // AD (Helmet)
    void FlowOfTheDivineDragon(); // ASD (Armor)
    void ThunderousFlyingSpear(); // SD (Trinket)
    
    // Dark Gear Skills
    void HeavenlyDemonDestroysTheWorld(); // AS (Weapon)
    void IntimidationDress();     // AD (Helmet)
    void DarkFlowerRedHeartFlame(); // ASD (Armor)
    void RainOfStarDestruction(); // SD (THE BIG METEOR)
    
    // Ultimate System
    void ActivateDivineArts();   // 100 Qi + Perfect Health
    void MastersVengeance();      // Emergency at 25% HP
    
    // Ultimate S Skills
    void SixBondsUnited();        // S+Up in Ultimate
    void DeathMoonOfDarkNight();  // S+Right (4th Bond)
    void BlueOceanDestruction();  // S+Left (6th Bond)
    void FingerWindPointPressing(); // S+Down
    
    // Getters
    float GetQiEnergy() const { return m_qiEnergy; }
    float GetTemperedBlossomGauge() const { return m_temperedTrueBlossomGauge; }
    bool IsInUltimate() const { return m_isInUltimate; }
    float GetHP() const { return m_currentHP; }
    float GetMaxHP() const { return m_maxHP; }
    float GetHPPercent() const { return (m_currentHP / m_maxHP) * 100.0f; }
    
    // Combat helpers
    void ConsumeQi(float amount);
    void GenerateQi(float amount);
    void TakeDamage(float damage);
    void Heal(float amount);
    
private:
    // Stance State
    StanceType m_currentStance;
    float m_stanceSwitchCooldown;
    bool m_isInUltimate;
    bool m_mastersVengeanceActive;
    float m_ultimateDuration;
    float m_mastersVengeanceDuration;
    
    // Health
    float m_currentHP;
    float m_maxHP;
    
    // Gauge System
    float m_temperedTrueBlossomGauge;
    float m_qiEnergy;
    float m_qiRegenRate;
    
    // Combat modifiers
    float m_lifesteal;
    bool m_stanceSwitchCreatesShockwave;
    bool m_comboCounterNoReset;
    bool m_attacksCreateDualExplosions;
    
    // Visual Components
    std::unique_ptr<StanceVFX> m_stanceVFX;
    std::unique_ptr<DragonGauge> m_dragonGauge;
    
    // Frame Data for attacks
    struct AttackFrameData {
        int startup;
        int active;
        int recovery;
    };
    
    // Cooldown tracking
    struct SkillCooldown {
        float maxCooldown;
        float currentCooldown;
    };
    
    std::vector<SkillCooldown> m_lightSkillCooldowns;
    std::vector<SkillCooldown> m_darkSkillCooldowns;
    std::vector<SkillCooldown> m_gearSkillCooldowns;
    
    // Helper functions
    void InitializeSkills();
    void UpdateCooldowns(float deltaTime);
    void PlayStanceAudio();
    void CheckUltimateConditions();
    void CheckMastersVengeance();
    
    // Visual effect helpers
    void CreateYinYangEffect();
    void CreateRedSmokeEffect();
    void CreateBlueShardEffect();
    void CreateGoldBlackPillar();
    void CreateEtherealWings();
    void CreateGhostlyImage(const std::string& name);
    
    // Combat helpers
    float CalculateDamage(float baseDamage);
    void ApplyLifesteal(float damage);
};

// Constants
constexpr float BASE_HP = 1000.0f;
constexpr float BASE_QI = 100.0f;
constexpr float QI_REGEN_RATE = 2.0f;
constexpr float STANCE_SWITCH_COOLDOWN = 1.0f;
constexpr float ULTIMATE_DURATION = 20.0f;
constexpr float MASTERS_VENGEANCE_DURATION = 15.0f;
constexpr float MASTERS_VENGEANCE_HP_THRESHOLD = 0.25f;
constexpr float MASTERS_VENGEANCE_LIFESTEAL = 0.5f;
constexpr float BIG_METEOR_DAMAGE_PERCENT = 0.4f;

// Skill cooldowns (in seconds)
namespace SkillCooldowns {
    // Light stance
    constexpr float SPEAR_SEA_IMPACT = 5.0f;
    constexpr float DIVINE_WIND = 3.0f;
    constexpr float LIGHTNING_STITCH = 4.0f;
    
    // Dark stance
    constexpr float HEAVENLY_DEMON_POWER = 6.0f;
    constexpr float BLACK_NIGHT = 2.0f;
    constexpr float MIND_SPLIT = 5.0f;
    
    // Gear skills
    constexpr float GLASSY_DEATH_RAIN = 8.0f;
    constexpr float SPEAR_AURA = 10.0f;
    constexpr float DIVINE_DRAGON_FLOW = 12.0f;
    constexpr float THUNDER_SPEAR = 7.0f;
    
    constexpr float DEMON_DESTROYS_WORLD = 9.0f;
    constexpr float INTIMIDATION_DRESS = 15.0f;
    constexpr float DARK_FLOWER = 11.0f;
    constexpr float BIG_METEOR = 45.0f;
}

} // namespace ArenaFighter