#include "DeathMatchMode.h"
#include <algorithm>
#include <random>

namespace ArenaFighter {

DeathMatchMode::DeathMatchMode(const DeathMatchConfig& config)
    : GameMode(config)
    , m_deathMatchConfig(config)
    , m_nextItemSpawn(0.0f)
    , m_lastKillTime(0.0f) {
    
    // Initialize RNG
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    m_rng.seed(seed);
    
    // Update base config
    m_config.maxPlayers = 8;
    m_config.roundTime = config.matchDuration;
    m_config.infiniteTime = false; // Death match has time limit
}

void DeathMatchMode::initialize() {
    GameMode::initialize();
    
    // Initialize spawn points
    initializeSpawnPoints();
    
    // Initialize item spawns if enabled
    if (m_deathMatchConfig.enableItems) {
        initializeItemSpawns();
    }
    
    // Initialize player stats
    for (const auto& player : m_players) {
        m_playerStats[player->getId()] = DeathMatchStats{
            player->getId(), 0, 0, 0, 0.0f, 0.0f, 0, 0, 0, 0.0f
        };
        m_alivePlayers.insert(player->getId());
    }
}

void DeathMatchMode::update(float deltaTime) {
    GameMode::update(deltaTime);
    
    if (m_currentState != MatchState::InProgress) return;
    
    // Update player statistics
    updatePlayerStats(deltaTime);
    
    // Process respawn queue
    for (auto it = m_respawnQueue.begin(); it != m_respawnQueue.end();) {
        it->second -= deltaTime;
        if (it->second <= 0.0f) {
            respawnPlayer(it->first);
            it = m_respawnQueue.erase(it);
        } else {
            ++it;
        }
    }
    
    // Update items
    if (m_deathMatchConfig.enableItems) {
        updateItems(deltaTime);
        
        // Check for item spawning
        m_nextItemSpawn -= deltaTime;
        if (m_nextItemSpawn <= 0.0f && 
            m_activeItems.size() < m_deathMatchConfig.maxActiveItems) {
            spawnItem();
            m_nextItemSpawn = m_deathMatchConfig.itemSpawnInterval;
        }
    }
    
    // Check win conditions
    if (checkWinConditions()) {
        int winner = calculateRoundWinner();
        endRound(winner, WinCondition::LastStanding);
    }
}

void DeathMatchMode::render() {
    GameMode::render();
    
    // Render active items
    if (m_gameUI) {
        for (const auto& item : m_activeItems) {
            if (item.isActive) {
                m_gameUI->renderItem(item.type, item.position);
            }
        }
        
        // Update scoreboard
        auto sortedStats = getSortedStats();
        m_gameUI->updateScoreboard(sortedStats);
        
        // Update kill feed
        auto recentKills = getRecentKills();
        m_gameUI->updateKillFeed(recentKills);
    }
}

void DeathMatchMode::shutdown() {
    m_playerStats.clear();
    m_alivePlayers.clear();
    m_respawnQueue.clear();
    m_activeItems.clear();
    m_itemSpawns.clear();
    m_recentKills.clear();
    
    GameMode::shutdown();
}

void DeathMatchMode::addPlayer(std::shared_ptr<CharacterBase> character) {
    GameMode::addPlayer(character);
    
    // Initialize stats for new player
    m_playerStats[character->getId()] = DeathMatchStats{
        character->getId(), 0, 0, 0, 0.0f, 0.0f, 0, 0, 0, 0.0f
    };
    m_alivePlayers.insert(character->getId());
    
    // Spawn at random point
    character->setPosition(getRandomSpawnPoint());
}

void DeathMatchMode::removePlayer(int playerId) {
    m_playerStats.erase(playerId);
    m_alivePlayers.erase(playerId);
    
    // Remove from respawn queue
    m_respawnQueue.erase(
        std::remove_if(m_respawnQueue.begin(), m_respawnQueue.end(),
            [playerId](const auto& p) { return p.first == playerId; }),
        m_respawnQueue.end()
    );
    
    GameMode::removePlayer(playerId);
}

void DeathMatchMode::onPlayerDeath(int victimId, int killerId) {
    // Update alive status
    m_alivePlayers.erase(victimId);
    
    // Register the kill
    registerKill(killerId, victimId);
    
    // Add to respawn queue
    m_respawnQueue.push_back({victimId, m_deathMatchConfig.respawnTime});
    
    // Update stats
    if (m_playerStats.find(victimId) != m_playerStats.end()) {
        m_playerStats[victimId].deaths++;
        m_playerStats[victimId].currentKillStreak = 0;
    }
    
    if (killerId >= 0 && m_playerStats.find(killerId) != m_playerStats.end()) {
        m_playerStats[killerId].kills++;
        updateKillStreaks(killerId);
        checkMultiKills(killerId);
    }
}

void DeathMatchMode::onPlayerRespawn(int playerId) {
    m_alivePlayers.insert(playerId);
    
    // Reset player state
    if (auto player = getPlayer(playerId)) {
        player->respawn();
        player->setPosition(getFarthestSpawnPoint(playerId));
        
        // Brief invincibility after respawn
        player->setInvincible(2.0f); // 2 seconds
    }
}

void DeathMatchMode::initializeSpawnPoints() {
    // Define spawn points based on map layout
    // This should be loaded from map data in real implementation
    m_spawnPoints = {
        Vector3(0, 0, 0),
        Vector3(50, 0, 0),
        Vector3(-50, 0, 0),
        Vector3(0, 0, 50),
        Vector3(0, 0, -50),
        Vector3(35, 0, 35),
        Vector3(-35, 0, 35),
        Vector3(35, 0, -35),
        Vector3(-35, 0, -35)
    };
}

Vector3 DeathMatchMode::getRandomSpawnPoint() const {
    if (m_spawnPoints.empty()) return Vector3(0, 0, 0);
    
    std::uniform_int_distribution<int> dist(0, m_spawnPoints.size() - 1);
    return m_spawnPoints[dist(const_cast<std::mt19937&>(m_rng))];
}

Vector3 DeathMatchMode::getFarthestSpawnPoint(int playerId) const {
    if (m_spawnPoints.empty()) return Vector3(0, 0, 0);
    
    Vector3 bestSpawn = m_spawnPoints[0];
    float maxMinDistance = 0.0f;
    
    // Find spawn point that maximizes minimum distance to all alive players
    for (const auto& spawn : m_spawnPoints) {
        float minDistance = FLT_MAX;
        
        for (int aliveId : m_alivePlayers) {
            if (aliveId != playerId) {
                if (auto player = getPlayer(aliveId)) {
                    float dist = (spawn - player->getPosition()).length();
                    minDistance = std::min(minDistance, dist);
                }
            }
        }
        
        if (minDistance > maxMinDistance) {
            maxMinDistance = minDistance;
            bestSpawn = spawn;
        }
    }
    
    return bestSpawn;
}

void DeathMatchMode::respawnPlayer(int playerId) {
    onPlayerRespawn(playerId);
}

void DeathMatchMode::initializeItemSpawns() {
    // Define item spawn points
    m_itemSpawnPoints = {
        Vector3(25, 5, 25),
        Vector3(-25, 5, 25),
        Vector3(25, 5, -25),
        Vector3(-25, 5, -25),
        Vector3(0, 5, 0),
        Vector3(40, 5, 0),
        Vector3(-40, 5, 0),
        Vector3(0, 5, 40),
        Vector3(0, 5, -40)
    };
}

void DeathMatchMode::spawnItem() {
    if (m_itemSpawnPoints.empty()) return;
    
    // Select random spawn point
    std::uniform_int_distribution<int> spawnDist(0, m_itemSpawnPoints.size() - 1);
    Vector3 spawnPos = m_itemSpawnPoints[spawnDist(m_rng)];
    
    // Select random item type
    std::uniform_int_distribution<int> typeDist(0, static_cast<int>(ItemType::InstantUltimate));
    ItemType itemType = static_cast<ItemType>(typeDist(m_rng));
    
    // Create item
    SpawnableItem newItem{
        itemType,
        spawnPos,
        30.0f, // 30 second respawn
        true,
        -1,
        std::chrono::steady_clock::now()
    };
    
    m_activeItems.push_back(newItem);
}

void DeathMatchMode::updateItems(float deltaTime) {
    // Check item collection
    for (auto& item : m_activeItems) {
        if (!item.isActive) continue;
        
        // Check collision with players
        for (int playerId : m_alivePlayers) {
            if (auto player = getPlayer(playerId)) {
                float distance = (player->getPosition() - item.position).length();
                if (distance < 2.0f) { // Collection radius
                    collectItem(playerId, item);
                    break;
                }
            }
        }
    }
    
    // Remove inactive items that have expired
    auto now = std::chrono::steady_clock::now();
    m_activeItems.erase(
        std::remove_if(m_activeItems.begin(), m_activeItems.end(),
            [now](const auto& item) {
                if (!item.isActive) {
                    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                        now - item.spawnTime).count();
                    return elapsed > item.respawnTime;
                }
                return false;
            }),
        m_activeItems.end()
    );
}

