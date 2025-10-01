#pragma once

#include "GameMode.h"
#include <deque>
#include <set>
#include <chrono>

namespace ArenaFighter {

// Item types that can spawn in death match
enum class ItemType {
    HealthRestore,      // Instant health restoration
    ManaRestore,        // Instant mana restoration
    DamageBoost,        // Temporary damage increase
    DefenseBoost,       // Temporary defense increase
    SpeedBoost,         // Temporary speed increase
    Invincibility,      // Brief invincibility
    DoublePoints,       // Double kill points
    InstantUltimate     // Fill ultimate meter
};

// Spawnable item
struct SpawnableItem {
    ItemType type;
    Vector3 position;
    float respawnTime;
    bool isActive;
    int lastCollectorId;
    std::chrono::steady_clock::time_point spawnTime;
};

// Death match statistics
struct DeathMatchStats {
    int playerId;
    int kills;
    int deaths;
    int assists;
    float damageDealt;
    float damageTaken;
    int itemsCollected;
    int longestKillStreak;
    int currentKillStreak;
    float survivalTime;
};

// Death match configuration
struct DeathMatchConfig : public MatchConfig {
    int targetKills = 20;              // Kills needed to win
    float matchDuration = 300.0f;      // 5 minute matches
    float respawnTime = 3.0f;          // Respawn delay
    bool enableItems = true;           // Enable item spawns
    float itemSpawnInterval = 15.0f;   // Item spawn frequency
    int maxActiveItems = 5;            // Max items on map
    bool enableKillStreaks = true;     // Enable killstreak bonuses
};

// Death Match Mode: 8 player FFA with item spawns
class DeathMatchMode : public GameMode {
private:
    DeathMatchConfig m_deathMatchConfig;
    
    // Player tracking
    std::map<int, DeathMatchStats> m_playerStats;
    std::set<int> m_alivePlayers;
    std::deque<std::pair<int, float>> m_respawnQueue; // playerId, respawnTime
    
    // Item management
    std::vector<SpawnableItem> m_itemSpawns;
    std::vector<SpawnableItem> m_activeItems;
    float m_nextItemSpawn;
    std::mt19937 m_rng;
    
    // Kill tracking
    std::deque<std::pair<int, int>> m_recentKills; // killer, victim
    float m_lastKillTime;
    
    // Map zones for spawning
    std::vector<Vector3> m_spawnPoints;
    std::vector<Vector3> m_itemSpawnPoints;
    
    // Spawn management
    void initializeSpawnPoints();
    Vector3 getRandomSpawnPoint() const;
    Vector3 getFarthestSpawnPoint(int playerId) const;
    void respawnPlayer(int playerId);
    
    // Item management
    void initializeItemSpawns();
    void spawnItem();
    void updateItems(float deltaTime);
    void collectItem(int playerId, SpawnableItem& item);
    void applyItemEffect(int playerId, ItemType type);
    
    // Kill tracking
    void registerKill(int killerId, int victimId, int assistId = -1);
    void updateKillStreaks(int killerId);
    void checkMultiKills(int killerId);
    void announceKillStreak(int playerId, int streak);
    
    // Statistics
    void updatePlayerStats(float deltaTime);
    int getLeadingPlayer() const;
    std::vector<DeathMatchStats> getSortedStats() const;
    
    // Round management overrides
    virtual void startRound() override;
    virtual void endRound(int winnerId, WinCondition condition) override;
    virtual bool checkWinConditions() override;
    virtual int calculateRoundWinner() override;

public:
    explicit DeathMatchMode(const DeathMatchConfig& config);
    virtual ~DeathMatchMode() = default;
    
    // Game mode implementation
    virtual void initialize() override;
    virtual void update(float deltaTime) override;
    virtual void render() override;
    virtual void shutdown() override;
    
    // Player management
    virtual void addPlayer(std::shared_ptr<CharacterBase> character) override;
    virtual void removePlayer(int playerId) override;
    
    // Death handling
    void onPlayerDeath(int victimId, int killerId);
    void onPlayerRespawn(int playerId);
    
    // Item interaction
    bool tryCollectItem(int playerId, const Vector3& position);
    std::vector<SpawnableItem> getActiveItems() const { return m_activeItems; }
    
    // Statistics access
    DeathMatchStats getPlayerStats(int playerId) const;
    std::vector<DeathMatchStats> getAllStats() const;
    int getPlayerKills(int playerId) const;
    int getPlayerDeaths(int playerId) const;
    float getPlayerKDRatio(int playerId) const;
    
    // Configuration
    void setDeathMatchConfig(const DeathMatchConfig& config) { m_deathMatchConfig = config; }
    DeathMatchConfig getDeathMatchConfig() const { return m_deathMatchConfig; }
    
    // Game mode properties
    virtual std::string getModeName() const override { return "Death Match"; }
    virtual GameModeType getModeType() const override { return GameModeType::DeathMatch; }
    virtual bool supportsOnline() const override { return true; }
    virtual int getMinPlayers() const override { return 2; }
    virtual int getMaxPlayers() const override { return 8; }
    
    // Kill feed
    std::deque<std::pair<int, int>> getRecentKills(int count = 5) const;
    
    // Respawn management
    bool isPlayerAlive(int playerId) const;
    float getTimeUntilRespawn(int playerId) const;
    
    // Item spawn predictions
    float getTimeUntilNextItem() const { return m_nextItemSpawn; }
    std::vector<Vector3> getPotentialItemSpawns() const { return m_itemSpawnPoints; }
};

} // namespace ArenaFighter