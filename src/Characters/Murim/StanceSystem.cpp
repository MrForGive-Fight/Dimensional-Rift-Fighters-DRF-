#include "StanceSystem.h"
#include <algorithm>

namespace ArenaFighter {

StanceSystem::StanceSystem()
    : m_currentStance(StanceType::Light)
    , m_temperedGauge(0.0f)
    , m_switchCooldown(0.0f) {
}

void StanceSystem::SwitchStance() {
    if (m_switchCooldown > 0.0f) {
        return; // Cannot switch during cooldown
    }
    
    StanceType oldStance = m_currentStance;
    m_currentStance = (m_currentStance == StanceType::Light) ? StanceType::Dark : StanceType::Light;
    m_switchCooldown = SWITCH_COOLDOWN;
    
    // Trigger callback if set
    if (m_onStanceChange) {
        m_onStanceChange(oldStance, m_currentStance);
    }
}

std::string StanceSystem::GetStanceColor() const {
    switch (m_currentStance) {
        case StanceType::Light:
            return "Blue";
        case StanceType::Dark:
            return "Red";
        default:
            return "Neutral";
    }
}

std::string StanceSystem::GetStanceAura() const {
    switch (m_currentStance) {
        case StanceType::Light:
            return "BlueSpearAura";
        case StanceType::Dark:
            return "RedDemonAura";
        default:
            return "NeutralAura";
    }
}

float StanceSystem::GetDamageModifier() const {
    switch (m_currentStance) {
        case StanceType::Light:
            return 1.0f; // Balanced damage
        case StanceType::Dark:
            return 1.2f; // 20% more damage in Dark stance
        default:
            return 1.0f;
    }
}

float StanceSystem::GetSpeedModifier() const {
    switch (m_currentStance) {
        case StanceType::Light:
            return 1.15f; // 15% faster in Light stance
        case StanceType::Dark:
            return 0.95f; // 5% slower in Dark stance
        default:
            return 1.0f;
    }
}

float StanceSystem::GetDefenseModifier() const {
    switch (m_currentStance) {
        case StanceType::Light:
            return 1.1f; // 10% more defense in Light stance
        case StanceType::Dark:
            return 0.9f; // 10% less defense in Dark stance
        default:
            return 1.0f;
    }
}

void StanceSystem::AddGauge(float amount) {
    m_temperedGauge = std::min(m_temperedGauge + amount, MAX_GAUGE);
}

void StanceSystem::ConsumeGauge(float amount) {
    m_temperedGauge = std::max(m_temperedGauge - amount, 0.0f);
}

void StanceSystem::Update(float deltaTime) {
    // Update switch cooldown
    if (m_switchCooldown > 0.0f) {
        m_switchCooldown = std::max(0.0f, m_switchCooldown - deltaTime);
    }
    
    // Gauge slowly decays when not in combat
    if (m_temperedGauge > 0.0f) {
        m_temperedGauge = std::max(0.0f, m_temperedGauge - GAUGE_DECAY_RATE * deltaTime);
    }
}

} // namespace ArenaFighter