void DeathMatchMode::collectItem(int playerId, SpawnableItem& item) {
    item.isActive = false;
    item.lastCollectorId = playerId;
    
    // Apply item effect
    applyItemEffect(playerId, item.type);
    
    // Update stats
    if (m_playerStats.find(playerId) != m_playerStats.end()) {
        m_playerStats[playerId].itemsCollected++;
    }
    
    // Notify UI
    if (m_gameUI) {
        m_gameUI->onItemCollected(playerId, item.type);
    }
}

void DeathMatchMode::applyItemEffect(int playerId, ItemType type) {
    auto player = getPlayer(playerId);
    if (!player) return;
    
    switch (type) {
        case ItemType::HealthRestore:
            player->heal(300.0f); // 30% health
            break;
            
        case ItemType::ManaRestore:
            player->restoreMana(50.0f); // 50% mana
            break;
            
        case ItemType::DamageBoost:
            player->applyBuff(BuffType::Damage, 1.5f, 10.0f); // 50% damage for 10s
            break;
            
        case ItemType::DefenseBoost:
            player->applyBuff(BuffType::Defense, 1.5f, 10.0f); // 50% defense for 10s
            break;
            
        case ItemType::SpeedBoost:
            player->applyBuff(BuffType::Speed, 1.3f, 8.0f); // 30% speed for 8s
            break;
            
        case ItemType::Invincibility:
            player->setInvincible(3.0f); // 3 seconds
            break;
            
        case ItemType::DoublePoints:
            // This would be handled by the scoring system
            player->applyBuff(BuffType::DoublePoints, 2.0f, 15.0f);
            break;
            
        case ItemType::InstantUltimate:
            player->setMana(100.0f); // Full mana
            break;
    }
}

