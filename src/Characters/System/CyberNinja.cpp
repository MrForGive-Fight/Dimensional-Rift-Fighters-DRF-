#include "CyberNinja.h"
#include "../../Combat/CombatSystem.h"

namespace ArenaFighter {

// Auto-register with factory
REGISTER_CHARACTER(103, "Cyber Ninja", CharacterCategory::System,
    "Stealth assassin with advanced cloaking and hacking abilities", CyberNinja)

CyberNinja::CyberNinja() 
    : CharacterBase("Cyber Ninja", CharacterCategory::System, StatMode::Hybrid)
    , m_isStealthed(false)
    , m_stealthDuration(0.0f)
    , m_hackProgress(0.0f)
    , m_hackTarget(nullptr)
    , m_digitalParticleTimer(0.0f)
    , m_cloakingActive(false) {
    
    // Cyber Ninja has balanced stats with slight speed advantage
    // Category and stat mode modifiers already applied in base constructor
    
    InitializeGearSkills();
}

void CyberNinja::Initialize() {
    CharacterBase::Initialize();
    
    // Reset cyber ninja specific state
    m_isStealthed = false;
    m_stealthDuration = 0.0f;
    m_hackProgress = 0.0f;
    m_hackTarget = nullptr;
}

void CyberNinja::InitializeGearSkills() {
    // Gear 1: Stealth Kit (Invisibility/Assassination)
    m_gearSkills[0] = {
        "Digital Cloak",          // name
        "cyber_cloak",           // animation
        25.0f,                   // mana cost
        0.0f,                    // no damage (utility)
        AttackType::Special,
        8,                       // startup frames
        1,                       // active frames
        12,                      // recovery frames
        false,                   // not projectile
        false,                   // can't combo
        ElementType::Void
    };
    
    m_gearSkills[1] = {
        "Shadow Strike",
        "shadow_strike",
        40.0f,                   // mana cost
        180.0f,                  // high damage from stealth
        AttackType::Heavy,
        5,                       // fast startup
        3,                       // active
        15,                      // recovery
        false,
        true,                    // can combo
        ElementType::Dark
    };
    
    // Gear 2: Hacking Tools (Disruption/Control)
    m_gearSkills[2] = {
        "System Breach",
        "system_breach",
        35.0f,                   // mana cost
        50.0f,                   // damage over time
        AttackType::Special,
        15,                      // longer startup
        20,                      // channel duration
        10,                      // recovery
        false,
        false,
        ElementType::Lightning
    };
    
    m_gearSkills[3] = {
        "EMP Pulse",
        "emp_pulse",
        30.0f,                   // mana cost
        120.0f,                  // area damage
        AttackType::Medium,
        10,                      // startup
        5,                       // active
        18,                      // recovery
        false,
        true,
        ElementType::Lightning
    };
    
    // Gear 3: Blade Dance (Melee Combat)
    m_gearSkills[4] = {
        "Quantum Dash",
        "quantum_dash",
        20.0f,                   // mana cost
        80.0f,                   // damage
        AttackType::Light,
        3,                       // very fast
        2,                       // active
        8,                       // recovery
        false,
        true,
        ElementType::Void
    };
    
    m_gearSkills[5] = {
        "Blade Cyclone",
        "blade_cyclone",
        45.0f,                   // mana cost
        150.0f,                  // spinning attack
        AttackType::Medium,
        12,                      // startup
        8,                       // active (multi-hit)
        20,                      // recovery
        false,
        true,
        ElementType::Wind
    };
    
    // Gear 4: Digital Arsenal (Projectiles/Traps)
    m_gearSkills[6] = {
        "Nano Shuriken",
        "nano_shuriken",
        15.0f,                   // low mana cost
        60.0f,                   // damage per shuriken
        AttackType::Light,
        6,                       // startup
        2,                       // active
        10,                      // recovery
        true,                    // projectile
        true,
        ElementType::Neutral
    };
    
    m_gearSkills[7] = {
        "Hologram Trap",
        "hologram_trap",
        50.0f,                   // high mana cost
        200.0f,                  // explosion damage
        AttackType::Special,
        18,                      // setup time
        1,                       // active
        25,                      // recovery
        false,
        false,
        ElementType::Light
    };
}

void CyberNinja::Update(float deltaTime) {
    CharacterBase::Update(deltaTime);
    
    // Update stealth
    if (m_isStealthed) {
        m_stealthDuration -= deltaTime;
        if (m_stealthDuration <= 0.0f) {
            ExitStealthMode();
        }
    }
    
    // Update hack progress
    if (m_hackTarget) {
        m_hackProgress += deltaTime * 0.25f; // 4 seconds to complete
        if (m_hackProgress >= 1.0f) {
            // Hack complete - apply debuff
            m_hackTarget = nullptr;
            m_hackProgress = 0.0f;
        }
    }
    
    // Update visual effects
    m_digitalParticleTimer += deltaTime;
}

void CyberNinja::OnGearSwitch(int oldGear, int newGear) {
    // Exit stealth when switching from stealth gear
    if (oldGear == 0 && m_isStealthed) {
        ExitStealthMode();
    }
    
    // Cancel hack when switching from hacking gear
    if (oldGear == 1 && m_hackTarget) {
        m_hackTarget = nullptr;
        m_hackProgress = 0.0f;
    }
}

void CyberNinja::OnSkillUse(int skillIndex) {
    // Handle special skill effects
    if (skillIndex == 0) { // Digital Cloak
        EnterStealthMode();
    }
    else if (skillIndex == 1 && m_isStealthed) { // Shadow Strike
        // Bonus damage from stealth applied in combat system
        ExitStealthMode();
    }
    else if (skillIndex == 2) { // System Breach
        // Handled by combat system target selection
    }
}

void CyberNinja::EnterStealthMode() {
    if (!m_isStealthed && CanAffordSkill(m_gearSkills[0].manaCost)) {
        m_isStealthed = true;
        m_stealthDuration = 5.0f; // 5 seconds of stealth
        m_cloakingActive = true;
        ConsumeMana(m_gearSkills[0].manaCost);
    }
}

void CyberNinja::ExitStealthMode() {
    m_isStealthed = false;
    m_stealthDuration = 0.0f;
    m_cloakingActive = false;
}

void CyberNinja::InitiateHack(CharacterBase* target) {
    if (target && !m_hackTarget && CanAffordSkill(m_gearSkills[2].manaCost)) {
        m_hackTarget = target;
        m_hackProgress = 0.0f;
        ConsumeMana(m_gearSkills[2].manaCost);
    }
}

} // namespace ArenaFighter