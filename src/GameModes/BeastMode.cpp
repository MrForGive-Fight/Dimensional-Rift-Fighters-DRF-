#include "BeastMode.h"
#include <algorithm>
#include <chrono>

namespace ArenaFighter {

BeastMode::BeastMode(const BeastModeConfig& config) 
    : GameMode(config)
    , m_beastConfig(config)
    , m_beastState(BeastModeState::SelectingBeast)
    , m_currentBeastId(-1)
    , m_totalBeastDamage(0.0f)
    , m_beastTimer(0.0f)
    , m_transformTimer(0.0f) {
    
    // Initialize RNG
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    m_rng.seed(seed);
    
    // Update base config for beast mode
    m_config.maxPlayers = 8;
    m_config.roundTime = config.beastModeDuration;
}

void BeastMode::initialize() {
    GameMode::initialize();
    
    // Initialize damage trackers for all players
    for (const auto& player : m_players) {
        m_damageTrackers[player->getId()] = DamageTracker{
            player->getId(), 0.0f, 0.0f, 0
        };
    }
}

void BeastMode::update(float deltaTime) {
    // Update base game mode
    GameMode::update(deltaTime);
    
    // Handle beast mode specific updates
    switch (m_beastState) {
        case BeastModeState::SelectingBeast:
            if (m_stateTimer > 3.0f) { // 3 second countdown
                selectBeast();
            }
            break;
            
        case BeastModeState::Transforming:
            m_transformTimer += deltaTime;
            updateBeastVisuals();
            
            if (m_transformTimer >= m_beastConfig.beastTransformTime) {
                setBeastState(BeastModeState::Fighting);
                setState(MatchState::InProgress);
            }
            break;
            
        case BeastModeState::Fighting:
            m_beastTimer += deltaTime;
            
            // Check if beast is AI controlled
            if (m_beastCharacter && !m_beastCharacter->isPlayerControlled()) {
                updateBeastAI(deltaTime);
            }
            
            // Update damage rankings
            updateDamageRankings();
            
            // Check win conditions
            if (checkWinConditions()) {
                int winner = calculateRoundWinner();
                endRound(winner, WinCondition::BeastDamage);
            }
            break;
            
        case BeastModeState::BeastDefeated:
        case BeastModeState::TimeExpired:
            // Handled by round end logic
            break;
    }
}

void BeastMode::render() {
    GameMode::render();
    
    // Additional beast mode rendering
    if (m_beastState == BeastModeState::Transforming) {
        // Render transformation effects
        playTransformationEffect();
    }
    
    // Update UI with damage rankings
    if (m_gameUI && m_beastState == BeastModeState::Fighting) {
        auto rankings = getSortedDamageRankings();
        m_gameUI->updateDamageRankings(rankings);
    }
}

void BeastMode::shutdown() {
    // Clean up beast mode specific resources
    revertFromBeast();
    m_damageTrackers.clear();
    
    GameMode::shutdown();
}

void BeastMode::setState(MatchState state) {
    GameMode::setState(state);
    
    if (state == MatchState::RoundStart) {
        setBeastState(BeastModeState::SelectingBeast);
    }
}

void BeastMode::setBeastState(BeastModeState state) {
    m_beastState = state;
    m_stateTimer = 0.0f;
    
    // Notify UI of state change
    if (m_gameUI) {
        m_gameUI->onBeastStateChanged(state);
    }
}

void BeastMode::addPlayer(std::shared_ptr<CharacterBase> character) {
    GameMode::addPlayer(character);
    
    // Initialize damage tracker for new player
    m_damageTrackers[character->getId()] = DamageTracker{
        character->getId(), 0.0f, 0.0f, 0
    };
}

void BeastMode::removePlayer(int playerId) {
    // If removing the beast, select a new one
    if (playerId == m_currentBeastId && m_beastState == BeastModeState::Fighting) {
        revertFromBeast();
        selectBeast();
    }
    
    // Remove damage tracker
    m_damageTrackers.erase(playerId);
    
    GameMode::removePlayer(playerId);
}

void BeastMode::handleInput(int playerId, const InputCommand& input) {
    // Handle beast-specific inputs
    if (isBeast(playerId) && m_beastState == BeastModeState::Fighting) {
        // Beast has modified input handling (e.g., different combos)
        // Pass to beast character with special flag
        if (m_beastCharacter) {
            m_beastCharacter->handleBeastInput(input);
            return;
        }
    }
    
    // Normal input handling for hunters
    GameMode::handleInput(playerId, input);
}

void BeastMode::selectBeast() {
    if (m_players.empty()) return;
    
    // Random selection for now, can be modified for different selection methods
    std::uniform_int_distribution<int> dist(0, m_players.size() - 1);
    int beastIndex = dist(m_rng);
    
    m_currentBeastId = m_players[beastIndex]->getId();
    transformToBeast(m_currentBeastId);
}

void BeastMode::transformToBeast(int playerId) {
    // Find the player
    auto it = std::find_if(m_players.begin(), m_players.end(),
        [playerId](const auto& p) { return p->getId() == playerId; });
    
    if (it == m_players.end()) return;
    
    m_beastCharacter = *it;
    
    // Separate beast from hunters
    m_hunters.clear();
    for (const auto& player : m_players) {
        if (player->getId() != playerId) {
            m_hunters.push_back(player);
        }
    }
    
    // Start transformation
    setBeastState(BeastModeState::Transforming);
    m_transformTimer = 0.0f;
    
    // Apply beast modifiers after transformation completes
}

void BeastMode::revertFromBeast() {
    if (!m_beastCharacter) return;
    
    removeBeastModifiers();
    
    // Reset beast reference
    m_beastCharacter = nullptr;
    m_currentBeastId = -1;
    m_hunters.clear();
}

void BeastMode::applyBeastModifiers() {
    if (!m_beastCharacter) return;
    
    // Apply stat multipliers
    auto stats = m_beastCharacter->getStats();
    stats.maxHealth *= m_beastConfig.beastHealthMultiplier;
    stats.currentHealth = stats.maxHealth; // Full heal on transform
    stats.maxMana *= m_beastConfig.beastManaMultiplier;
    stats.currentMana = stats.maxMana;
    stats.baseDamage *= m_beastConfig.beastDamageMultiplier;
    
    m_beastCharacter->setStats(stats);
    
    // Apply size multiplier
    m_beastCharacter->setScale(m_beastConfig.beastSizeMultiplier);
    
    // Enable beast abilities
    m_beastCharacter->enableBeastMode();
}

void BeastMode::removeBeastModifiers() {
    if (!m_beastCharacter) return;
    
    // Reset stats to normal
    m_beastCharacter->resetStats();
    m_beastCharacter->setScale(1.0f);
    m_beastCharacter->disableBeastMode();
}

void BeastMode::trackDamage(int attackerId, float damage) {
    auto it = m_damageTrackers.find(attackerId);
    if (it != m_damageTrackers.end()) {
        it->second.totalDamage += damage;
        it->second.hitCount++;
        
        // Track combo damage if applicable
        if (auto attacker = getPlayer(attackerId)) {
            float combo = attacker->getCurrentCombo();
            if (combo > it->second.highestCombo) {
                it->second.highestCombo = combo;
            }
        }
    }
    
    m_totalBeastDamage += damage;
}

void BeastMode::updateDamageRankings() {
    // This is called every frame to track damage dealt to the beast
    if (!m_beastCharacter) return;
    
    // Get damage events from combat system
    auto damageEvents = m_combatSystem->getRecentDamageEvents();
    
    for (const auto& event : damageEvents) {
        if (event.targetId == m_currentBeastId) {
            trackDamage(event.attackerId, event.damage);
        }
    }
}

int BeastMode::getTopDamageDealer() const {
    if (m_damageTrackers.empty()) return -1;
    
    auto maxIt = std::max_element(m_damageTrackers.begin(), m_damageTrackers.end(),
        [](const auto& a, const auto& b) {
            return a.second.totalDamage < b.second.totalDamage;
        });
    
    return maxIt->first;
}

std::vector<DamageTracker> BeastMode::getSortedDamageRankings() const {
    std::vector<DamageTracker> rankings;
    
    for (const auto& [id, tracker] : m_damageTrackers) {
        if (id != m_currentBeastId) { // Don't include beast in rankings
            rankings.push_back(tracker);
        }
    }
    
    // Sort by damage dealt (descending)
    std::sort(rankings.begin(), rankings.end(),
        [](const auto& a, const auto& b) {
            return a.totalDamage > b.totalDamage;
        });
    
    return rankings;
}

void BeastMode::updateBeastAI(float deltaTime) {
    // Simple beast AI for when no player controls it
    // This could be expanded with more complex behavior patterns
    
    static float attackCooldown = 0.0f;
    attackCooldown -= deltaTime;
    
    if (attackCooldown <= 0.0f && !m_hunters.empty()) {
        performBeastAttack();
        attackCooldown = 2.0f; // Attack every 2 seconds
    }
}

void BeastMode::performBeastAttack() {
    if (!m_beastCharacter || m_hunters.empty()) return;
    
    // Find closest hunter
    auto closestHunter = m_hunters[0];
    float minDistance = FLT_MAX;
    
    for (const auto& hunter : m_hunters) {
        float distance = m_beastCharacter->getDistanceTo(hunter.get());
        if (distance < minDistance) {
            minDistance = distance;
            closestHunter = hunter;
        }
    }
    
    // Perform attack towards closest hunter
    m_beastCharacter->performAutoAttack(closestHunter->getPosition());
}

void BeastMode::playTransformationEffect() {
    // Visual effect implementation
    // This would interface with the rendering system
}

void BeastMode::updateBeastVisuals() {
    if (!m_beastCharacter) return;
    
    // Gradually scale up during transformation
    float progress = m_transformTimer / m_beastConfig.beastTransformTime;
    float currentScale = 1.0f + (m_beastConfig.beastSizeMultiplier - 1.0f) * progress;
    m_beastCharacter->setScale(currentScale);
    
    // Apply other visual effects (glow, particles, etc.)
}

void BeastMode::startRound() {
    GameMode::startRound();
    
    // Reset damage trackers
    for (auto& [id, tracker] : m_damageTrackers) {
        tracker.totalDamage = 0.0f;
        tracker.highestCombo = 0.0f;
        tracker.hitCount = 0;
    }
    
    m_totalBeastDamage = 0.0f;
    m_beastTimer = 0.0f;
    
    setBeastState(BeastModeState::SelectingBeast);
}

void BeastMode::endRound(int winnerId, WinCondition condition) {
    // Determine round winner based on damage dealt
    if (condition == WinCondition::BeastDamage) {
        winnerId = getTopDamageDealer();
    }
    
    GameMode::endRound(winnerId, condition);
    
    // Revert beast transformation
    revertFromBeast();
}

bool BeastMode::checkWinConditions() {
    // Check if beast is defeated
    if (m_beastCharacter && m_beastCharacter->isDead()) {
        setBeastState(BeastModeState::BeastDefeated);
        return true;
    }
    
    // Check if time expired
    if (m_beastTimer >= m_beastConfig.beastModeDuration) {
        setBeastState(BeastModeState::TimeExpired);
        return true;
    }
    
    return false;
}

int BeastMode::calculateRoundWinner() {
    // Winner is the player who dealt most damage to beast
    return getTopDamageDealer();
}

void BeastMode::forceBeastSelection(int playerId) {
    if (m_beastState != BeastModeState::SelectingBeast) return;
    
    m_currentBeastId = playerId;
    transformToBeast(playerId);
}

float BeastMode::getBeastHealthPercentage() const {
    if (!m_beastCharacter) return 0.0f;
    
    auto stats = m_beastCharacter->getStats();
    return (stats.currentHealth / stats.maxHealth) * 100.0f;
}

float BeastMode::getPlayerDamageDealt(int playerId) const {
    auto it = m_damageTrackers.find(playerId);
    return it != m_damageTrackers.end() ? it->second.totalDamage : 0.0f;
}

DamageTracker BeastMode::getPlayerStats(int playerId) const {
    auto it = m_damageTrackers.find(playerId);
    return it != m_damageTrackers.end() ? it->second : DamageTracker{playerId, 0, 0, 0};
}

std::vector<DamageTracker> BeastMode::getAllPlayerStats() const {
    return getSortedDamageRankings();
}

} // namespace ArenaFighter