#include "SinglePlayerMode.h"
#include <random>
#include <algorithm>

namespace ArenaFighter {

// Random number generator for AI decisions
static std::mt19937 s_rng(std::random_device{}());
static std::uniform_real_distribution<float> s_dist(0.0f, 1.0f);

SinglePlayerMode::SinglePlayerMode(AIDifficulty difficulty)
    : GameMode(MatchConfig()),
      m_difficulty(difficulty),
      m_nextDecisionTime(0.0f) {
    
    // Configure for single player
    m_config.maxPlayers = 2;
    m_config.roundsToWin = 2;
    m_config.roundTime = 99.0f;
    
    initializeAI(difficulty);
}

void SinglePlayerMode::initialize() {
    GameMode::initialize();
    
    // AI starts with neutral input
    m_currentAIInput = InputCommand();
}

void SinglePlayerMode::initializeAI(AIDifficulty difficulty) {
    m_aiState = AIState();
    
    switch (difficulty) {
        case AIDifficulty::Easy:
            m_aiState.reactionTime = 0.5f;      // 500ms reaction
            m_aiState.decisionCooldown = 1.0f;  // 1s between decisions
            m_aiState.aggressiveness = 0.3f;
            m_aiState.defensiveness = 0.7f;
            m_aiState.comboAccuracy = 0.4f;
            m_aiState.manaEfficiency = 0.5f;
            break;
            
        case AIDifficulty::Normal:
            m_aiState.reactionTime = 0.3f;      // 300ms reaction
            m_aiState.decisionCooldown = 0.5f;  // 500ms between decisions
            m_aiState.aggressiveness = 0.5f;
            m_aiState.defensiveness = 0.5f;
            m_aiState.comboAccuracy = 0.7f;
            m_aiState.manaEfficiency = 0.7f;
            break;
            
        case AIDifficulty::Hard:
            m_aiState.reactionTime = 0.15f;     // 150ms reaction
            m_aiState.decisionCooldown = 0.25f; // 250ms between decisions
            m_aiState.aggressiveness = 0.7f;
            m_aiState.defensiveness = 0.6f;
            m_aiState.comboAccuracy = 0.85f;
            m_aiState.manaEfficiency = 0.85f;
            break;
            
        case AIDifficulty::Extreme:
            m_aiState.reactionTime = 0.05f;     // 50ms reaction (3 frames)
            m_aiState.decisionCooldown = 0.1f;  // 100ms between decisions
            m_aiState.aggressiveness = 0.9f;
            m_aiState.defensiveness = 0.8f;
            m_aiState.comboAccuracy = 0.95f;
            m_aiState.manaEfficiency = 0.95f;
            break;
    }
    
    // Start with balanced behavior
    m_aiState.currentBehavior = AIBehavior::Balanced;
    m_aiState.behaviorTimer = 0.0f;
}

void SinglePlayerMode::update(float deltaTime) {
    // Update base game mode
    GameMode::update(deltaTime);
    
    // Update AI if in active gameplay
    if (m_currentState == MatchState::InProgress) {
        updateAI(deltaTime);
    }
}

void SinglePlayerMode::updateAI(float deltaTime) {
    if (!m_aiCharacter || !m_playerCharacter) {
        return;
    }
    
    // Update behavior timer
    m_aiState.behaviorTimer += deltaTime;
    
    // Change behavior periodically
    if (m_aiState.behaviorTimer > 10.0f) {
        m_aiState.behaviorTimer = 0.0f;
        
        // Random behavior change
        float roll = s_dist(s_rng);
        if (roll < 0.3f) {
            m_aiState.currentBehavior = AIBehavior::Aggressive;
        } else if (roll < 0.6f) {
            m_aiState.currentBehavior = AIBehavior::Defensive;
        } else {
            m_aiState.currentBehavior = AIBehavior::Balanced;
        }
    }
    
    // Update decision timer
    m_nextDecisionTime -= deltaTime;
    
    // Make new decision if needed
    if (m_nextDecisionTime <= 0) {
        makeAIDecision();
        m_nextDecisionTime = m_aiState.decisionCooldown;
    }
    
    // Apply current AI input
    handleInput(1, m_currentAIInput); // AI is always player 2
}

void SinglePlayerMode::makeAIDecision() {
    // Calculate new input based on game state
    m_currentAIInput = calculateAIInput();
}

InputCommand SinglePlayerMode::calculateAIInput() {
    InputCommand input;
    
    // Get current state info
    float distance = getDistanceToPlayer();
    bool playerAttacking = isPlayerAttacking();
    bool playerVulnerable = isPlayerVulnerable();
    
    // Reaction delay simulation
    if (m_aiState.behaviorTimer < m_aiState.reactionTime) {
        return input; // Still reacting
    }
    
    // Decision making based on behavior
    switch (m_aiState.currentBehavior) {
        case AIBehavior::Aggressive:
            if (shouldAttack()) {
                input = getAttackInput();
            } else {
                input = getMovementInput();
            }
            break;
            
        case AIBehavior::Defensive:
            if (shouldDefend()) {
                input = getDefenseInput();
            } else if (playerVulnerable && shouldAttack()) {
                input = getAttackInput();
            } else {
                input = getMovementInput();
            }
            break;
            
        case AIBehavior::Balanced:
            if (playerAttacking && shouldDefend()) {
                input = getDefenseInput();
            } else if (shouldAttack()) {
                input = getAttackInput();
            } else {
                input = getMovementInput();
            }
            break;
            
        case AIBehavior::Random:
            // Completely random actions
            {
                float roll = s_dist(s_rng);
                if (roll < 0.4f) {
                    input = getAttackInput();
                } else if (roll < 0.6f) {
                    input = getDefenseInput();
                } else {
                    input = getMovementInput();
                }
            }
            break;
    }
    
    return input;
}

bool SinglePlayerMode::shouldAttack() const {
    float distance = getDistanceToPlayer();
    float mana = m_aiCharacter->getMana();
    
    // Check attack conditions
    bool inRange = distance < 150.0f; // Close combat range
    bool hasMana = mana >= 20.0f; // Minimum for special moves
    bool aggressive = s_dist(s_rng) < m_aiState.aggressiveness;
    
    return inRange && hasMana && aggressive;
}

bool SinglePlayerMode::shouldDefend() const {
    bool playerAttacking = isPlayerAttacking();
    bool defensive = s_dist(s_rng) < m_aiState.defensiveness;
    
    return playerAttacking && defensive;
}

bool SinglePlayerMode::shouldUseSkill(int skillIndex) const {
    // Check mana efficiency
    float manaCost = 20.0f + (skillIndex * 10.0f); // Example costs
    float currentMana = m_aiCharacter->getMana();
    
    if (currentMana < manaCost) {
        return false;
    }
    
    // Use skills based on mana efficiency setting
    return s_dist(s_rng) < m_aiState.manaEfficiency;
}

bool SinglePlayerMode::canStartCombo() const {
    // Check if AI should attempt a combo
    return s_dist(s_rng) < m_aiState.comboAccuracy;
}

InputCommand SinglePlayerMode::getAttackInput() {
    InputCommand input;
    
    if (canStartCombo()) {
        // Attempt combo sequence
        int comboType = static_cast<int>(s_dist(s_rng) * 3);
        
        switch (comboType) {
            case 0: // Light combo
                input.action = InputAction::LightAttack;
                input.direction = InputDirection::Neutral;
                break;
                
            case 1: // Medium combo
                input.action = InputAction::MediumAttack;
                input.direction = InputDirection::Forward;
                break;
                
            case 2: // Heavy combo
                input.action = InputAction::HeavyAttack;
                input.direction = InputDirection::Down;
                break;
        }
    } else {
        // Random attack
        float roll = s_dist(s_rng);
        if (roll < 0.5f) {
            input.action = InputAction::LightAttack;
        } else if (roll < 0.8f) {
            input.action = InputAction::MediumAttack;
        } else {
            input.action = InputAction::HeavyAttack;
        }
    }
    
    // Check for special move
    if (shouldUseSkill(1)) {
        input.action = InputAction::Special;
    }
    
    return input;
}

InputCommand SinglePlayerMode::getDefenseInput() {
    InputCommand input;
    
    // Block or evade
    float roll = s_dist(s_rng);
    if (roll < 0.7f) {
        input.action = InputAction::Block;
        input.direction = InputDirection::Back;
    } else {
        input.action = InputAction::Dash;
        input.direction = (roll < 0.85f) ? InputDirection::Back : InputDirection::Forward;
    }
    
    return input;
}

InputCommand SinglePlayerMode::getMovementInput() {
    InputCommand input;
    float distance = getDistanceToPlayer();
    
    // Movement decision
    if (distance > 200.0f) {
        // Move closer
        input.action = InputAction::Move;
        input.direction = (m_playerCharacter->getPosition().x < m_aiCharacter->getPosition().x) 
                          ? InputDirection::Back : InputDirection::Forward;
        
        // Dash if far
        if (distance > 400.0f && s_dist(s_rng) < 0.5f) {
            input.action = InputAction::Dash;
        }
    } else if (distance < 100.0f) {
        // Too close, create space
        input.action = InputAction::Move;
        input.direction = (m_playerCharacter->getPosition().x < m_aiCharacter->getPosition().x) 
                          ? InputDirection::Forward : InputDirection::Back;
    } else {
        // Optimal range, neutral or jump
        if (s_dist(s_rng) < 0.3f) {
            input.action = InputAction::Jump;
        }
    }
    
    return input;
}

float SinglePlayerMode::getDistanceToPlayer() const {
    if (!m_playerCharacter || !m_aiCharacter) {
        return 0.0f;
    }
    
    XMFLOAT3 playerPos = m_playerCharacter->getPosition();
    XMFLOAT3 aiPos = m_aiCharacter->getPosition();
    
    return std::abs(playerPos.x - aiPos.x);
}

bool SinglePlayerMode::isPlayerAttacking() const {
    if (!m_playerCharacter) {
        return false;
    }
    
    CharacterState state = m_playerCharacter->getCurrentState();
    return state == CharacterState::Attacking || 
           state == CharacterState::Special;
}

bool SinglePlayerMode::isPlayerVulnerable() const {
    if (!m_playerCharacter) {
        return false;
    }
    
    CharacterState state = m_playerCharacter->getCurrentState();
    return state == CharacterState::Recovery || 
           state == CharacterState::Stunned ||
           state == CharacterState::Falling;
}

float SinglePlayerMode::predictPlayerPosition(float time) const {
    if (!m_playerCharacter) {
        return 0.0f;
    }
    
    // Simple linear prediction
    XMFLOAT3 pos = m_playerCharacter->getPosition();
    XMFLOAT3 vel = m_playerCharacter->getVelocity();
    
    return pos.x + (vel.x * time);
}

void SinglePlayerMode::setDifficulty(AIDifficulty difficulty) {
    m_difficulty = difficulty;
    initializeAI(difficulty);
}

void SinglePlayerMode::setAIBehavior(AIBehavior behavior) {
    m_aiState.currentBehavior = behavior;
    m_aiState.behaviorTimer = 0.0f;
}

void SinglePlayerMode::setPlayerCharacter(std::shared_ptr<CharacterBase> character) {
    m_playerCharacter = character;
    if (m_players.empty()) {
        addPlayer(character);
    } else {
        m_players[0] = character;
    }
}

void SinglePlayerMode::setAICharacter(std::shared_ptr<CharacterBase> character) {
    m_aiCharacter = character;
    if (m_players.size() < 2) {
        addPlayer(character);
    } else {
        m_players[1] = character;
    }
    
    // Mark as AI controlled
    character->setAIControlled(true);
}

} // namespace ArenaFighter