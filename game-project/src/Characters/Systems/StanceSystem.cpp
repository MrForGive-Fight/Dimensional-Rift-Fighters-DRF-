#include "StanceSystem.h"

namespace ArenaFighter {

StanceSystem::StanceSystem() {
    // Add default neutral stance
    Stance neutral("Neutral", StanceType::Neutral, {});
    m_stances["Neutral"] = neutral;
}

void StanceSystem::AddStance(const std::string& name, const Stance& stance) {
    m_stances[name] = stance;
}

bool StanceSystem::SwitchStance(const std::string& stanceName) {
    if (m_stances.find(stanceName) == m_stances.end()) {
        return false;
    }
    
    if (IsTransitioning()) {
        return false;
    }
    
    m_nextStance = stanceName;
    m_transitionTimer = m_transitionTime;
    return true;
}

const Stance* StanceSystem::GetCurrentStance() const {
    auto it = m_stances.find(m_currentStanceName);
    if (it != m_stances.end()) {
        return &it->second;
    }
    return nullptr;
}

const StanceModifiers& StanceSystem::GetCurrentModifiers() const {
    static StanceModifiers defaultMods;
    const Stance* stance = GetCurrentStance();
    return stance ? stance->GetModifiers() : defaultMods;
}

void StanceSystem::Update(float deltaTime) {
    // Handle stance transitions
    if (m_transitionTimer > 0) {
        m_transitionTimer -= deltaTime;
        if (m_transitionTimer <= 0) {
            m_currentStanceName = m_nextStance;
            m_nextStance.clear();
        }
    }
    
    // Update stance stamina
    const Stance* currentStance = GetCurrentStance();
    if (currentStance) {
        const auto& mods = currentStance->GetModifiers();
        if (mods.drainsStamina) {
            m_stanceStamina -= mods.staminaDrainRate * deltaTime;
            if (m_stanceStamina <= 0) {
                // Force return to neutral stance
                SwitchStance("Neutral");
                m_stanceStamina = 0;
            }
        } else {
            // Regenerate stamina
            m_stanceStamina += 10.0f * deltaTime;
            if (m_stanceStamina > m_maxStanceStamina) {
                m_stanceStamina = m_maxStanceStamina;
            }
        }
    }
}

StanceSystem* StanceSystem::CreateFrostStanceSystem() {
    auto system = new StanceSystem();
    
    // Add frost-themed stances
    system->AddStance("Ice Wall", FrostStances::IceWall);
    system->AddStance("Frost Blade", FrostStances::FrostBlade);
    system->AddStance("Absolute Zero", FrostStances::AbsoluteZero);
    
    // Set up stance chain for combo
    system->SetStanceChain({"Ice Wall", "Frost Blade", "Absolute Zero"});
    system->EnableStanceCombo(true);
    
    return system;
}

} // namespace ArenaFighter