#include "EvolutionStateMachine.h"
#include <cmath>

namespace ArenaFighter {

EvolutionStateMachine::EvolutionStateMachine(Rou* owner) 
    : m_owner(owner),
      m_currentForm(RouEvolutionForm::GOBLIN),
      m_currentState(nullptr),
      m_timeInCurrentForm(0.0f),
      m_killsInCurrentForm(0),
      m_damageDealtInCurrentForm(0.0f),
      m_damageTakenInCurrentForm(0.0f) {
    
    InitializeStates();
    ChangeState(RouEvolutionForm::GOBLIN);
}

EvolutionStateMachine::~EvolutionStateMachine() = default;

void EvolutionStateMachine::InitializeStates() {
    m_goblinState = std::make_unique<GoblinFormState>();
    m_hobgoblinState = std::make_unique<HobgoblinFormState>();
    m_ogreState = std::make_unique<OgreFormState>();
    m_apostleLordState = std::make_unique<ApostleLordFormState>();
    m_vajrayaksaState = std::make_unique<VajrayaksaFormState>();
    
    m_stateMap[RouEvolutionForm::GOBLIN] = m_goblinState.get();
    m_stateMap[RouEvolutionForm::HOBGOBLIN] = m_hobgoblinState.get();
    m_stateMap[RouEvolutionForm::OGRE] = m_ogreState.get();
    m_stateMap[RouEvolutionForm::APOSTLE_LORD] = m_apostleLordState.get();
    m_stateMap[RouEvolutionForm::VAJRAYAKSA] = m_vajrayaksaState.get();
}

void EvolutionStateMachine::Update(float deltaTime) {
    if (m_currentState) {
        m_currentState->Update(m_owner, deltaTime);
        m_timeInCurrentForm += deltaTime;
    }
    
    // Check for automatic devolution conditions
    if (ShouldDevolve() && m_currentForm != RouEvolutionForm::GOBLIN) {
        RouEvolutionForm previousForm = static_cast<RouEvolutionForm>(static_cast<int>(m_currentForm) - 1);
        ChangeState(previousForm);
    }
}

void EvolutionStateMachine::ChangeState(RouEvolutionForm newForm) {
    if (newForm == m_currentForm) return;
    
    // Exit current state
    if (m_currentState) {
        RemoveEvolutionBonuses();
        m_currentState->Exit(m_owner);
    }
    
    // Change to new state
    m_currentForm = newForm;
    m_currentState = m_stateMap[newForm];
    
    // Enter new state
    if (m_currentState) {
        m_currentState->Enter(m_owner);
        ApplyEvolutionBonuses();
        ResetFormTracking();
    }
}

bool EvolutionStateMachine::CanEvolve(RouEvolutionForm toForm) const {
    // Check gauge requirements
    float gauge = m_owner->GetEvolutionGauge();
    
    switch (toForm) {
        case RouEvolutionForm::HOBGOBLIN:
            return gauge >= 25.0f;
        case RouEvolutionForm::OGRE:
            return gauge >= 50.0f;
        case RouEvolutionForm::APOSTLE_LORD:
            return gauge >= 75.0f;
        case RouEvolutionForm::VAJRAYAKSA:
            return gauge >= 100.0f;
        default:
            return false;
    }
}

bool EvolutionStateMachine::ShouldDevolve() const {
    // Only devolve if gauge drops below form threshold
    float gauge = m_owner->GetEvolutionGauge();
    
    switch (m_currentForm) {
        case RouEvolutionForm::VAJRAYAKSA:
            return gauge < 75.0f; // Drop to Apostle Lord
        case RouEvolutionForm::APOSTLE_LORD:
            return gauge < 50.0f; // Drop to Ogre
        case RouEvolutionForm::OGRE:
            return gauge < 25.0f; // Drop to Hobgoblin
        case RouEvolutionForm::HOBGOBLIN:
            return gauge < 10.0f; // Drop to Goblin if very low
        default:
            return false;
    }
}

float EvolutionStateMachine::GetDamageMultiplier() const {
    return m_currentState ? m_currentState->GetDamageMultiplier() : 1.0f;
}

float EvolutionStateMachine::GetSpeedMultiplier() const {
    return m_currentState ? m_currentState->GetSpeedMultiplier() : 1.0f;
}

float EvolutionStateMachine::GetDefenseMultiplier() const {
    return m_currentState ? m_currentState->GetDefenseMultiplier() : 1.0f;
}

int EvolutionStateMachine::GetMaxComboHits() const {
    return m_currentState ? m_currentState->GetMaxComboHits() : 3;
}

float EvolutionStateMachine::GetSizeScale() const {
    return m_currentState ? m_currentState->GetSizeScale() : 1.0f;
}

void EvolutionStateMachine::ApplyEvolutionBonuses() {
    // Apply stat changes based on current form
    // This would interface with the game's stat system
}

void EvolutionStateMachine::RemoveEvolutionBonuses() {
    // Remove previous form's bonuses
    // This would interface with the game's stat system
}

void EvolutionStateMachine::ForceEvolution(RouEvolutionForm targetForm) {
    if (targetForm > m_currentForm) {
        ChangeState(targetForm);
    }
}

void EvolutionStateMachine::ResetFormTracking() {
    m_timeInCurrentForm = 0.0f;
    m_killsInCurrentForm = 0;
    m_damageDealtInCurrentForm = 0.0f;
    m_damageTakenInCurrentForm = 0.0f;
}

bool EvolutionStateMachine::CheckEvolutionRequirements(RouEvolutionForm targetForm) const {
    // Additional requirements beyond gauge could be checked here
    return CanEvolve(targetForm);
}

// Form State Implementations

void GoblinFormState::Enter(Rou* rou) {
    // Apply goblin form visuals and properties
}

void GoblinFormState::Exit(Rou* rou) {
    // Clean up goblin form effects
}

void GoblinFormState::Update(Rou* rou, float deltaTime) {
    // Goblin-specific behavior
    // High mobility, evasive patterns
}

void HobgoblinFormState::Enter(Rou* rou) {
    // Apply hobgoblin form visuals and properties
}

void HobgoblinFormState::Exit(Rou* rou) {
    // Clean up hobgoblin form effects
}

void HobgoblinFormState::Update(Rou* rou, float deltaTime) {
    // Hobgoblin-specific behavior
    // Balanced combat approach
}

void OgreFormState::Enter(Rou* rou) {
    // Apply ogre form visuals and properties
}

void OgreFormState::Exit(Rou* rou) {
    // Clean up ogre form effects
}

void OgreFormState::Update(Rou* rou, float deltaTime) {
    // Ogre-specific behavior
    // Aggressive, high damage patterns
}

void ApostleLordFormState::Enter(Rou* rou) {
    // Apply apostle lord form visuals and properties
}

void ApostleLordFormState::Exit(Rou* rou) {
    // Clean up apostle lord form effects
}

void ApostleLordFormState::Update(Rou* rou, float deltaTime) {
    // Apostle Lord-specific behavior
    // Mixed ranged and melee patterns
}

void VajrayaksaFormState::Enter(Rou* rou) {
    // Apply vajrayaksa form visuals and properties
    m_auraIntensity = 1.0f;
    m_isFullPower = true;
}

void VajrayaksaFormState::Exit(Rou* rou) {
    // Clean up vajrayaksa form effects
}

void VajrayaksaFormState::Update(Rou* rou, float deltaTime) {
    // Vajrayaksa-specific behavior
    // Ultimate form with all abilities enhanced
    
    // Pulsing aura effect
    m_auraIntensity = 0.7f + 0.3f * sin(deltaTime * 2.0f);
    
    // Check if still at full power
    m_isFullPower = rou->GetEvolutionGauge() >= 100.0f;
}

} // namespace ArenaFighter