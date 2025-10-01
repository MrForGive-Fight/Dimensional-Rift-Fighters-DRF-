#include "VersusMode.h"
#include <algorithm>

namespace ArenaFighter {

VersusMode::VersusMode(const VersusSettings& settings)
    : GameMode(MatchConfig()),
      m_settings(settings),
      m_doubleKO(false),
      m_suddenDeathTimer(0.0f) {
    
    // Configure match settings
    m_config.maxPlayers = 2;
    m_config.roundsToWin = settings.bestOf3 ? 2 : 3;
    m_config.roundTime = settings.infiniteTime ? 0.0f : 99.0f;
    m_config.infiniteTime = settings.infiniteTime;
    
    // Initialize ready states
    m_playerReady[0] = false;
    m_playerReady[1] = false;
}

void VersusMode::initialize() {
    GameMode::initialize();
    
    // Reset statistics
    resetStatistics();
    
    // Setup character slots based on mode
    setupCharacterSlots();
    
    // Initialize switch cooldowns
    for (const auto& player : m_players) {
        m_switchCooldowns[player->getId()] = 0.0f;
    }
    
    // Set up versus-specific UI elements
    if (m_gameUI) {
        // Additional UI setup for versus mode
        m_gameUI->setCharacterSlotMode(m_settings.slotMode);
    }
}

void VersusMode::update(float deltaTime) {
    // Check if we're waiting for players to be ready
    if (m_currentState == MatchState::PreMatch) {
        if (areAllPlayersReady()) {
            showVersusIntro();
            startMatch();
        }
        return;
    }
    
    // Update base game mode
    GameMode::update(deltaTime);
    
    // Versus-specific updates
    if (m_currentState == MatchState::InProgress) {
        // Update switch cooldowns
        updateSwitchCooldowns(deltaTime);
        
        // Check for double KO
        checkDoubleKO();
        
        // Handle sudden death if enabled
        if (m_suddenDeathTimer > 0) {
            handleSuddenDeath(deltaTime);
        }
        
        // Update statistics
        updatePlayerStatistics();
    }
}

void VersusMode::enterState(MatchState newState) {
    GameMode::enterState(newState);
    
    switch (newState) {
        case MatchState::PreMatch:
            // Reset ready states
            m_playerReady[0] = false;
            m_playerReady[1] = false;
            break;
            
        case MatchState::RoundStart:
            // Reset round-specific variables
            m_doubleKO = false;
            m_suddenDeathTimer = 0.0f;
            break;
            
        case MatchState::RoundEnd:
            // Update statistics based on round result
            {
                RoundResult result = getCurrentRoundResult();
                if (result.winnerId >= 0 && result.winnerId < 2) {
                    PlayerStats& stats = m_playerStats[result.winnerId];
                    
                    // Check for perfect round
                    if (result.remainingHealth >= BASE_HEALTH * 0.99f) {
                        stats.perfectRounds++;
                    }
                    
                    // Update average round time
                    int rounds = getWinsForPlayer(result.winnerId);
                    stats.avgRoundTime = (stats.avgRoundTime * (rounds - 1) + result.timeTaken) / rounds;
                }
            }
            break;
            
        case MatchState::MatchEnd:
            // Show final victory screen
            {
                int winner = -1;
                for (int i = 0; i < 2; ++i) {
                    if (getWinsForPlayer(i) >= m_config.roundsToWin) {
                        winner = i;
                        break;
                    }
                }
                showVictoryScreen(winner);
            }
            break;
    }
}

void VersusMode::checkDoubleKO() {
    // Check if both players are knocked out simultaneously
    if (m_players.size() >= 2) {
        bool p1KO = m_players[0]->getHealth() <= 0;
        bool p2KO = m_players[1]->getHealth() <= 0;
        
        if (p1KO && p2KO && !m_doubleKO) {
            m_doubleKO = true;
            
            // Handle double KO - sudden death or draw
            if (m_currentRound < 5) { // Prevent infinite rounds
                enableSuddenDeath();
            } else {
                // Draw - no winner
                endRound(-1, WinCondition::Knockout);
            }
        }
    }
}

void VersusMode::handleSuddenDeath(float deltaTime) {
    m_suddenDeathTimer += deltaTime;
    
    // In sudden death, increase damage over time
    if (m_suddenDeathTimer > 5.0f) {
        float damageIncrease = 1.0f + (m_suddenDeathTimer - 5.0f) * 0.1f;
        m_settings.damageMultiplier = std::min(damageIncrease, 3.0f);
    }
    
    // First hit wins in sudden death
    for (int i = 0; i < m_players.size(); ++i) {
        if (m_players[i]->getHealth() < BASE_HEALTH) {
            // Other player wins
            int winner = (i == 0) ? 1 : 0;
            endRound(winner, WinCondition::Knockout);
            m_suddenDeathTimer = 0.0f;
            m_settings.damageMultiplier = 1.0f;
            break;
        }
    }
}

void VersusMode::updatePlayerStatistics() {
    // Track various statistics during gameplay
    for (int i = 0; i < m_players.size(); ++i) {
        PlayerStats& stats = m_playerStats[i];
        
        // Update max combo
        int currentCombo = m_combatSystem->getCurrentCombo(i);
        if (currentCombo > stats.maxComboLength) {
            stats.maxComboLength = currentCombo;
        }
        
        // Track first hit (would need combat system support)
        // This is a placeholder for the concept
    }
}

void VersusMode::showVersusIntro() {
    // Display versus intro animation
    // This would typically trigger UI animations showing:
    // - Character portraits
    // - "VS" graphic
    // - Stage preview
    // For now, just a placeholder
}

void VersusMode::showVictoryScreen(int winner) {
    // Display victory screen with:
    // - Winner announcement
    // - Match statistics
    // - Rematch option
    // Placeholder for UI integration
}

void VersusMode::setPlayerReady(int playerId, bool ready) {
    if (playerId >= 0 && playerId < 2) {
        m_playerReady[playerId] = ready;
    }
}

bool VersusMode::isPlayerReady(int playerId) const {
    if (playerId >= 0 && playerId < 2) {
        return m_playerReady[playerId];
    }
    return false;
}

bool VersusMode::areAllPlayersReady() const {
    return m_playerReady[0] && m_playerReady[1] && m_players.size() >= 2;
}

VersusMode::PlayerStats VersusMode::getPlayerStats(int playerId) const {
    if (playerId >= 0 && playerId < 2) {
        return m_playerStats[playerId];
    }
    return PlayerStats();
}

void VersusMode::resetStatistics() {
    for (auto& stats : m_playerStats) {
        stats = PlayerStats();
    }
}

void VersusMode::enableSuddenDeath() {
    m_suddenDeathTimer = 0.01f; // Start sudden death
    
    // Reset both players to full health
    for (auto& player : m_players) {
        player->setHealth(BASE_HEALTH);
        player->setMana(BASE_MANA);
    }
    
    // Announce sudden death (UI notification)
    // Placeholder for UI integration
}

void VersusMode::setupCharacterSlots() {
    // Clear existing character data
    m_playerCharacters.clear();
    m_activeCharacterIndex.clear();
    
    // Initialize based on slot mode
    int requiredCount = getRequiredCharacterCount();
    
    // This will be populated when players select characters
    // For now, just set up the structure
    for (const auto& player : m_players) {
        m_activeCharacterIndex[player->getId()] = 0;
    }
}

void VersusMode::setPlayerCharacters(int playerId, const std::vector<std::shared_ptr<CharacterBase>>& characters) {
    int requiredCount = getRequiredCharacterCount();
    
    if (characters.size() != requiredCount) {
        // Log error: incorrect character count
        return;
    }
    
    m_playerCharacters[playerId] = characters;
    m_activeCharacterIndex[playerId] = 0;
    
    // Set the first character as active
    if (!characters.empty()) {
        // In single character mode, just use the character directly
        if (m_settings.slotMode == CharacterSlotMode::Single) {
            // Update the player reference
            for (int i = 0; i < m_players.size(); ++i) {
                if (m_players[i]->getId() == playerId) {
                    m_players[i] = characters[0];
                    break;
                }
            }
        } else {
            // In triple mode, activate the first character
            characters[0]->setActive(true);
            for (int i = 1; i < characters.size(); ++i) {
                characters[i]->setActive(false);
            }
        }
    }
}

std::vector<std::shared_ptr<CharacterBase>> VersusMode::getPlayerCharacters(int playerId) const {
    auto it = m_playerCharacters.find(playerId);
    if (it != m_playerCharacters.end()) {
        return it->second;
    }
    return {};
}

std::shared_ptr<CharacterBase> VersusMode::getActiveCharacter(int playerId) const {
    auto charIt = m_playerCharacters.find(playerId);
    auto indexIt = m_activeCharacterIndex.find(playerId);
    
    if (charIt != m_playerCharacters.end() && indexIt != m_activeCharacterIndex.end()) {
        int index = indexIt->second;
        if (index >= 0 && index < charIt->second.size()) {
            return charIt->second[index];
        }
    }
    
    return nullptr;
}

int VersusMode::getActiveCharacterIndex(int playerId) const {
    auto it = m_activeCharacterIndex.find(playerId);
    if (it != m_activeCharacterIndex.end()) {
        return it->second;
    }
    return -1;
}

void VersusMode::requestCharacterSwitch(int playerId, int direction) {
    // Only allow switching in Triple mode with switching enabled
    if (m_settings.slotMode != CharacterSlotMode::Triple || !m_settings.allowCharacterSwitch) {
        return;
    }
    
    // Check if can switch (cooldown, not in hitstun, etc.)
    if (!canSwitchCharacter(playerId)) {
        return;
    }
    
    switchCharacter(playerId, direction);
}

void VersusMode::switchCharacter(int playerId, int direction) {
    auto charIt = m_playerCharacters.find(playerId);
    auto indexIt = m_activeCharacterIndex.find(playerId);
    
    if (charIt == m_playerCharacters.end() || indexIt == m_activeCharacterIndex.end()) {
        return;
    }
    
    const auto& characters = charIt->second;
    if (characters.size() <= 1) return;
    
    int currentIndex = indexIt->second;
    int newIndex = currentIndex;
    
    // Calculate new index based on direction
    if (direction > 0) {
        newIndex = (currentIndex + 1) % characters.size();
    } else {
        newIndex = (currentIndex - 1 + characters.size()) % characters.size();
    }
    
    // Skip dead characters
    int attempts = 0;
    while (characters[newIndex]->isDead() && attempts < characters.size()) {
        if (direction > 0) {
            newIndex = (newIndex + 1) % characters.size();
        } else {
            newIndex = (newIndex - 1 + characters.size()) % characters.size();
        }
        attempts++;
    }
    
    // If all other characters are dead, can't switch
    if (characters[newIndex]->isDead()) {
        return;
    }
    
    // Perform the switch
    if (newIndex != currentIndex) {
        // Deactivate current character
        characters[currentIndex]->setActive(false);
        
        // Get current position to maintain continuity
        Vector3 currentPos = characters[currentIndex]->getPosition();
        
        // Activate new character
        characters[newIndex]->setActive(true);
        characters[newIndex]->setPosition(currentPos);
        
        // Update active index
        m_activeCharacterIndex[playerId] = newIndex;
        
        // Set switch cooldown
        m_switchCooldowns[playerId] = m_settings.switchCooldown;
        
        // Update player reference for game mode
        for (int i = 0; i < m_players.size(); ++i) {
            if (m_players[i]->getId() == playerId) {
                m_players[i] = characters[newIndex];
                break;
            }
        }
        
        // Notify UI
        if (m_gameUI) {
            m_gameUI->onCharacterSwitch(playerId, currentIndex, newIndex);
        }
    }
}

bool VersusMode::canSwitchCharacter(int playerId) const {
    // Check cooldown
    auto cooldownIt = m_switchCooldowns.find(playerId);
    if (cooldownIt != m_switchCooldowns.end() && cooldownIt->second > 0) {
        return false;
    }
    
    // Check if character is in a state that allows switching
    auto character = getActiveCharacter(playerId);
    if (character) {
        // Can't switch while in hitstun, during attacks, etc.
        if (character->isInHitstun() || character->isAttacking()) {
            return false;
        }
    }
    
    return true;
}

void VersusMode::updateSwitchCooldowns(float deltaTime) {
    for (auto& [playerId, cooldown] : m_switchCooldowns) {
        if (cooldown > 0) {
            cooldown -= deltaTime;
            if (cooldown < 0) {
                cooldown = 0;
            }
        }
    }
}

void VersusMode::setWeeklySlotMode(CharacterSlotMode mode) {
    m_settings.slotMode = mode;
    
    // If game is in progress, this won't take effect until next match
    if (m_currentState == MatchState::PreMatch) {
        setupCharacterSlots();
    }
}

int VersusMode::getRequiredCharacterCount() const {
    switch (m_settings.slotMode) {
        case CharacterSlotMode::Single:
            return 1;
        case CharacterSlotMode::Triple:
            return 3;
        default:
            return 1;
    }
}

} // namespace ArenaFighter