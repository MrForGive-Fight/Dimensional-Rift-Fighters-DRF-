#include "GameMode.h"
#include "../Physics/PhysicsConstants.h"
#include <algorithm>

namespace ArenaFighter {

GameMode::GameMode(const MatchConfig& config)
    : m_config(config),
      m_currentState(MatchState::PreMatch),
      m_roundTimer(0.0f),
      m_stateTimer(0.0f),
      m_currentRound(0) {
    
    // Initialize core systems
    m_combatSystem = std::make_shared<CombatSystem>();
    m_physicsEngine = std::make_shared<PhysicsEngine>();
    m_networkManager = std::make_shared<NetworkManager>();
}

void GameMode::initialize() {
    // Initialize physics
    m_physicsEngine->initialize();
    m_physicsEngine->setGravity(XMFLOAT3(0, GRAVITY, 0));
    
    // Initialize combat system
    m_combatSystem->initialize();
    
    // Create UI based on mode
    m_gameUI = std::make_shared<GameModeUI>("GameModeUI", getModeType());
    
    // Reserve space for players
    m_players.reserve(m_config.maxPlayers);
}

void GameMode::update(float deltaTime) {
    // Don't update if paused
    if (m_currentState == MatchState::Paused) {
        return;
    }
    
    // Update state timer
    m_stateTimer += deltaTime;
    
    // State-specific updates
    switch (m_currentState) {
        case MatchState::PreMatch:
            // Wait for all players to be ready
            if (m_players.size() >= getMinPlayers() && m_stateTimer > 1.0f) {
                startMatch();
            }
            break;
            
        case MatchState::RoundStart:
            // Show round intro for 3 seconds
            if (m_stateTimer > 3.0f) {
                setState(MatchState::InProgress);
            }
            break;
            
        case MatchState::InProgress:
            // Update round timer
            if (!m_config.infiniteTime) {
                m_roundTimer -= deltaTime;
                m_gameUI->setMatchTime(m_roundTimer);
                
                if (m_roundTimer <= 0) {
                    // Time out
                    int winner = calculateRoundWinner();
                    endRound(winner, WinCondition::TimeOut);
                    return;
                }
            }
            
            // Update physics
            m_physicsEngine->update(deltaTime);
            
            // Update combat
            m_combatSystem->update(deltaTime);
            
            // Update characters
            for (auto& player : m_players) {
                player->update(deltaTime);
                
                // Update mana regeneration
                float currentMana = player->getMana();
                player->setMana(std::min(currentMana + MANA_REGEN_RATE * deltaTime, BASE_MANA));
            }
            
            // Check win conditions
            if (checkWinConditions()) {
                return; // Win condition handled the state change
            }
            
            // Update UI
            updateUI();
            break;
            
        case MatchState::RoundEnd:
            // Show round end for 3 seconds
            if (m_stateTimer > 3.0f) {
                // Check if match is over
                int matchWinner = -1;
                for (int i = 0; i < m_players.size(); ++i) {
                    if (getWinsForPlayer(i) >= m_config.roundsToWin) {
                        matchWinner = i;
                        break;
                    }
                }
                
                if (matchWinner >= 0) {
                    endMatch();
                } else {
                    // Start next round
                    m_currentRound++;
                    startRound();
                }
            }
            break;
            
        case MatchState::MatchEnd:
            // Wait for player input or timeout
            if (m_stateTimer > 10.0f) {
                // Return to main menu or restart
                quitMatch();
            }
            break;
    }
}

void GameMode::render() {
    // Render game UI
    if (m_gameUI) {
        m_gameUI->render();
    }
    
    // Mode-specific rendering handled by derived classes
}

void GameMode::shutdown() {
    // Clean up systems
    if (m_combatSystem) {
        m_combatSystem->shutdown();
    }
    
    if (m_physicsEngine) {
        m_physicsEngine->shutdown();
    }
    
    // Clear players
    m_players.clear();
    m_roundResults.clear();
}

void GameMode::addPlayer(std::shared_ptr<CharacterBase> character) {
    if (m_players.size() < m_config.maxPlayers) {
        m_players.push_back(character);
        
        // Register with systems
        m_physicsEngine->registerCharacter(character.get());
        m_combatSystem->registerCharacter(character.get());
        
        // Set player index
        character->setPlayerIndex(static_cast<int>(m_players.size()) - 1);
    }
}

void GameMode::removePlayer(int playerId) {
    if (playerId >= 0 && playerId < m_players.size()) {
        // Unregister from systems
        m_physicsEngine->unregisterCharacter(m_players[playerId].get());
        m_combatSystem->unregisterCharacter(m_players[playerId].get());
        
        // Remove player
        m_players.erase(m_players.begin() + playerId);
        
        // Update remaining player indices
        for (int i = playerId; i < m_players.size(); ++i) {
            m_players[i]->setPlayerIndex(i);
        }
    }
}

std::shared_ptr<CharacterBase> GameMode::getPlayer(int index) const {
    if (index >= 0 && index < m_players.size()) {
        return m_players[index];
    }
    return nullptr;
}

void GameMode::setState(MatchState state) {
    if (m_currentState != state) {
        exitState(m_currentState);
        m_currentState = state;
        m_stateTimer = 0.0f;
        enterState(state);
    }
}

void GameMode::enterState(MatchState newState) {
    switch (newState) {
        case MatchState::PreMatch:
            // Reset everything
            m_currentRound = 0;
            m_roundResults.clear();
            break;
            
        case MatchState::RoundStart:
            resetPlayerPositions();
            resetPlayerStats();
            m_roundTimer = m_config.roundTime;
            break;
            
        case MatchState::InProgress:
            // Enable player input
            for (auto& player : m_players) {
                player->setInputEnabled(true);
            }
            break;
            
        case MatchState::RoundEnd:
            // Disable player input
            for (auto& player : m_players) {
                player->setInputEnabled(false);
            }
            break;
            
        case MatchState::Paused:
            if (m_onPause) {
                m_onPause();
            }
            break;
    }
}

void GameMode::exitState(MatchState oldState) {
    // State-specific cleanup
}

void GameMode::startRound() {
    setState(MatchState::RoundStart);
    m_gameUI->setRoundNumber(m_currentRound + 1);
}

void GameMode::endRound(int winnerId, WinCondition condition) {
    // Create round result
    RoundResult result;
    result.winnerId = winnerId;
    result.winType = condition;
    result.timeTaken = m_config.roundTime - m_roundTimer;
    
    if (winnerId >= 0 && winnerId < m_players.size()) {
        result.remainingHealth = m_players[winnerId]->getHealth();
        result.remainingMana = m_players[winnerId]->getMana();
        result.maxCombo = m_combatSystem->getMaxCombo(winnerId);
        result.damageDealt = m_combatSystem->getTotalDamage(winnerId);
    }
    
    m_roundResults.push_back(result);
    
    // Trigger callback
    if (m_onRoundEnd) {
        m_onRoundEnd(winnerId);
    }
    
    setState(MatchState::RoundEnd);
}

bool GameMode::checkWinConditions() {
    // Check for knockouts
    for (int i = 0; i < m_players.size(); ++i) {
        if (m_players[i]->getHealth() <= 0) {
            // Find the winner (other player in 1v1)
            int winner = (i == 0) ? 1 : 0;
            endRound(winner, WinCondition::Knockout);
            return true;
        }
    }
    
    return false;
}

int GameMode::calculateRoundWinner() {
    // Determine winner based on remaining health
    int winner = -1;
    float maxHealth = 0;
    
    for (int i = 0; i < m_players.size(); ++i) {
        if (m_players[i]->getHealth() > maxHealth) {
            maxHealth = m_players[i]->getHealth();
            winner = i;
        }
    }
    
    return winner;
}

void GameMode::spawnPlayers() {
    // Default spawn positions
    float spacing = 400.0f;
    float centerX = 0.0f;
    
    for (int i = 0; i < m_players.size(); ++i) {
        float xPos = centerX + (i - 0.5f * (m_players.size() - 1)) * spacing;
        m_players[i]->setPosition(XMFLOAT3(xPos, 0, 0));
        m_players[i]->setFacingRight(i == 0);
    }
}

void GameMode::resetPlayerPositions() {
    spawnPlayers();
}

void GameMode::resetPlayerStats() {
    for (auto& player : m_players) {
        player->setHealth(BASE_HEALTH);
        player->setMana(BASE_MANA);
        player->resetCombo();
    }
}

bool GameMode::isMatchActive() const {
    return m_currentState == MatchState::InProgress ||
           m_currentState == MatchState::RoundStart ||
           m_currentState == MatchState::RoundEnd;
}

void GameMode::handleInput(int playerId, const InputCommand& input) {
    if (m_currentState == MatchState::InProgress) {
        if (playerId >= 0 && playerId < m_players.size()) {
            m_players[playerId]->handleInput(input);
        }
    }
    
    // Handle pause
    if (input.action == InputAction::Pause && m_config.allowPause) {
        if (m_currentState == MatchState::InProgress) {
            pauseGame();
        } else if (m_currentState == MatchState::Paused) {
            resumeGame();
        }
    }
}

void GameMode::pauseGame() {
    if (m_currentState == MatchState::InProgress) {
        setState(MatchState::Paused);
    }
}

void GameMode::resumeGame() {
    if (m_currentState == MatchState::Paused) {
        setState(MatchState::InProgress);
    }
}

void GameMode::startMatch() {
    m_currentRound = 0;
    m_roundResults.clear();
    spawnPlayers();
    startRound();
}

void GameMode::endMatch() {
    // Find overall winner
    int winner = -1;
    int maxWins = 0;
    
    for (int i = 0; i < m_players.size(); ++i) {
        int wins = getWinsForPlayer(i);
        if (wins > maxWins) {
            maxWins = wins;
            winner = i;
        }
    }
    
    if (m_onMatchEnd) {
        m_onMatchEnd(winner);
    }
    
    setState(MatchState::MatchEnd);
}

void GameMode::restartMatch() {
    setState(MatchState::PreMatch);
    startMatch();
}

void GameMode::quitMatch() {
    shutdown();
}

RoundResult GameMode::getCurrentRoundResult() const {
    if (!m_roundResults.empty()) {
        return m_roundResults.back();
    }
    return RoundResult();
}

int GameMode::getWinsForPlayer(int playerId) const {
    int wins = 0;
    for (const auto& result : m_roundResults) {
        if (result.winnerId == playerId) {
            wins++;
        }
    }
    return wins;
}

void GameMode::updateUI() {
    // Update player health and mana
    for (int i = 0; i < m_players.size(); ++i) {
        m_gameUI->updatePlayerHealth(i, m_players[i]->getHealth());
        m_gameUI->updatePlayerMana(i, m_players[i]->getMana());
        
        // Update combo display
        int combo = m_combatSystem->getCurrentCombo(i);
        m_gameUI->updateCombo(i, combo);
    }
}

} // namespace ArenaFighter