#include "ForGloryMode.h"
#include <algorithm>

namespace ArenaFighter {

ForGloryMode::ForGloryMode(const ForGloryConfig& config)
    : GameMode(config)
    , m_forGloryConfig(config)
    , m_currentHeroRound(0) {
    
    // Update base config
    m_config.maxPlayers = 2; // For Glory is always 1v1
    m_config.roundsToWin = 2; // Best of 3
}

void ForGloryMode::initialize() {
    GameMode::initialize();
    
    // Initialize glory points
    for (const auto& player : m_players) {
        m_gloryPoints[player->getId()] = 0;
    }
    
    setupHeroes();
}

void ForGloryMode::update(float deltaTime) {
    GameMode::update(deltaTime);
    
    if (m_currentState != MatchState::InProgress) return;
    
    // Check for hero deaths and transitions
    for (const auto& player : m_players) {
        int playerId = player->getId();
        checkHeroTransition(playerId);
    }
    
    // Check win conditions
    if (checkWinConditions()) {
        int winner = calculateRoundWinner();
        endRound(winner, WinCondition::Knockout);
    }
}

void ForGloryMode::render() {
    GameMode::render();
    
    // Render hero status UI
    if (m_gameUI) {
        for (const auto& [playerId, heroes] : m_playerHeroes) {
            m_gameUI->renderHeroStatus(playerId, heroes, m_activeHeroIndex[playerId]);
        }
        
        // Render glory points
        m_gameUI->renderGloryPoints(m_gloryPoints);
    }
}

void ForGloryMode::shutdown() {
    m_playerHeroes.clear();
    m_activeHeroIndex.clear();
    m_gloryPoints.clear();
    m_heroesLostPerRound.clear();
    
    GameMode::shutdown();
}

void ForGloryMode::addPlayer(std::shared_ptr<CharacterBase> character) {
    // In For Glory mode, we don't directly add characters
    // Instead, use setPlayerHeroes to set up the hero roster
    m_players.push_back(character);
}

void ForGloryMode::removePlayer(int playerId) {
    m_playerHeroes.erase(playerId);
    m_activeHeroIndex.erase(playerId);
    m_gloryPoints.erase(playerId);
    
    GameMode::removePlayer(playerId);
}

void ForGloryMode::setPlayerHeroes(int playerId, const std::vector<std::shared_ptr<CharacterBase>>& heroes) {
    if (heroes.size() != m_forGloryConfig.heroCount) {
        // Log error: incorrect hero count
        return;
    }
    
    m_playerHeroes[playerId].clear();
    
    for (const auto& hero : heroes) {
        HeroSlot slot{
            hero,
            false, // Will be set active during setup
            false, // Not dead
            hero->getStats().currentHealth,
            hero->getStats().currentMana
        };
        m_playerHeroes[playerId].push_back(slot);
    }
    
    // First hero is active by default
    m_activeHeroIndex[playerId] = 0;
    if (!m_playerHeroes[playerId].empty()) {
        m_playerHeroes[playerId][0].isActive = true;
    }
}

void ForGloryMode::setupHeroes() {
    // This is called at the start of each round
    for (auto& [playerId, heroes] : m_playerHeroes) {
        // Reset all heroes for the round
        for (int i = 0; i < heroes.size(); ++i) {
            auto& slot = heroes[i];
            slot.isDead = false;
            slot.isActive = (i == 0); // First hero active
            
            if (slot.character) {
                // Full restore at round start
                slot.character->respawn();
                slot.character->setPosition(getPlayer(playerId)->getPosition());
                slot.currentHealth = slot.character->getStats().maxHealth;
                slot.currentMana = slot.character->getStats().maxMana;
            }
        }
        
        m_activeHeroIndex[playerId] = 0;
    }
}

void ForGloryMode::switchToNextHero(int playerId) {
    auto& heroes = m_playerHeroes[playerId];
    int currentIndex = m_activeHeroIndex[playerId];
    
    // Deactivate current hero
    if (currentIndex < heroes.size()) {
        heroes[currentIndex].isActive = false;
    }
    
    // Find next alive hero
    int nextIndex = -1;
    for (int i = 0; i < heroes.size(); ++i) {
        if (!heroes[i].isDead && i != currentIndex) {
            nextIndex = i;
            break;
        }
    }
    
    if (nextIndex != -1) {
        // Switch to next hero
        m_activeHeroIndex[playerId] = nextIndex;
        heroes[nextIndex].isActive = true;
        
        // Position new hero
        if (heroes[nextIndex].character) {
            Vector3 pos = heroes[currentIndex].character ? 
                heroes[currentIndex].character->getPosition() : Vector3(0, 0, 0);
            heroes[nextIndex].character->setPosition(pos);
            
            // Brief invincibility on switch
            heroes[nextIndex].character->setInvincible(1.0f);
        }
        
        // Notify UI
        if (m_gameUI) {
            m_gameUI->onHeroSwitch(playerId, currentIndex, nextIndex);
        }
    }
}

bool ForGloryMode::hasAliveHeroes(int playerId) const {
    auto it = m_playerHeroes.find(playerId);
    if (it == m_playerHeroes.end()) return false;
    
    for (const auto& slot : it->second) {
        if (!slot.isDead) return true;
    }
    
    return false;
}

int ForGloryMode::getAliveHeroCount(int playerId) const {
    auto it = m_playerHeroes.find(playerId);
    if (it == m_playerHeroes.end()) return 0;
    
    int count = 0;
    for (const auto& slot : it->second) {
        if (!slot.isDead) count++;
    }
    
    return count;
}

std::shared_ptr<CharacterBase> ForGloryMode::getActiveHero(int playerId) const {
    auto heroIt = m_playerHeroes.find(playerId);
    auto indexIt = m_activeHeroIndex.find(playerId);
    
    if (heroIt != m_playerHeroes.end() && indexIt != m_activeHeroIndex.end()) {
        int index = indexIt->second;
        if (index >= 0 && index < heroIt->second.size()) {
            return heroIt->second[index].character;
        }
    }
    
    return nullptr;
}

void ForGloryMode::handleInput(int playerId, const InputCommand& input) {
    // Get active hero and forward input
    auto hero = getActiveHero(playerId);
    if (hero) {
        hero->handleInput(input);
    }
    
    // Note: Hero switching is disabled in For Glory mode
    // So we don't handle switch commands
}

void ForGloryMode::onHeroDeath(int playerId, int heroIndex) {
    auto& heroes = m_playerHeroes[playerId];
    if (heroIndex < 0 || heroIndex >= heroes.size()) return;
    
    heroes[heroIndex].isDead = true;
    heroes[heroIndex].currentHealth = 0.0f;
    
    // Automatically switch to next hero
    switchToNextHero(playerId);
    
    // Award glory points to opponent for hero KO
    for (const auto& player : m_players) {
        if (player->getId() != playerId) {
            awardGloryPoints(player->getId(), 100); // 100 points per hero KO
        }
    }
}

void ForGloryMode::checkHeroTransition(int playerId) {
    auto heroIt = m_playerHeroes.find(playerId);
    auto indexIt = m_activeHeroIndex.find(playerId);
    
    if (heroIt == m_playerHeroes.end() || indexIt == m_activeHeroIndex.end()) return;
    
    int activeIndex = indexIt->second;
    if (activeIndex < 0 || activeIndex >= heroIt->second.size()) return;
    
    auto& activeSlot = heroIt->second[activeIndex];
    
    // Check if active hero is dead
    if (activeSlot.character && activeSlot.character->isDead() && !activeSlot.isDead) {
        onHeroDeath(playerId, activeIndex);
    }
    
    // Update health/mana tracking
    if (activeSlot.character && !activeSlot.isDead) {
        auto stats = activeSlot.character->getStats();
        activeSlot.currentHealth = stats.currentHealth;
        activeSlot.currentMana = stats.currentMana;
    }
}

void ForGloryMode::awardGloryPoints(int playerId, int points) {
    if (m_gloryPoints.find(playerId) != m_gloryPoints.end()) {
        m_gloryPoints[playerId] += points;
        
        // Notify UI
        if (m_gameUI) {
            m_gameUI->onGloryPointsAwarded(playerId, points);
        }
    }
}

void ForGloryMode::calculateRoundGloryPoints() {
    // Award bonus glory points based on round performance
    for (const auto& [playerId, heroes] : m_playerHeroes) {
        int aliveCount = getAliveHeroCount(playerId);
        
        // Perfect round bonus (no heroes lost)
        if (aliveCount == m_forGloryConfig.heroCount) {
            awardGloryPoints(playerId, 200);
        }
        
        // Survival bonus
        awardGloryPoints(playerId, aliveCount * 50);
    }
}

void ForGloryMode::startRound() {
    GameMode::startRound();
    
    m_currentHeroRound++;
    setupHeroes();
    
    // Clear active character references
    m_players.clear();
    
    // Set first hero as active character for each player
    for (auto& [playerId, heroes] : m_playerHeroes) {
        if (!heroes.empty() && heroes[0].character) {
            m_players.push_back(heroes[0].character);
        }
    }
}

void ForGloryMode::endRound(int winnerId, WinCondition condition) {
    // Track heroes lost this round
    int p1Lost = m_forGloryConfig.heroCount - getAliveHeroCount(m_players[0]->getId());
    int p2Lost = m_forGloryConfig.heroCount - getAliveHeroCount(m_players[1]->getId());
    m_heroesLostPerRound.push_back({p1Lost, p2Lost});
    
    // Calculate glory points for the round
    calculateRoundGloryPoints();
    
    GameMode::endRound(winnerId, condition);
}

bool ForGloryMode::checkWinConditions() {
    // Check if any player has no heroes left
    for (const auto& player : m_players) {
        if (!hasAliveHeroes(player->getId())) {
            return true;
        }
    }
    
    // Check time out
    if (m_roundTimer >= m_config.roundTime && !m_config.infiniteTime) {
        return true;
    }
    
    return false;
}

int ForGloryMode::calculateRoundWinner() {
    // Winner is player with heroes remaining
    for (const auto& player : m_players) {
        if (hasAliveHeroes(player->getId())) {
            return player->getId();
        }
    }
    
    // If both have heroes (timeout), winner has more total health
    float maxHealth = 0.0f;
    int winnerId = -1;
    
    for (const auto& [playerId, heroes] : m_playerHeroes) {
        float totalHealth = 0.0f;
        for (const auto& slot : heroes) {
            if (!slot.isDead) {
                totalHealth += slot.currentHealth;
            }
        }
        
        if (totalHealth > maxHealth) {
            maxHealth = totalHealth;
            winnerId = playerId;
        }
    }
    
    return winnerId;
}

std::vector<HeroSlot> ForGloryMode::getPlayerHeroes(int playerId) const {
    auto it = m_playerHeroes.find(playerId);
    return it != m_playerHeroes.end() ? it->second : std::vector<HeroSlot>();
}

int ForGloryMode::getActiveHeroIndex(int playerId) const {
    auto it = m_activeHeroIndex.find(playerId);
    return it != m_activeHeroIndex.end() ? it->second : -1;
}

bool ForGloryMode::isHeroDead(int playerId, int heroIndex) const {
    auto it = m_playerHeroes.find(playerId);
    if (it != m_playerHeroes.end() && heroIndex >= 0 && heroIndex < it->second.size()) {
        return it->second[heroIndex].isDead;
    }
    return true;
}

float ForGloryMode::getHeroHealth(int playerId, int heroIndex) const {
    auto it = m_playerHeroes.find(playerId);
    if (it != m_playerHeroes.end() && heroIndex >= 0 && heroIndex < it->second.size()) {
        return it->second[heroIndex].currentHealth;
    }
    return 0.0f;
}

float ForGloryMode::getHeroMana(int playerId, int heroIndex) const {
    auto it = m_playerHeroes.find(playerId);
    if (it != m_playerHeroes.end() && heroIndex >= 0 && heroIndex < it->second.size()) {
        return it->second[heroIndex].currentMana;
    }
    return 0.0f;
}

int ForGloryMode::getGloryPoints(int playerId) const {
    auto it = m_gloryPoints.find(playerId);
    return it != m_gloryPoints.end() ? it->second : 0;
}

int ForGloryMode::getTotalHeroesLost(int playerId) const {
    int total = 0;
    int playerIndex = 0;
    
    // Determine player index (0 or 1)
    for (int i = 0; i < m_players.size(); ++i) {
        if (m_players[i]->getId() == playerId) {
            playerIndex = i;
            break;
        }
    }
    
    // Sum up heroes lost across all rounds
    for (const auto& [p1Lost, p2Lost] : m_heroesLostPerRound) {
        total += (playerIndex == 0) ? p1Lost : p2Lost;
    }
    
    return total;
}

} // namespace ArenaFighter