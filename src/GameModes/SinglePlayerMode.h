#pragma once

#include "GameMode.h"
#include <memory>

namespace ArenaFighter {

// AI difficulty levels
enum class AIDifficulty {
    Easy,       // 30% reaction time, basic combos
    Normal,     // 60% reaction time, intermediate combos
    Hard,       // 90% reaction time, advanced combos
    Extreme     // Frame-perfect reactions, optimal combos
};

// AI behavior patterns
enum class AIBehavior {
    Aggressive,     // Constant pressure
    Defensive,      // Counter-focused
    Balanced,       // Mix of offense and defense
    Random          // Unpredictable
};

// AI state for decision making
struct AIState {
    float reactionTime;         // Time before AI responds
    float decisionCooldown;     // Time between decisions
    float aggressiveness;       // 0.0 - 1.0 scale
    float defensiveness;        // 0.0 - 1.0 scale
    float comboAccuracy;        // Chance to complete combos
    float manaEfficiency;       // How well AI manages mana
    AIBehavior currentBehavior;
    float behaviorTimer;
};

class SinglePlayerMode : public GameMode {
private:
    AIDifficulty m_difficulty;
    AIState m_aiState;
    std::shared_ptr<CharacterBase> m_playerCharacter;
    std::shared_ptr<CharacterBase> m_aiCharacter;
    
    // AI decision making
    float m_nextDecisionTime;
    InputCommand m_currentAIInput;
    
    // AI behavior methods
    void initializeAI(AIDifficulty difficulty);
    void updateAI(float deltaTime);
    void makeAIDecision();
    InputCommand calculateAIInput();
    
    // AI tactics
    bool shouldAttack() const;
    bool shouldDefend() const;
    bool shouldUseSkill(int skillIndex) const;
    bool canStartCombo() const;
    InputCommand getAttackInput();
    InputCommand getDefenseInput();
    InputCommand getMovementInput();
    
    // AI analysis
    float getDistanceToPlayer() const;
    bool isPlayerAttacking() const;
    bool isPlayerVulnerable() const;
    float predictPlayerPosition(float time) const;

public:
    SinglePlayerMode(AIDifficulty difficulty = AIDifficulty::Normal);
    virtual ~SinglePlayerMode() = default;
    
    // Override base methods
    void initialize() override;
    void update(float deltaTime) override;
    
    // AI configuration
    void setDifficulty(AIDifficulty difficulty);
    AIDifficulty getDifficulty() const { return m_difficulty; }
    void setAIBehavior(AIBehavior behavior);
    
    // Player setup
    void setPlayerCharacter(std::shared_ptr<CharacterBase> character);
    void setAICharacter(std::shared_ptr<CharacterBase> character);
    
    // Mode specific implementations
    std::string getModeName() const override { return "Single Player"; }
    GameModeType getModeType() const override { return GameModeType::Ranked1v1; }
    bool supportsOnline() const override { return false; }
    int getMinPlayers() const override { return 2; } // Player + AI
    int getMaxPlayers() const override { return 2; }
};

} // namespace ArenaFighter