void DeathMatchMode::registerKill(int killerId, int victimId, int assistId) {
    m_recentKills.push_front({killerId, victimId});
    if (m_recentKills.size() > 10) {
        m_recentKills.pop_back();
    }
    
    m_lastKillTime = m_roundTimer;
    
    // Handle assist if applicable
    if (assistId >= 0 && m_playerStats.find(assistId) != m_playerStats.end()) {
        m_playerStats[assistId].assists++;
    }
}

void DeathMatchMode::updateKillStreaks(int killerId) {
    auto& stats = m_playerStats[killerId];
    stats.currentKillStreak++;
    
    if (stats.currentKillStreak > stats.longestKillStreak) {
        stats.longestKillStreak = stats.currentKillStreak;
    }
    
    // Announce kill streaks
    if (m_deathMatchConfig.enableKillStreaks) {
        announceKillStreak(killerId, stats.currentKillStreak);
    }
}

void DeathMatchMode::checkMultiKills(int killerId) {
    // Count recent kills by this player
    int recentKillCount = 0;
    float killWindow = 3.0f; // 3 second window for multi-kills
    
    for (const auto& [killer, victim] : m_recentKills) {
        if (killer == killerId && (m_roundTimer - m_lastKillTime) < killWindow) {
            recentKillCount++;
        }
    }
    
    // Announce multi-kills
    if (m_gameUI && recentKillCount > 1) {
        m_gameUI->announceMultiKill(killerId, recentKillCount);
    }
}

void DeathMatchMode::announceKillStreak(int playerId, int streak) {
    if (!m_gameUI) return;
    
    // Announce based on streak count
    if (streak == 3) {
        m_gameUI->announceKillStreak(playerId, "Killing Spree!");
    } else if (streak == 5) {
        m_gameUI->announceKillStreak(playerId, "Rampage!");
    } else if (streak == 7) {
        m_gameUI->announceKillStreak(playerId, "Dominating!");
    } else if (streak == 10) {
        m_gameUI->announceKillStreak(playerId, "Unstoppable!");
    } else if (streak == 15) {
        m_gameUI->announceKillStreak(playerId, "Godlike!");
    }
}

void DeathMatchMode::updatePlayerStats(float deltaTime) {
    // Update survival time for alive players
    for (int playerId : m_alivePlayers) {
        if (m_playerStats.find(playerId) != m_playerStats.end()) {
            m_playerStats[playerId].survivalTime += deltaTime;
        }
    }
    
    // Update damage tracking from combat system
    auto damageEvents = m_combatSystem->getRecentDamageEvents();
    for (const auto& event : damageEvents) {
        if (m_playerStats.find(event.attackerId) != m_playerStats.end()) {
            m_playerStats[event.attackerId].damageDealt += event.damage;
        }
        if (m_playerStats.find(event.targetId) != m_playerStats.end()) {
            m_playerStats[event.targetId].damageTaken += event.damage;
        }
    }
}

int DeathMatchMode::getLeadingPlayer() const {
    if (m_playerStats.empty()) return -1;
    
    auto maxIt = std::max_element(m_playerStats.begin(), m_playerStats.end(),
        [](const auto& a, const auto& b) {
            return a.second.kills < b.second.kills;
        });
    
    return maxIt->first;
}

