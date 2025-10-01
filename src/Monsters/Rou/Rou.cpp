#include "Rou.h"
#include <cmath>
#include <algorithm>

namespace ArenaFighter {

Rou::Rou() 
    : CharacterBase("Rou", CharacterCategory::Monsters),
      m_currentForm(RouEvolutionForm::GOBLIN),
      m_evolutionGauge(0.0f),
      m_emergencyProtocolUsed(false),
      m_emergencyProtocolTimer(0.0f),
      m_currentHP(FORM_STATS[0].baseHP),
      m_maxHP(FORM_STATS[0].baseHP) {
    
    EnableEvolutionSystem();
    InitializeSpecialMoves();
    
    // Initialize gear skills based on form
    m_gearSkills[0] = {"Goblin Strike", "goblin_strike", 5.0f, 50.0f, 3.0f, 5, 2, 8};
    m_gearSkills[1] = {"Quick Bite", "quick_bite", 8.0f, 65.0f, 2.5f, 6, 3, 10};
    m_gearSkills[2] = {"Shadow Claw", "shadow_claw", 12.0f, 80.0f, 4.0f, 8, 3, 12};
    m_gearSkills[3] = {"Dark Pulse", "dark_pulse", 15.0f, 95.0f, 5.0f, 10, 4, 15};
    m_gearSkills[4] = {"Demon Rush", "demon_rush", 20.0f, 110.0f, 6.0f, 12, 5, 18};
    m_gearSkills[5] = {"Void Strike", "void_strike", 25.0f, 125.0f, 7.0f, 14, 6, 20};
    m_gearSkills[6] = {"Heaven's Fall", "heavens_fall", 30.0f, 140.0f, 8.0f, 16, 7, 22};
    m_gearSkills[7] = {"Apocalypse", "apocalypse", 40.0f, 160.0f, 10.0f, 20, 10, 30};
}

Rou::~Rou() = default;

void Rou::Update(float deltaTime) {
    CharacterBase::Update(deltaTime);
    
    // Update buffs
    UpdateBuffs(deltaTime);
    
    // Update cooldowns
    UpdateCooldowns(deltaTime);
    
    // Check for emergency protocol
    if (!m_emergencyProtocolUsed && GetHPPercent() <= 30.0f) {
        ApplyEmergencyProtocol();
    }
    
    // Update emergency protocol timer
    if (m_emergencyProtocolTimer > 0) {
        m_emergencyProtocolTimer -= deltaTime;
    }
}

void Rou::Render() {
    // Rendering handled by game engine
}

void Rou::UpdateEvolutionGauge(float amount) {
    float previousGauge = m_evolutionGauge;
    m_evolutionGauge = std::clamp(m_evolutionGauge + amount, 0.0f, 100.0f);
    
    // Check if we crossed a threshold
    if (m_evolutionGauge != previousGauge) {
        CheckEvolution();
    }
}

void Rou::CheckEvolution() {
    RouEvolutionForm targetForm = RouEvolutionForm::GOBLIN;
    
    if (m_evolutionGauge >= 100.0f) {
        targetForm = RouEvolutionForm::VAJRAYAKSA;
    } else if (m_evolutionGauge >= 75.0f) {
        targetForm = RouEvolutionForm::APOSTLE_LORD;
    } else if (m_evolutionGauge >= 50.0f) {
        targetForm = RouEvolutionForm::OGRE;
    } else if (m_evolutionGauge >= 25.0f) {
        targetForm = RouEvolutionForm::HOBGOBLIN;
    }
    
    if (targetForm > m_currentForm) {
        m_currentForm = targetForm;
        Evolve();
    }
}

void Rou::Evolve() {
    PlayEvolutionVFX();
    ApplyFormChanges();
    
    // Update HP based on new form
    const FormStats& stats = GetCurrentFormStats();
    float hpRatio = m_currentHP / m_maxHP;
    m_maxHP = stats.baseHP;
    m_currentHP = m_maxHP * hpRatio;
}

void Rou::ApplyEmergencyProtocol() {
    if (m_emergencyProtocolUsed || GetHPPercent() > EMERGENCY_PROTOCOL_HP_THRESHOLD * 100.0f) {
        return;
    }
    
    // Auto-evolve to next form
    if (m_currentForm < RouEvolutionForm::VAJRAYAKSA) {
        m_currentForm = static_cast<RouEvolutionForm>(static_cast<int>(m_currentForm) + 1);
        
        // Apply evolution changes
        Evolve();
        
        // Heal 15% HP
        Heal(m_maxHP * EMERGENCY_PROTOCOL_HEAL);
        
        // Apply damage reduction for 3 seconds
        AddBuff(BuffInfo::DAMAGE_REDUCTION, 0.3f, EMERGENCY_PROTOCOL_DR_DURATION);
        
        m_emergencyProtocolUsed = true;
        
        // Special case for Vajrayaksa
        if (m_currentForm == RouEvolutionForm::VAJRAYAKSA) {
            Heal(m_maxHP); // Full heal
            m_evolutionGauge = 100.0f; // Lock at 100%
        }
    }
}

void Rou::ExecuteBasicCombo() {
    switch (m_currentForm) {
        case RouEvolutionForm::GOBLIN:
            GoblinCombo();
            break;
        case RouEvolutionForm::HOBGOBLIN:
            HobgoblinCombo();
            break;
        case RouEvolutionForm::OGRE:
            OgreCombo();
            break;
        case RouEvolutionForm::APOSTLE_LORD:
            ApostleLordCombo();
            break;
        case RouEvolutionForm::VAJRAYAKSA:
            VajrayaksaCombo();
            break;
    }
}

void Rou::ExecuteSpecialMove(int skillId) {
    if (skillId >= 0 && skillId < 8) {
        // Execute gear skill
        const GearSkill& skill = m_gearSkills[skillId];
        // Skill execution handled by combat system
    }
}

void Rou::ExecuteDirectionalSpecial(Direction dir) {
    int moveIndex = static_cast<int>(m_currentForm) * 3;
    
    switch (dir) {
        case Direction::UP:
            moveIndex += 0;
            break;
        case Direction::LEFT:
            moveIndex += 1;
            break;
        case Direction::RIGHT:
            moveIndex += 2;
            break;
        case Direction::DOWN:
            return; // No down specials
    }
    
    if (moveIndex < m_specialMoves.size() && m_specialMoves[moveIndex].currentCooldown <= 0) {
        m_specialMoves[moveIndex].execute();
        m_specialMoves[moveIndex].currentCooldown = m_specialMoves[moveIndex].cooldown;
    }
}

void Rou::GoblinCombo() {
    const FormStats& stats = GetCurrentFormStats();
    float baseDamage = 7.0f * stats.damageMultiplier;
    
    // 3-hit combo: DDD
    for (int i = 0; i < 3; i++) {
        float damage = baseDamage * std::pow(COMBO_SCALING, i);
        // Apply damage through combat system
    }
}

void Rou::HobgoblinCombo() {
    const FormStats& stats = GetCurrentFormStats();
    float baseDamage = 9.0f * stats.damageMultiplier;
    
    // 4-hit combo: DDDD
    for (int i = 0; i < 4; i++) {
        float damage = baseDamage * std::pow(COMBO_SCALING, i);
        // Apply damage through combat system
    }
}

void Rou::OgreCombo() {
    const FormStats& stats = GetCurrentFormStats();
    float baseDamage = 13.0f * stats.damageMultiplier;
    
    // 5-hit combo: DDDDD
    for (int i = 0; i < 5; i++) {
        float damage = baseDamage * std::pow(COMBO_SCALING, i);
        // Apply damage through combat system
    }
}

void Rou::ApostleLordCombo() {
    const FormStats& stats = GetCurrentFormStats();
    float baseDamage = 15.0f * stats.damageMultiplier;
    
    // 6-hit combo: DDDDDD
    for (int i = 0; i < 6; i++) {
        float damage = baseDamage * std::pow(COMBO_SCALING, i);
        // Apply damage through combat system
    }
}

void Rou::VajrayaksaCombo() {
    const FormStats& stats = GetCurrentFormStats();
    float baseDamage = 18.0f * stats.damageMultiplier;
    
    // 8-hit combo: DDDDDDDD
    for (int i = 0; i < 8; i++) {
        float damage = baseDamage * std::pow(COMBO_SCALING, i);
        // Apply damage through combat system
    }
}

void Rou::OnHit(float damage) {
    UpdateEvolutionGauge(EVOLUTION_GAUGE_ON_HIT);
}

void Rou::OnTakeDamage(float damage) {
    float gaugeGain = (damage / 10.0f) * EVOLUTION_GAUGE_PER_10HP_LOST;
    UpdateEvolutionGauge(gaugeGain);
}

void Rou::OnKill() {
    UpdateEvolutionGauge(EVOLUTION_GAUGE_ON_KILL);
}

void Rou::OnEquipmentPickup() {
    UpdateEvolutionGauge(EVOLUTION_GAUGE_ON_PICKUP);
}

void Rou::OnDeath() {
    // Apply 35% penalty to current meter
    UpdateEvolutionGauge(m_evolutionGauge * EVOLUTION_GAUGE_DEATH_PENALTY);
}

void Rou::AddBuff(BuffInfo::Type type, float value, float duration) {
    BuffInfo buff;
    buff.type = type;
    buff.value = value;
    buff.duration = duration;
    m_activeBuffs.push_back(buff);
}

float Rou::GetDamageReduction() const {
    float reduction = 0.0f;
    for (const auto& buff : m_activeBuffs) {
        if (buff.type == BuffInfo::DAMAGE_REDUCTION) {
            reduction += buff.value;
        }
    }
    return std::min(reduction, 0.8f); // Cap at 80% reduction
}

void Rou::InitializeSpecialMoves() {
    m_specialMoves.reserve(15);
    
    // Goblin moves
    m_specialMoves.push_back({"Panic Jump", 3.0f, 0.0f, [this]() { PanicJump(); }});
    m_specialMoves.push_back({"Survival Bite", 5.0f, 0.0f, [this]() { SurvivalBite(); }});
    m_specialMoves.push_back({"Goblin Rush", 4.0f, 0.0f, [this]() { GoblinRush(); }});
    
    // Hobgoblin moves
    m_specialMoves.push_back({"Shadow Upper", 4.0f, 0.0f, [this]() { ShadowUpper(); }});
    m_specialMoves.push_back({"Dark Counter", 6.0f, 0.0f, [this]() { DarkCounter(); }});
    m_specialMoves.push_back({"Phantom Strike", 7.0f, 0.0f, [this]() { PhantomStrike(); }});
    
    // Ogre moves
    m_specialMoves.push_back({"Ogre Slam", 7.0f, 0.0f, [this]() { OgreSlam(); }});
    m_specialMoves.push_back({"Ground Quake", 8.0f, 0.0f, [this]() { GroundQuake(); }});
    m_specialMoves.push_back({"Brutal Charge", 9.0f, 0.0f, [this]() { BrutalCharge(); }});
    
    // Apostle Lord moves
    m_specialMoves.push_back({"Demon Ascension", 9.0f, 0.0f, [this]() { DemonAscension(); }});
    m_specialMoves.push_back({"Lord's Territory", 11.0f, 0.0f, [this]() { LordsTerritory(); }});
    m_specialMoves.push_back({"Orb Barrage", 8.0f, 0.0f, [this]() { OrbBarrage(); }});
    
    // Vajrayaksa moves
    m_specialMoves.push_back({"Heaven Splitter", 12.0f, 0.0f, [this]() { HeavenSplitter(); }});
    m_specialMoves.push_back({"Overlord's Decree", 15.0f, 0.0f, [this]() { OverlordsDecree(); }});
    m_specialMoves.push_back({"Thousand Arms Rush", 13.0f, 0.0f, [this]() { ThousandArmsRush(); }});
}

// Special Move Implementations

void Rou::PanicJump() {
    // Quick vertical escape with invincibility frames
    // Implementation handled by physics/combat system
}

void Rou::SurvivalBite() {
    // Counter stance that heals 30 HP on successful counter
    Heal(30.0f);
}

void Rou::GoblinRush() {
    // Fast roll through enemies, steals 5% meter from hit enemies
    // Implementation handled by physics/combat system
}

void Rou::ShadowUpper() {
    // Rising uppercut that starts air combos
    // Implementation handled by combat system
}

void Rou::DarkCounter() {
    // Counter stance with shadow explosion
    // Implementation handled by combat system
}

void Rou::PhantomStrike() {
    // Teleport behind enemy for backstab
    // Implementation handled by physics/combat system
}

void Rou::OgreSlam() {
    // Jump slam with guard-breaking shockwave
    // Implementation handled by physics/combat system
}

void Rou::GroundQuake() {
    // Stomp creating earth spikes
    // Implementation handled by combat system
}

void Rou::BrutalCharge() {
    // Armored rush that grabs first enemy
    // Implementation handled by physics/combat system
}

void Rou::DemonAscension() {
    // Fly up and rain demon orbs
    // Implementation handled by combat system
}

void Rou::LordsTerritory() {
    // Create buff field for allies
    // Implementation handled by combat system
}

void Rou::OrbBarrage() {
    // Fire homing demon orbs
    // Implementation handled by combat system
}

void Rou::HeavenSplitter() {
    // All 4 arms create energy pillar
    // Implementation handled by combat system
}

void Rou::OverlordsDecree() {
    // AOE fear and reset ally cooldowns
    // Implementation handled by combat system
}

void Rou::ThousandArmsRush() {
    // Teleport barrage with 20 hits
    // Implementation handled by combat system
}

// Helper functions

void Rou::UpdateBuffs(float deltaTime) {
    for (auto it = m_activeBuffs.begin(); it != m_activeBuffs.end();) {
        it->duration -= deltaTime;
        if (it->duration <= 0) {
            it = m_activeBuffs.erase(it);
        } else {
            ++it;
        }
    }
}

void Rou::UpdateCooldowns(float deltaTime) {
    for (auto& move : m_specialMoves) {
        if (move.currentCooldown > 0) {
            move.currentCooldown -= deltaTime;
        }
    }
}

Rou::FormStats Rou::GetCurrentFormStats() const {
    return FORM_STATS[static_cast<int>(m_currentForm)];
}

void Rou::PlayEvolutionVFX() {
    // VFX handled by visual system
}

void Rou::ApplyFormChanges() {
    // Update character properties based on new form
    const FormStats& stats = GetCurrentFormStats();
    
    // Update visual scale
    // Update movement speed
    // Update defense values
    // All handled by respective game systems
}

} // namespace ArenaFighter