#include "HyukWoonSung.h"
#include "Visuals/StanceVFX.h"
#include "Visuals/DragonGauge.h"
#include <algorithm>
#include <cmath>

namespace ArenaFighter {

HyukWoonSung::HyukWoonSung()
    : CharacterBase("Hyuk Woon Sung", CharacterCategory::Murim),
      m_currentStance(StanceType::LIGHT_STANCE),
      m_stanceSwitchCooldown(0.0f),
      m_isInUltimate(false),
      m_mastersVengeanceActive(false),
      m_ultimateDuration(0.0f),
      m_mastersVengeanceDuration(0.0f),
      m_currentHP(BASE_HP),
      m_maxHP(BASE_HP),
      m_temperedTrueBlossomGauge(0.0f),
      m_qiEnergy(BASE_QI),
      m_qiRegenRate(QI_REGEN_RATE),
      m_lifesteal(0.0f),
      m_stanceSwitchCreatesShockwave(false),
      m_comboCounterNoReset(false),
      m_attacksCreateDualExplosions(false) {
    
    EnableStanceSystem();
    
    // Initialize VFX systems
    m_stanceVFX = std::make_unique<StanceVFX>();
    m_dragonGauge = std::make_unique<DragonGauge>();
    
    InitializeSkills();
}

HyukWoonSung::~HyukWoonSung() = default;

void HyukWoonSung::Update(float deltaTime) {
    CharacterBase::Update(deltaTime);
    
    // Update cooldowns
    UpdateCooldowns(deltaTime);
    
    // Regenerate Qi
    if (!m_isInUltimate) {
        GenerateQi(m_qiRegenRate * deltaTime);
    }
    
    // Update stance switch cooldown
    if (m_stanceSwitchCooldown > 0) {
        m_stanceSwitchCooldown -= deltaTime;
    }
    
    // Check ultimate conditions
    CheckUltimateConditions();
    
    // Check Master's Vengeance
    CheckMastersVengeance();
    
    // Update ultimate duration
    if (m_isInUltimate && m_ultimateDuration > 0) {
        m_ultimateDuration -= deltaTime;
        if (m_ultimateDuration <= 0) {
            m_isInUltimate = false;
            m_attacksCreateDualExplosions = false;
        }
    }
    
    // Update Master's Vengeance duration
    if (m_mastersVengeanceActive && m_mastersVengeanceDuration > 0) {
        m_mastersVengeanceDuration -= deltaTime;
        if (m_mastersVengeanceDuration <= 0) {
            m_mastersVengeanceActive = false;
            m_lifesteal = 0.0f;
            m_stanceSwitchCreatesShockwave = false;
            m_comboCounterNoReset = false;
            m_qiRegenRate = QI_REGEN_RATE;
        }
    }
    
    // Update VFX
    if (m_stanceVFX) {
        m_stanceVFX->Update(deltaTime);
    }
    if (m_dragonGauge) {
        m_dragonGauge->Update(deltaTime, m_temperedTrueBlossomGauge);
    }
}

void HyukWoonSung::Render() {
    if (m_stanceVFX) {
        m_stanceVFX->Render();
    }
    if (m_dragonGauge) {
        m_dragonGauge->Render();
    }
}

void HyukWoonSung::SwitchStance() {
    if (m_stanceSwitchCooldown > 0) return;
    
    CreateYinYangEffect();
    
    if (m_currentStance == StanceType::LIGHT_STANCE) {
        m_currentStance = StanceType::DARK_STANCE;
        CreateRedSmokeEffect();
    } else {
        m_currentStance = StanceType::LIGHT_STANCE;
        CreateBlueShardEffect();
    }
    
    // Create shockwave if Master's Vengeance is active
    if (m_stanceSwitchCreatesShockwave) {
        // Implementation handled by combat system
    }
    
    m_stanceSwitchCooldown = STANCE_SWITCH_COOLDOWN;
    PlayStanceAudio();
    
    if (m_stanceVFX) {
        m_stanceVFX->PlayStanceSwitchEffect(m_currentStance);
    }
}

void HyukWoonSung::ExecuteBasicCombo() {
    if (m_currentStance == StanceType::LIGHT_STANCE) {
        SevenStarsOfTheSpear();
    } else {
        HeavenlyDemonDivinePalm();
    }
}

void HyukWoonSung::ExecuteSpecialMove(int skillId) {
    if (skillId >= 0 && skillId < 8) {
        const GearSkill& skill = m_gearSkills[skillId];
        // Skill execution handled by combat system
    }
}

// Universal Combat
void HyukWoonSung::BasicSpearChain() {
    // 4-hit basic chain
    for (int i = 0; i < 4; i++) {
        float damage = 25.0f + (i * 5.0f);
        // Apply damage through combat system
    }
}

void HyukWoonSung::PowerThrust() {
    // Charged thrust attack
    float damage = 60.0f;
    // Apply damage through combat system
}

void HyukWoonSung::RushStrikes() {
    // Dash into double strike
    float damage1 = 35.0f;
    float damage2 = 40.0f;
    // Apply damage through combat system
}

void HyukWoonSung::AerialSpin() {
    // Jumping spinning attack
    float damage = 45.0f;
    // Apply damage through combat system
}

void HyukWoonSung::DivingDragon() {
    // Jump hold attack - diving spear
    float damage = 70.0f;
    // Apply damage through combat system
}

// Light Stance Combat
void HyukWoonSung::SevenStarsOfTheSpear() {
    if (m_currentStance != StanceType::LIGHT_STANCE) return;
    
    // 7 rapid attacks with escalating damage
    float baseDamage = 15.0f;
    for (int i = 0; i < 7; i++) {
        float damage = baseDamage + (i * 3.0f);
        
        if (i < 3) {
            // Lightning-fast thrusts
            if (m_stanceVFX) m_stanceVFX->CreateThrustEffect(StanceVFX::BLUE);
        } else if (i < 6) {
            // Circular spear spins
            if (m_stanceVFX) m_stanceVFX->CreateStarPattern();
        } else {
            // Final thrust with 7 blue stars
            if (m_stanceVFX) m_stanceVFX->CreateBlueStarProjectiles(7);
        }
        
        // Apply damage through combat system
    }
}

void HyukWoonSung::DivineSpearOfEndingNight() {
    if (m_currentStance != StanceType::LIGHT_STANCE) return;
    
    // Charge phase
    if (m_stanceVFX) m_stanceVFX->ChargeEnergy(StanceVFX::BLUE, 1.0f);
    
    // Release crescent wave
    if (m_stanceVFX) {
        m_stanceVFX->CreateCrescentWave(StanceVFX::BLUE, true); // fullscreen
        m_stanceVFX->BrightenScreen(0.5f);
    }
    
    float damage = 85.0f;
    // Apply damage through combat system
}

void HyukWoonSung::TravelOfTheGale() {
    if (m_currentStance != StanceType::LIGHT_STANCE) return;
    
    // 3-hit dash combo
    for (int i = 0; i < 3; i++) {
        float damage = 30.0f + (i * 5.0f);
        // Apply damage through combat system
    }
}

void HyukWoonSung::AerialSpearDance() {
    if (m_currentStance != StanceType::LIGHT_STANCE) return;
    
    // Aerial spinning attack
    float damage = 55.0f;
    // Apply damage through combat system
}

// Dark Stance Combat
void HyukWoonSung::HeavenlyDemonDivinePalm() {
    if (m_currentStance != StanceType::DARK_STANCE) return;
    
    // 4-hit palm combo with escalating effects
    float damages[4] = {20.0f, 25.0f, 30.0f, 40.0f};
    
    for (int i = 0; i < 4; i++) {
        if (m_stanceVFX) {
            switch (i) {
                case 0:
                    m_stanceVFX->CreatePalmStrike(StanceVFX::RED);
                    m_stanceVFX->CreateShockwave(StanceVFX::RED);
                    break;
                case 1:
                    m_stanceVFX->CreateDoublePalm();
                    m_stanceVFX->CreateExplosion(StanceVFX::RED);
                    break;
                case 2:
                    m_stanceVFX->CreateSpinningPalm();
                    m_stanceVFX->CreateDarkTrail();
                    break;
                case 3:
                    m_stanceVFX->CreateDemonFaceProjection();
                    // Launch enemy
                    break;
            }
        }
        
        // Apply damage through combat system
    }
}

void HyukWoonSung::RedSoulCharge() {
    if (m_currentStance != StanceType::DARK_STANCE) return;
    
    // Charged dark energy attack
    if (m_stanceVFX) {
        m_stanceVFX->ChargeEnergy(StanceVFX::RED, 1.5f);
    }
    
    float damage = 90.0f;
    // Apply damage through combat system
}

void HyukWoonSung::ReignOfHeavenlyDemonStep() {
    if (m_currentStance != StanceType::DARK_STANCE) return;
    
    // 2-hit dash combo with demon step
    for (int i = 0; i < 2; i++) {
        float damage = 40.0f + (i * 10.0f);
        // Apply damage through combat system
    }
}

void HyukWoonSung::HeavenlyDemonsSpiral() {
    if (m_currentStance != StanceType::DARK_STANCE) return;
    
    // Spiral attack from jump
    float damage = 65.0f;
    // Apply damage through combat system
}

// S+Direction Skills
void HyukWoonSung::ExecuteDirectionalSkill(Direction dir) {
    if (m_currentStance == StanceType::LIGHT_STANCE) {
        // Light stance S skills
        switch (dir) {
            case Direction::UP:
                if (m_lightSkillCooldowns[0].currentCooldown <= 0) {
                    SpearSeaImpact();
                    m_lightSkillCooldowns[0].currentCooldown = SkillCooldowns::SPEAR_SEA_IMPACT;
                }
                break;
            case Direction::RIGHT:
                if (m_lightSkillCooldowns[1].currentCooldown <= 0) {
                    DivineWindOfThePast();
                    m_lightSkillCooldowns[1].currentCooldown = SkillCooldowns::DIVINE_WIND;
                }
                break;
            case Direction::LEFT:
                if (m_lightSkillCooldowns[2].currentCooldown <= 0) {
                    LightningStitchingArt();
                    m_lightSkillCooldowns[2].currentCooldown = SkillCooldowns::LIGHTNING_STITCH;
                }
                break;
            default: break;
        }
    } else {
        // Dark stance S skills
        switch (dir) {
            case Direction::UP:
                if (m_darkSkillCooldowns[0].currentCooldown <= 0) {
                    HeavenlyDemonDivinePower();
                    m_darkSkillCooldowns[0].currentCooldown = SkillCooldowns::HEAVENLY_DEMON_POWER;
                }
                break;
            case Direction::RIGHT:
                if (m_darkSkillCooldowns[1].currentCooldown <= 0) {
                    BlackNightOfFourthMoon();
                    m_darkSkillCooldowns[1].currentCooldown = SkillCooldowns::BLACK_NIGHT;
                }
                break;
            case Direction::LEFT:
                if (m_darkSkillCooldowns[2].currentCooldown <= 0) {
                    MindSplitDoubleWill();
                    m_darkSkillCooldowns[2].currentCooldown = SkillCooldowns::MIND_SPLIT;
                }
                break;
            default: break;
        }
    }
    
    // Ultimate S skills
    if (m_isInUltimate) {
        switch (dir) {
            case Direction::UP: SixBondsUnited(); break;
            case Direction::RIGHT: DeathMoonOfDarkNight(); break;
            case Direction::LEFT: BlueOceanDestruction(); break;
            case Direction::DOWN: FingerWindPointPressing(); break;
        }
    }
}

// Light S Skills Implementation
void HyukWoonSung::SpearSeaImpact() {
    // 5th Bond technique
    ConsumeQi(25.0f);
    if (m_stanceVFX) {
        m_stanceVFX->CreateSpearSeaEffect();
    }
    float damage = 120.0f;
    // Apply damage through combat system
}

void HyukWoonSung::DivineWindOfThePast() {
    // 3rd Bond technique
    ConsumeQi(20.0f);
    if (m_stanceVFX) {
        m_stanceVFX->CreateDivineWindEffect();
    }
    float damage = 80.0f;
    // Apply damage through combat system
}

void HyukWoonSung::LightningStitchingArt() {
    ConsumeQi(22.0f);
    if (m_stanceVFX) {
        m_stanceVFX->CreateLightningStitchEffect();
    }
    float damage = 95.0f;
    // Apply damage through combat system
}

// Dark S Skills Implementation
void HyukWoonSung::HeavenlyDemonDivinePower() {
    ConsumeQi(30.0f);
    if (m_stanceVFX) {
        m_stanceVFX->CreateHeavenlyDemonPowerEffect();
    }
    float damage = 110.0f;
    // Apply damage through combat system
}

void HyukWoonSung::BlackNightOfFourthMoon() {
    // 2nd Bond technique
    ConsumeQi(18.0f);
    if (m_stanceVFX) {
        m_stanceVFX->CreateBlackNightEffect();
    }
    float damage = 75.0f;
    // Apply damage through combat system
}

void HyukWoonSung::MindSplitDoubleWill() {
    ConsumeQi(24.0f);
    if (m_stanceVFX) {
        m_stanceVFX->CreateMindSplitEffect();
    }
    float damage = 100.0f;
    // Apply damage through combat system
}

// Gear Skills - Light Stance
void HyukWoonSung::GlassyDeathRain() {
    ConsumeQi(35.0f);
    if (m_stanceVFX) {
        m_stanceVFX->CreateGlassyRainEffect();
    }
    float damage = 150.0f;
    // Apply damage through combat system
}

void HyukWoonSung::SpearAura() {
    ConsumeQi(40.0f);
    if (m_stanceVFX) {
        m_stanceVFX->CreateSpearAuraEffect();
    }
    // Buff effect
}

void HyukWoonSung::FlowOfTheDivineDragon() {
    ConsumeQi(50.0f);
    if (m_stanceVFX) {
        m_stanceVFX->CreateDivineDragonFlow();
    }
    float damage = 200.0f;
    // Apply damage through combat system
}

void HyukWoonSung::ThunderousFlyingSpear() {
    ConsumeQi(30.0f);
    if (m_stanceVFX) {
        m_stanceVFX->CreateThunderSpearEffect();
    }
    float damage = 130.0f;
    // Apply damage through combat system
}

// Gear Skills - Dark Stance
void HyukWoonSung::HeavenlyDemonDestroysTheWorld() {
    ConsumeQi(40.0f);
    if (m_stanceVFX) {
        m_stanceVFX->CreateWorldDestructionEffect();
    }
    float damage = 180.0f;
    // Apply damage through combat system
}

void HyukWoonSung::IntimidationDress() {
    ConsumeQi(60.0f);
    if (m_stanceVFX) {
        m_stanceVFX->CreateIntimidationEffect();
    }
    // Fear effect on enemies
}

void HyukWoonSung::DarkFlowerRedHeartFlame() {
    ConsumeQi(55.0f);
    if (m_stanceVFX) {
        m_stanceVFX->CreateDarkFlowerEffect();
    }
    float damage = 210.0f;
    // Apply damage through combat system
}

void HyukWoonSung::RainOfStarDestruction() {
    // THE BIG METEOR
    ConsumeQi(45.0f);
    
    if (m_stanceVFX) {
        m_stanceVFX->CreateSkyPortal(true); // massive
        m_stanceVFX->CreateBigMeteor();
    }
    
    // 40% max HP damage
    float damage = m_maxHP * BIG_METEOR_DAMAGE_PERCENT;
    // Apply damage through combat system
}

// Ultimate System
void HyukWoonSung::ActivateDivineArts() {
    if (m_qiEnergy < 100.0f || GetHPPercent() < 100.0f) return;
    
    ConsumeQi(100.0f);
    
    // Transformation animation
    CreateGoldBlackPillar();
    CreateEtherealWings();
    
    m_isInUltimate = true;
    m_ultimateDuration = ULTIMATE_DURATION;
    m_attacksCreateDualExplosions = true;
    
    if (m_stanceVFX) {
        m_stanceVFX->PlayUltimateTransformation();
    }
}

void HyukWoonSung::MastersVengeance() {
    if (m_mastersVengeanceActive || GetHPPercent() > MASTERS_VENGEANCE_HP_THRESHOLD * 100.0f) {
        return;
    }
    
    CreateGhostlyImage("Nok Yu On");
    CreateGhostlyImage("Chun Hwi");
    
    // Apply Master's Vengeance effects
    m_lifesteal = MASTERS_VENGEANCE_LIFESTEAL;
    m_stanceSwitchCreatesShockwave = true;
    m_comboCounterNoReset = true;
    m_qiRegenRate *= 3.0f;
    
    m_mastersVengeanceActive = true;
    m_mastersVengeanceDuration = MASTERS_VENGEANCE_DURATION;
    
    if (m_stanceVFX) {
        m_stanceVFX->PlayMastersVengeanceEffect();
    }
}

// Ultimate S Skills
void HyukWoonSung::SixBondsUnited() {
    if (!m_isInUltimate) return;
    
    if (m_stanceVFX) {
        m_stanceVFX->CreateSixBondsEffect();
    }
    float damage = 300.0f;
    // Apply damage through combat system
}

void HyukWoonSung::DeathMoonOfDarkNight() {
    if (!m_isInUltimate) return;
    
    // 4th Bond technique
    if (m_stanceVFX) {
        m_stanceVFX->CreateDeathMoonEffect();
    }
    float damage = 250.0f;
    // Apply damage through combat system
}

void HyukWoonSung::BlueOceanDestruction() {
    if (!m_isInUltimate) return;
    
    // 6th Bond technique
    if (m_stanceVFX) {
        m_stanceVFX->CreateBlueOceanEffect();
    }
    float damage = 280.0f;
    // Apply damage through combat system
}

void HyukWoonSung::FingerWindPointPressing() {
    if (!m_isInUltimate) return;
    
    if (m_stanceVFX) {
        m_stanceVFX->CreateFingerWindEffect();
    }
    float damage = 200.0f;
    // Apply damage through combat system
}

// Helper functions
void HyukWoonSung::ConsumeQi(float amount) {
    m_qiEnergy = std::max(0.0f, m_qiEnergy - amount);
}

void HyukWoonSung::GenerateQi(float amount) {
    m_qiEnergy = std::min(100.0f, m_qiEnergy + amount);
}

void HyukWoonSung::TakeDamage(float damage) {
    float actualDamage = CalculateDamage(damage);
    m_currentHP = std::max(0.0f, m_currentHP - actualDamage);
    
    // Apply lifesteal if active
    if (m_lifesteal > 0.0f) {
        ApplyLifesteal(actualDamage);
    }
}

void HyukWoonSung::Heal(float amount) {
    m_currentHP = std::min(m_maxHP, m_currentHP + amount);
}

void HyukWoonSung::InitializeSkills() {
    // Initialize light stance cooldowns
    m_lightSkillCooldowns.resize(3);
    m_lightSkillCooldowns[0] = {SkillCooldowns::SPEAR_SEA_IMPACT, 0.0f};
    m_lightSkillCooldowns[1] = {SkillCooldowns::DIVINE_WIND, 0.0f};
    m_lightSkillCooldowns[2] = {SkillCooldowns::LIGHTNING_STITCH, 0.0f};
    
    // Initialize dark stance cooldowns
    m_darkSkillCooldowns.resize(3);
    m_darkSkillCooldowns[0] = {SkillCooldowns::HEAVENLY_DEMON_POWER, 0.0f};
    m_darkSkillCooldowns[1] = {SkillCooldowns::BLACK_NIGHT, 0.0f};
    m_darkSkillCooldowns[2] = {SkillCooldowns::MIND_SPLIT, 0.0f};
    
    // Initialize gear skill cooldowns
    m_gearSkillCooldowns.resize(8);
    m_gearSkillCooldowns[0] = {SkillCooldowns::GLASSY_DEATH_RAIN, 0.0f};
    m_gearSkillCooldowns[1] = {SkillCooldowns::SPEAR_AURA, 0.0f};
    m_gearSkillCooldowns[2] = {SkillCooldowns::DIVINE_DRAGON_FLOW, 0.0f};
    m_gearSkillCooldowns[3] = {SkillCooldowns::THUNDER_SPEAR, 0.0f};
    m_gearSkillCooldowns[4] = {SkillCooldowns::DEMON_DESTROYS_WORLD, 0.0f};
    m_gearSkillCooldowns[5] = {SkillCooldowns::INTIMIDATION_DRESS, 0.0f};
    m_gearSkillCooldowns[6] = {SkillCooldowns::DARK_FLOWER, 0.0f};
    m_gearSkillCooldowns[7] = {SkillCooldowns::BIG_METEOR, 0.0f};
    
    // Initialize gear skills
    m_gearSkills[0] = {"Glassy Death Rain", "glassy_rain", 35.0f, 150.0f, 8.0f, 15, 5, 20};
    m_gearSkills[1] = {"Spear Aura", "spear_aura", 40.0f, 0.0f, 0.0f, 10, 30, 15}; // Buff skill
    m_gearSkills[2] = {"Flow of Divine Dragon", "divine_flow", 50.0f, 200.0f, 10.0f, 20, 8, 25};
    m_gearSkills[3] = {"Thunderous Flying Spear", "thunder_spear", 30.0f, 130.0f, 12.0f, 12, 4, 18};
    m_gearSkills[4] = {"Demon Destroys World", "demon_world", 40.0f, 180.0f, 8.0f, 18, 6, 22};
    m_gearSkills[5] = {"Intimidation Dress", "intimidation", 60.0f, 0.0f, 15.0f, 15, 60, 20}; // Fear effect
    m_gearSkills[6] = {"Dark Flower Red Heart", "dark_flower", 55.0f, 210.0f, 9.0f, 16, 7, 24};
    m_gearSkills[7] = {"Rain of Star Destruction", "big_meteor", 45.0f, 400.0f, 20.0f, 30, 15, 60};
}

void HyukWoonSung::UpdateCooldowns(float deltaTime) {
    for (auto& cooldown : m_lightSkillCooldowns) {
        if (cooldown.currentCooldown > 0) {
            cooldown.currentCooldown -= deltaTime;
        }
    }
    
    for (auto& cooldown : m_darkSkillCooldowns) {
        if (cooldown.currentCooldown > 0) {
            cooldown.currentCooldown -= deltaTime;
        }
    }
    
    for (auto& cooldown : m_gearSkillCooldowns) {
        if (cooldown.currentCooldown > 0) {
            cooldown.currentCooldown -= deltaTime;
        }
    }
}

void HyukWoonSung::CheckUltimateConditions() {
    if (!m_isInUltimate && m_qiEnergy >= 100.0f && GetHPPercent() >= 100.0f) {
        // Ultimate ready - player can activate with special input
    }
}

void HyukWoonSung::CheckMastersVengeance() {
    if (!m_mastersVengeanceActive && GetHPPercent() <= MASTERS_VENGEANCE_HP_THRESHOLD * 100.0f) {
        MastersVengeance();
    }
}

void HyukWoonSung::PlayStanceAudio() {
    // Audio implementation handled by audio system
}

float HyukWoonSung::CalculateDamage(float baseDamage) {
    // Apply LSFDC damage formula
    float damage = baseDamage * (100.0f / (100.0f + 100.0f)); // Base defense of 100
    return damage;
}

void HyukWoonSung::ApplyLifesteal(float damage) {
    float healAmount = damage * m_lifesteal;
    Heal(healAmount);
}

// Visual effect helpers (will be implemented in VFX classes)
void HyukWoonSung::CreateYinYangEffect() {
    if (m_stanceVFX) {
        m_stanceVFX->PlayYinYangShatter();
    }
}

void HyukWoonSung::CreateRedSmokeEffect() {
    if (m_stanceVFX) {
        m_stanceVFX->CreateRedSmoke();
    }
}

void HyukWoonSung::CreateBlueShardEffect() {
    if (m_stanceVFX) {
        m_stanceVFX->CreateBlueShards();
    }
}

void HyukWoonSung::CreateGoldBlackPillar() {
    if (m_stanceVFX) {
        m_stanceVFX->CreateUltimatePillar();
    }
}

void HyukWoonSung::CreateEtherealWings() {
    if (m_stanceVFX) {
        m_stanceVFX->CreateWings();
    }
}

void HyukWoonSung::CreateGhostlyImage(const std::string& name) {
    if (m_stanceVFX) {
        m_stanceVFX->CreateGhostMaster(name);
    }
}

} // namespace ArenaFighter