#pragma once

#include <memory>
#include <unordered_map>
#include "../Rou.h"
#include "FormStates.h"

namespace ArenaFighter {

class EvolutionStateMachine {
public:
    EvolutionStateMachine(Rou* owner);
    ~EvolutionStateMachine();
    
    // State management
    void Update(float deltaTime);
    void ChangeState(RouEvolutionForm newForm);
    
    // Get current state info
    FormState* GetCurrentState() const { return m_currentState; }
    RouEvolutionForm GetCurrentForm() const { return m_currentForm; }
    
    // Evolution conditions
    bool CanEvolve(RouEvolutionForm toForm) const;
    bool ShouldDevolve() const;
    
    // State queries
    float GetDamageMultiplier() const;
    float GetSpeedMultiplier() const;
    float GetDefenseMultiplier() const;
    int GetMaxComboHits() const;
    float GetSizeScale() const;
    
    // Evolution effects
    void ApplyEvolutionBonuses();
    void RemoveEvolutionBonuses();
    
    // Emergency evolution
    void ForceEvolution(RouEvolutionForm targetForm);
    
private:
    Rou* m_owner;
    RouEvolutionForm m_currentForm;
    FormState* m_currentState;
    
    // State instances
    std::unique_ptr<GoblinFormState> m_goblinState;
    std::unique_ptr<HobgoblinFormState> m_hobgoblinState;
    std::unique_ptr<OgreFormState> m_ogreState;
    std::unique_ptr<ApostleLordFormState> m_apostleLordState;
    std::unique_ptr<VajrayaksaFormState> m_vajrayaksaState;
    
    // State map for easy lookup
    std::unordered_map<RouEvolutionForm, FormState*> m_stateMap;
    
    // Evolution tracking
    float m_timeInCurrentForm;
    int m_killsInCurrentForm;
    float m_damageDealtInCurrentForm;
    float m_damageTakenInCurrentForm;
    
    // Helper functions
    void InitializeStates();
    void ResetFormTracking();
    bool CheckEvolutionRequirements(RouEvolutionForm targetForm) const;
};

} // namespace ArenaFighter