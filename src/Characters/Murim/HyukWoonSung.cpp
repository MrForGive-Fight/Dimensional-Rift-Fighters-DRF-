#include "HyukWoonSung.h"
#include <algorithm>

namespace ArenaFighter {

HyukWoonSung::HyukWoonSung() 
    : CharacterBase("Hyuk Woon Sung", CharacterCategory::Murim, StatMode::Hybrid)
    , m_stanceSystem(std::make_unique<StanceSystem>()) {
    
    // S-Tier character stat adjustments
    m_maxHealth = 1100.0f;      // +10% health for S-tier
    m_currentHealth = 1100.0f;
    m_defense = 110.0f;         // +10% defense
    m_speed = 115.0f;           // +15% speed for martial artist
    m_powerModifier = 1.15f;    // +15% damage for S-tier
    m_criticalChance = 0.08f;   // 8% crit chance
    
    // Set primary element based on stance
    m_element = ElementType::Neutral; // Changes with stance
    
    // Initialize special moves (S+Direction inputs)
    InitializeSpecialMoves();
}

void HyukWoonSung::Initialize() {
    CharacterBase::Initialize();
    
    // Setup stance change callback
    m_stanceSystem->SetOnStanceChange([this](StanceType oldStance, StanceType newStance) {
        UpdateStanceEffects();
        ApplyStanceModifiers();
        // Update special moves when stance changes
        InitializeSpecialMoves();
    });
    
    // Setup initial skills based on Light stance
    SetupLightStanceSkills();
    
    // Initialize special moves for current stance
    InitializeSpecialMoves();
}

void HyukWoonSung::SetupLightStanceSkills() {
    // Gear 1: Orthodox Spear Arts (Light Stance)
    m_gearSkills[0] = {
        "Flowing River Strike",
        "SpearFlow_Light",
        15.0f,  // Mana cost
        100.0f, // Base damage
        3.0f,   // Cooldown
        AttackType::Medium,
        12, 4, 16,  // Startup, Active, Recovery
        false, true,
        ElementType::Water
    };
    
    m_gearSkills[1] = {
        "Mountain Pierce",
        "SpearPierce_Light",
        20.0f,
        120.0f,
        4.0f,   // Cooldown
        AttackType::Heavy,
        18, 5, 22,
        false, true,
        ElementType::Earth
    };
    
    // Gear 2: Divine Spear Techniques
    m_gearSkills[2] = {
        "Azure Dragon Sweep",
        "DragonSweep_Blue",
        25.0f,
        130.0f,
        5.0f,   // Cooldown
        AttackType::Heavy,
        15, 6, 20,
        false, true,
        ElementType::Wind
    };
    
    m_gearSkills[3] = {
        "Heavenly Spear Rain",
        "SpearRain_Blue",
        30.0f,
        140.0f,
        6.0f,   // Cooldown
        AttackType::Special,
        20, 8, 25,
        true, false,  // Projectile
        ElementType::Light
    };
    
    // Gear 3: Defensive Forms
    m_gearSkills[4] = {
        "Circular Guard",
        "SpearGuard_Light",
        10.0f,
        50.0f,
        2.0f,   // Cooldown
        AttackType::Light,
        5, 3, 10,
        false, true,
        ElementType::Neutral
    };
    
    m_gearSkills[5] = {
        "Counter Thrust",
        "CounterThrust_Blue",
        15.0f,
        110.0f,
        3.5f,   // Cooldown
        AttackType::Medium,
        8, 4, 14,
        false, true,
        ElementType::Light
    };
    
    // Gear 4: Ultimate Techniques
    m_gearSkills[6] = {
        "True Spear Formation",
        "SpearFormation_Ultimate",
        40.0f,
        180.0f,
        10.0f,  // Cooldown
        AttackType::Ultimate,
        25, 10, 30,
        false, false,
        ElementType::Light
    };
    
    m_gearSkills[7] = {
        "Divine Spear Ascension",
        "SpearAscend_Blue",
        35.0f,
        160.0f,
        8.0f,   // Cooldown
        AttackType::Special,
        22, 8, 28,
        false, true,
        ElementType::Wind
    };
}

void HyukWoonSung::SetupDarkStanceSkills() {
    // Gear 1: Heavenly Demon Arts (Dark Stance)
    m_gearSkills[0] = {
        "Demon Claw Strike",
        "DemonClaw_Dark",
        18.0f,
        120.0f,
        3.5f,   // Cooldown
        AttackType::Medium,
        10, 5, 15,
        false, true,
        ElementType::Dark
    };
    
    m_gearSkills[1] = {
        "Blood Moon Palm",
        "BloodPalm_Red",
        25.0f,
        150.0f,
        5.0f,   // Cooldown
        AttackType::Heavy,
        16, 6, 20,
        false, true,
        ElementType::Fire
    };
    
    // Gear 2: Destruction Techniques
    m_gearSkills[2] = {
        "Crimson Wave",
        "CrimsonWave_Dark",
        30.0f,
        160.0f,
        6.0f,   // Cooldown
        AttackType::Heavy,
        14, 7, 18,
        true, false,  // Projectile
        ElementType::Fire
    };
    
    m_gearSkills[3] = {
        "Void Rending Fist",
        "VoidFist_Red",
        35.0f,
        170.0f,
        7.0f,   // Cooldown
        AttackType::Special,
        18, 8, 24,
        false, true,
        ElementType::Void
    };
    
    // Gear 3: Aggressive Forms
    m_gearSkills[4] = {
        "Demon Rush",
        "DemonRush_Dark",
        12.0f,
        80.0f,
        2.5f,   // Cooldown
        AttackType::Light,
        6, 3, 8,
        false, true,
        ElementType::Dark
    };
    
    m_gearSkills[5] = {
        "Hell's Embrace",
        "HellGrab_Red",
        20.0f,
        140.0f,
        4.5f,   // Cooldown
        AttackType::Medium,
        12, 5, 16,
        false, false,  // Grab move
        ElementType::Fire
    };
    
    // Gear 4: Demon Lord Techniques
    m_gearSkills[6] = {
        "Asura Decimation",
        "AsuraForm_Ultimate",
        45.0f,
        220.0f,
        12.0f,  // Cooldown
        AttackType::Ultimate,
        28, 12, 35,
        false, false,
        ElementType::Dark
    };
    
    m_gearSkills[7] = {
        "Demon God Manifestation",
        "DemonGod_Red",
        40.0f,
        200.0f,
        10.0f,  // Cooldown
        AttackType::Special,
        24, 10, 30,
        false, true,
        ElementType::Void
    };
}

void HyukWoonSung::Update(float deltaTime) {
    CharacterBase::Update(deltaTime);
    
    // Update stance system
    m_stanceSystem->Update(deltaTime);
    
    // Update ultimate state
    if (m_isInUltimate) {
        m_ultimateTimer -= deltaTime;
        if (m_ultimateTimer <= 0.0f) {
            m_isInUltimate = false;
            m_ultimateTimer = 0.0f;
        }
    }
    
    // Update visual effects
    UpdateAuraVisuals();
}

void HyukWoonSung::OnGearSwitch(int oldGear, int newGear) {
    // Play gear switch effect based on stance
    if (m_stanceSystem->GetCurrentStance() == StanceType::Light) {
        PlayStanceEffect("BlueGearShift");
    } else {
        PlayStanceEffect("RedGearShift");
    }
}

void HyukWoonSung::OnSkillUse(int skillIndex) {
    // Build gauge on skill use
    m_stanceSystem->AddGauge(StanceSystem::GAUGE_PER_HIT);
    
    // Additional gauge for combo
    if (m_comboMultiplier > 1.0f) {
        m_stanceSystem->AddGauge(StanceSystem::GAUGE_PER_COMBO);
    }
}

void HyukWoonSung::SwitchStance(int stanceIndex) {
    m_stanceSystem->SwitchStance();
    
    // Play yin-yang shatter effect
    PlayStanceEffect(m_stanceSystem->GetSwitchEffect());
    
    // Update skills based on new stance
    if (m_stanceSystem->GetCurrentStance() == StanceType::Light) {
        SetupLightStanceSkills();
        m_element = ElementType::Light;
    } else {
        SetupDarkStanceSkills();
        m_element = ElementType::Dark;
    }
}

int HyukWoonSung::GetCurrentStance() const {
    return static_cast<int>(m_stanceSystem->GetCurrentStance());
}

void HyukWoonSung::SpearSeaImpact() {
    if (!CanAffordSkill(SPEAR_SEA_MANA)) return;
    
    ConsumeMana(SPEAR_SEA_MANA);
    
    // Multi-hit spear thrust attack
    float damage = SPEAR_SEA_DAMAGE * m_powerModifier;
    damage *= m_stanceSystem->GetDamageModifier();
    
    PlayStanceEffect("SpearSeaWaves_Blue");
    m_stanceSystem->AddGauge(5.0f); // Extra gauge for special move
}

void HyukWoonSung::DivineWind() {
    if (!CanAffordSkill(DIVINE_WIND_MANA)) return;
    
    ConsumeMana(DIVINE_WIND_MANA);
    
    // Movement skill with wind effect
    float damage = DIVINE_WIND_DAMAGE * m_powerModifier;
    m_speed *= 1.5f; // Temporary speed boost
    
    PlayStanceEffect("DivineWindRush_Blue");
}

void HyukWoonSung::LightningStitching() {
    if (!CanAffordSkill(LIGHTNING_STITCH_MANA)) return;
    
    ConsumeMana(LIGHTNING_STITCH_MANA);
    
    // Rapid multi-hit combo
    float damage = LIGHTNING_STITCH_DAMAGE * m_powerModifier;
    damage *= m_stanceSystem->GetDamageModifier();
    
    PlayStanceEffect("LightningNeedles_Blue");
    m_stanceSystem->AddGauge(8.0f); // High gauge build
}

void HyukWoonSung::HeavenlyDemonPalm() {
    if (!CanAffordSkill(DEMON_PALM_MANA)) return;
    
    ConsumeMana(DEMON_PALM_MANA);
    
    // Powerful palm strike
    float damage = DEMON_PALM_DAMAGE * m_powerModifier;
    damage *= m_stanceSystem->GetDamageModifier();
    
    PlayStanceEffect("DemonPalmExplosion_Red");
    m_stanceSystem->AddGauge(7.0f);
}

void HyukWoonSung::RedSoulCharge() {
    if (!CanAffordSkill(RED_SOUL_MANA)) return;
    
    ConsumeMana(RED_SOUL_MANA);
    
    // Buff self and build gauge
    m_powerModifier *= 1.25f; // Temporary damage boost
    m_stanceSystem->AddGauge(20.0f); // Large gauge gain
    
    PlayStanceEffect("RedSoulAura_Dark");
}

void HyukWoonSung::DivineArtsOfTheUnrecorded() {
    if (!CanAffordSkill(ULTIMATE_MANA) || !m_stanceSystem->CanUseUltimate()) return;
    
    ConsumeMana(ULTIMATE_MANA);
    m_stanceSystem->ConsumeGauge(100.0f);
    
    // Enter ultimate state
    m_isInUltimate = true;
    m_ultimateTimer = 10.0f; // 10 second duration
    
    // Massive damage based on stance
    float damage = ULTIMATE_DAMAGE * m_powerModifier;
    damage *= m_stanceSystem->GetDamageModifier();
    
    // Play combined stance effect
    PlayStanceEffect("DivineArts_YinYang");
    
    // Enhanced stats during ultimate
    m_powerModifier *= 1.5f;
    m_speed *= 1.3f;
}

float HyukWoonSung::GetTemperedGauge() const {
    return m_stanceSystem->GetGaugeLevel();
}

void HyukWoonSung::BuildGauge(float amount) {
    m_stanceSystem->AddGauge(amount);
}

void HyukWoonSung::ExecuteLightAttack() {
    // Stance-specific light attacks
    if (m_stanceSystem->GetCurrentStance() == StanceType::Light) {
        PlayStanceEffect("SpearJab_Blue");
    } else {
        PlayStanceEffect("DemonJab_Red");
    }
}

void HyukWoonSung::ExecuteMediumAttack() {
    // Stance-specific medium attacks
    if (m_stanceSystem->GetCurrentStance() == StanceType::Light) {
        PlayStanceEffect("SpearThrust_Blue");
    } else {
        PlayStanceEffect("DemonStrike_Red");
    }
}

void HyukWoonSung::ExecuteHeavyAttack() {
    // Stance-specific heavy attacks
    if (m_stanceSystem->GetCurrentStance() == StanceType::Light) {
        PlayStanceEffect("SpearSpin_Blue");
    } else {
        PlayStanceEffect("DemonSlam_Red");
    }
}

void HyukWoonSung::UpdateStanceEffects() {
    // Visual updates when stance changes
    UpdateAuraVisuals();
}

void HyukWoonSung::ApplyStanceModifiers() {
    // Apply stance-specific stat modifiers
    float baseDefense = 110.0f;
    float baseSpeed = 115.0f;
    
    m_defense = baseDefense * m_stanceSystem->GetDefenseModifier();
    m_speed = baseSpeed * m_stanceSystem->GetSpeedModifier();
}

void HyukWoonSung::PlayStanceEffect(const std::string& effect) {
    // This would trigger visual effects in the game
    // For now, just a placeholder
}

void HyukWoonSung::UpdateAuraVisuals() {
    // Update character's aura based on stance
    std::string aura = m_stanceSystem->GetStanceAura();
    // Apply aura visual
}

void HyukWoonSung::InitializeSpecialMoves() {
    // Clear existing special moves
    m_specialMoves.clear();
    
    if (m_stanceSystem->GetCurrentStance() == StanceType::Light) {
        // Light Stance special moves
        RegisterSpecialMove(InputDirection::Up, {
            "Spear Sea Impact",
            "SpearSea_Blue",
            SPEAR_SEA_MANA,         // 25 mana, NO cooldown
            SPEAR_SEA_DAMAGE,
            AttackType::Special,
            15, 8, 20,              // Startup, Active, Recovery
            true,                   // Projectile
            true,                   // Can combo
            true,                   // Blockable
            ElementType::Water,
            0                       // Light stance (0)
        });
        
        RegisterSpecialMove(InputDirection::Right, {
            "Divine Wind of the Past",
            "DivineWind_Blue",
            DIVINE_WIND_MANA,       // 20 mana, NO cooldown
            DIVINE_WIND_DAMAGE,
            AttackType::Medium,
            8, 5, 12,               // Fast movement skill
            false,                  // Not projectile
            true,                   // Can combo
            true,                   // Blockable
            ElementType::Wind,
            0                       // Light stance (0)
        });
        
        RegisterSpecialMove(InputDirection::Left, {
            "Lightning Stitching Art",
            "LightningStitch_Blue",
            LIGHTNING_STITCH_MANA,  // 30 mana, NO cooldown
            LIGHTNING_STITCH_DAMAGE,
            AttackType::Heavy,
            12, 10, 18,             // Multi-hit combo
            false,                  // Not projectile
            true,                   // Can combo
            true,                   // Blockable
            ElementType::Lightning,
            0                       // Light stance (0)
        });
        
        RegisterSpecialMove(InputDirection::Down, {
            "Piercing Heaven Spear",
            "PiercingHeaven_Blue",
            PIERCING_HEAVEN_MANA,   // 35 mana, NO cooldown
            PIERCING_HEAVEN_DAMAGE,
            AttackType::Heavy,
            15, 12, 20,             // Ground stab with eruptions
            false,                  // Not projectile (ground-based)
            true,                   // Can combo
            true,                   // Blockable
            ElementType::Earth,
            0                       // Light stance (0)
        });
    } else {
        // Dark Stance special moves
        RegisterSpecialMove(InputDirection::Up, {
            "Heavenly Demon Divine Power",
            "HeavenlyDemon_Red",
            HEAVENLY_DEMON_DIVINE_MANA,  // 25 mana, NO cooldown
            HEAVENLY_DEMON_DIVINE_DAMAGE,
            AttackType::Special,
            18, 6, 22,              // Powerful but slower
            false,                  // Not projectile
            true,                   // Can combo
            true,                   // Blockable
            ElementType::Dark,
            1                       // Dark stance (1)
        });
        
        RegisterSpecialMove(InputDirection::Right, {
            "Black Night of Fourth Moon",
            "BlackNight_Red",
            BLACK_NIGHT_FOURTH_MOON_MANA,  // 30 mana, NO cooldown
            BLACK_NIGHT_FOURTH_MOON_DAMAGE,
            AttackType::Heavy,
            16, 8, 20,              // Area attack
            true,                   // Projectile (dark wave)
            false,                  // Cannot combo
            true,                   // Blockable
            ElementType::Void,
            1                       // Dark stance (1)
        });
        
        RegisterSpecialMove(InputDirection::Left, {
            "Mind Split Double Will",
            "MindSplit_Red",
            MIND_SPLIT_DOUBLE_WILL_MANA,  // 35 mana, NO cooldown
            MIND_SPLIT_DOUBLE_WILL_DAMAGE,
            AttackType::Ultimate,
            20, 12, 25,             // Devastating finisher
            false,                  // Not projectile
            false,                  // Cannot combo
            false,                  // Unblockable!
            ElementType::Dark,
            1                       // Dark stance (1)
        });
        
        RegisterSpecialMove(InputDirection::Down, {
            "Demon God Stomp",
            "DemonGodStomp_Red",
            DEMON_GOD_STOMP_MANA,   // 40 mana, NO cooldown
            DEMON_GOD_STOMP_DAMAGE,
            AttackType::Ultimate,
            18, 10, 24,             // Ground pound with shockwave
            true,                   // Projectile (shockwave)
            false,                  // Cannot combo
            true,                   // Blockable
            ElementType::Fire,
            1                       // Dark stance (1)
        });
    }
}

void HyukWoonSung::OnSpecialMoveExecute(InputDirection direction) {
    // Execute stance-specific special move logic
    switch (direction) {
        case InputDirection::Up:
            if (m_stanceSystem->GetCurrentStance() == StanceType::Light) {
                ExecuteSpearSeaImpact();
            } else {
                ExecuteHeavenlyDemonDivinePower();
            }
            break;
            
        case InputDirection::Right:
            if (m_stanceSystem->GetCurrentStance() == StanceType::Light) {
                ExecuteDivineWindOfThePast();
            } else {
                ExecuteBlackNightOfFourthMoon();
            }
            break;
            
        case InputDirection::Left:
            if (m_stanceSystem->GetCurrentStance() == StanceType::Light) {
                ExecuteLightningStitchingArt();
            } else {
                ExecuteMindSplitDoubleWill();
            }
            break;
            
        case InputDirection::Down:
            if (m_stanceSystem->GetCurrentStance() == StanceType::Light) {
                ExecutePiercingHeavenSpear();
            } else {
                ExecuteDemonGodStomp();
            }
            break;
    }
    
    // Build gauge on special move use
    m_stanceSystem->AddGauge(StanceSystem::GAUGE_PER_HIT * 2.0f);
}

void HyukWoonSung::ExecuteSpearSeaImpact() {
    // Multi-hit spear thrust with water waves
    PlayStanceEffect("SpearSeaWaves_Blue");
    PlayStanceEffect("WaterImpact_Multi");
    
    // Apply damage modifiers
    float damage = SPEAR_SEA_DAMAGE * m_powerModifier;
    damage *= m_stanceSystem->GetDamageModifier();
}

void HyukWoonSung::ExecuteDivineWindOfThePast() {
    // Fast movement skill with wind trails
    PlayStanceEffect("DivineWindRush_Blue");
    PlayStanceEffect("WindTrail_Speed");
    
    // Temporary speed boost
    m_speed *= 1.5f;
    
    float damage = DIVINE_WIND_DAMAGE * m_powerModifier;
    damage *= m_stanceSystem->GetDamageModifier();
}

void HyukWoonSung::ExecuteLightningStitchingArt() {
    // Rapid multi-hit combo with lightning effects
    PlayStanceEffect("LightningNeedles_Blue");
    PlayStanceEffect("ElectricBurst_Multi");
    
    float damage = LIGHTNING_STITCH_DAMAGE * m_powerModifier;
    damage *= m_stanceSystem->GetDamageModifier();
    
    // High gauge build for combo potential
    m_stanceSystem->AddGauge(8.0f);
}

void HyukWoonSung::ExecuteHeavenlyDemonDivinePower() {
    // Powerful palm strike with dark energy
    PlayStanceEffect("DemonPalmExplosion_Red");
    PlayStanceEffect("DarkEnergy_Burst");
    
    float damage = HEAVENLY_DEMON_DIVINE_DAMAGE * m_powerModifier;
    damage *= m_stanceSystem->GetDamageModifier();
}

void HyukWoonSung::ExecuteBlackNightOfFourthMoon() {
    // Dark wave projectile attack
    PlayStanceEffect("BlackMoonWave_Red");
    PlayStanceEffect("VoidProjectile_Large");
    
    float damage = BLACK_NIGHT_FOURTH_MOON_DAMAGE * m_powerModifier;
    damage *= m_stanceSystem->GetDamageModifier();
}

void HyukWoonSung::ExecuteMindSplitDoubleWill() {
    // Unblockable finisher with mind-split effect
    PlayStanceEffect("MindSplitAura_Red");
    PlayStanceEffect("DoubleWill_Unblockable");
    PlayStanceEffect("DemonLord_Ultimate");
    
    float damage = MIND_SPLIT_DOUBLE_WILL_DAMAGE * m_powerModifier;
    damage *= m_stanceSystem->GetDamageModifier();
    
    // Massive gauge gain for ultimate setup
    m_stanceSystem->AddGauge(15.0f);
}

void HyukWoonSung::ExecutePiercingHeavenSpear() {
    // Ground stab that creates spear eruptions
    PlayStanceEffect("SpearPierce_Ground");
    PlayStanceEffect("HeavenlySpears_Eruption");
    PlayStanceEffect("EarthShatter_Blue");
    
    float damage = PIERCING_HEAVEN_DAMAGE * m_powerModifier;
    damage *= m_stanceSystem->GetDamageModifier();
    
    // Creates multiple spear eruptions from ground
    // Good for area control and anti-air
    m_stanceSystem->AddGauge(10.0f);
}

void HyukWoonSung::ExecuteDemonGodStomp() {
    // Powerful ground pound with fire shockwave
    PlayStanceEffect("DemonStomp_Impact");
    PlayStanceEffect("FireShockwave_Red");
    PlayStanceEffect("GroundCrater_Large");
    
    float damage = DEMON_GOD_STOMP_DAMAGE * m_powerModifier;
    damage *= m_stanceSystem->GetDamageModifier();
    
    // Creates expanding shockwave, hits multiple times
    // Excellent for pressure and space control
    m_stanceSystem->AddGauge(12.0f);
}

} // namespace ArenaFighter