std::vector<DeathMatchStats> DeathMatchMode::getSortedStats() const {
    std::vector<DeathMatchStats> stats;
    
    for (const auto& [id, stat] : m_playerStats) {
        stats.push_back(stat);
    }
    
    // Sort by kills (primary), then by deaths (secondary)
    std::sort(stats.begin(), stats.end(),
        [](const auto& a, const auto& b) {
            if (a.kills != b.kills) {
                return a.kills > b.kills;
            }
            return a.deaths < b.deaths;
        });
    
    return stats;
}

void DeathMatchMode::startRound() {
    GameMode::startRound();
    
    // Reset all stats
    for (auto& [id, stats] : m_playerStats) {
        stats.kills = 0;
        stats.deaths = 0;
        stats.assists = 0;
        stats.damageDealt = 0.0f;
        stats.damageTaken = 0.0f;
        stats.itemsCollected = 0;
        stats.longestKillStreak = 0;
        stats.currentKillStreak = 0;
        stats.survivalTime = 0.0f;
    }
    
    // Clear game state
    m_alivePlayers.clear();
    for (const auto& player : m_players) {
        m_alivePlayers.insert(player->getId());
    }
    
    m_respawnQueue.clear();
    m_recentKills.clear();
    m_activeItems.clear();
    
    // Spawn initial items
    if (m_deathMatchConfig.enableItems) {
        for (int i = 0; i < 3; ++i) {
            spawnItem();
        }
    }
    
    m_nextItemSpawn = m_deathMatchConfig.itemSpawnInterval;
}

void DeathMatchMode::endRound(int winnerId, WinCondition condition) {
    GameMode::endRound(winnerId, condition);
}

bool DeathMatchMode::checkWinConditions() {
    // Check kill target
    for (const auto& [id, stats] : m_playerStats) {
        if (stats.kills >= m_deathMatchConfig.targetKills) {
            return true;
        }
    }
    
    // Check time limit
    if (m_roundTimer >= m_deathMatchConfig.matchDuration) {
        return true;
    }
    
    // Check if only one player remains (unlikely in respawn mode)
    if (m_alivePlayers.size() <= 1 && m_respawnQueue.empty()) {
        return true;
    }
    
    return false;
}

int DeathMatchMode::calculateRoundWinner() {
    return getLeadingPlayer();
}

bool DeathMatchMode::tryCollectItem(int playerId, const Vector3& position) {
    for (auto& item : m_activeItems) {
        if (item.isActive) {
            float distance = (item.position - position).length();
            if (distance < 2.0f) {
                collectItem(playerId, item);
                return true;
            }
        }
    }
    return false;
}

DeathMatchStats DeathMatchMode::getPlayerStats(int playerId) const {
    auto it = m_playerStats.find(playerId);
    if (it != m_playerStats.end()) {
        return it->second;
    }
    return DeathMatchStats{playerId, 0, 0, 0, 0.0f, 0.0f, 0, 0, 0, 0.0f};
}

std::vector<DeathMatchStats> DeathMatchMode::getAllStats() const {
    return getSortedStats();
}

int DeathMatchMode::getPlayerKills(int playerId) const {
    auto it = m_playerStats.find(playerId);
    return it != m_playerStats.end() ? it->second.kills : 0;
}

int DeathMatchMode::getPlayerDeaths(int playerId) const {
    auto it = m_playerStats.find(playerId);
    return it != m_playerStats.end() ? it->second.deaths : 0;
}

float DeathMatchMode::getPlayerKDRatio(int playerId) const {
    auto it = m_playerStats.find(playerId);
    if (it != m_playerStats.end()) {
        if (it->second.deaths == 0) {
            return static_cast<float>(it->second.kills);
        }
        return static_cast<float>(it->second.kills) / it->second.deaths;
    }
    return 0.0f;
}

std::deque<std::pair<int, int>> DeathMatchMode::getRecentKills(int count) const {
    std::deque<std::pair<int, int>> recent;
    
    auto it = m_recentKills.begin();
    for (int i = 0; i < count && it != m_recentKills.end(); ++i, ++it) {
        recent.push_back(*it);
    }
    
    return recent;
}

bool DeathMatchMode::isPlayerAlive(int playerId) const {
    return m_alivePlayers.find(playerId) != m_alivePlayers.end();
}

float DeathMatchMode::getTimeUntilRespawn(int playerId) const {
    for (const auto& [id, time] : m_respawnQueue) {
        if (id == playerId) {
            return time;
        }
    }
    return 0.0f;
}

} // namespace ArenaFighter