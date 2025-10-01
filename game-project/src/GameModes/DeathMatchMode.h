#pragma once

#include "GameModesManager.h"
#include <random>
#include <DirectXMath.h>

namespace ArenaFighter {

struct SpawnPoint {
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 rotation;
    bool isOccupied = false;
    float cooldown = 0.0f;
};

struct PlayerScore {
    int playerID;
    int kills = 0;
    int deaths = 0;
    int assists = 0;
    int score = 0;
    int streak = 0;
    int bestStreak = 0;
    float damageDealt = 0.0f;
    float damageTaken = 0.0f;
};

class DeathMatchMode : public GameModeBase {
public:
    DeathMatchMode();
    bool Initialize() override;
    void Start() override;
    void Update(float deltaTime) override;
    void End() override;
    bool IsMatchComplete() const override;
    int GetWinner() const override;
    
    // Game configuration
    void SetMaxPlayers(int max) { m_maxPlayers = max; }
    void SetScoreLimit(int limit) { m_scoreLimit = limit; }
    void SetTimeLimit(float minutes) { m_timeLimit = minutes * 60.0f; }
    void SetMapName(const std::string& map) { m_mapName = map; }
    
    // Player management
    bool AddPlayer(int playerID);
    void RemovePlayer(int playerID);
    void RespawnPlayer(int playerID);
    
    // Scoring system
    void OnPlayerKill(int killerID, int victimID);
    void OnPlayerAssist(int assistID, int victimID, float damagePercent);
    void OnPlayerDamage(int attackerID, int targetID, float damage);
    
    // Spawn system
    DirectX::XMFLOAT3 GetBestSpawnPoint(int playerID);
    void AddSpawnPoint(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation);
    
    // Power-ups and pickups
    void SpawnPowerUp();
    void OnPowerUpCollected(int playerID, const std::string& powerUpType);
    
    // Leaderboard
    std::vector<PlayerScore> GetLeaderboard() const;
    PlayerScore GetPlayerScore(int playerID) const;
    
private:
    std::unordered_map<int, PlayerScore> m_playerScores;
    std::vector<SpawnPoint> m_spawnPoints;
    std::string m_mapName;
    
    // Match settings
    int m_scoreLimit = 20;  // First to 20 kills
    float m_timeLimit = 600.0f;  // 10 minutes
    float m_elapsedTime = 0.0f;
    
    // Spawn logic
    std::random_device m_randomDevice;
    std::mt19937 m_rng{m_randomDevice()};
    float GetSpawnSafety(const SpawnPoint& spawn, int playerID);
    
    // Kill streak bonuses
    void HandleKillStreak(int playerID);
    std::vector<std::pair<int, std::string>> m_streakRewards = {
        {3, "Triple Kill"},
        {5, "Rampage"},
        {7, "Unstoppable"},
        {10, "Godlike"},
        {15, "Beyond Godlike"}
    };
    
    // Power-up system
    struct PowerUp {
        std::string type;
        DirectX::XMFLOAT3 position;
        float respawnTimer = 0.0f;
        bool isActive = true;
    };
    std::vector<PowerUp> m_powerUps;
    float m_powerUpSpawnTimer = 30.0f;
    
    // Dynamic difficulty
    void BalanceMatch();
    void ApplyHandicap(int playerID, float multiplier);
};

// Free-for-all specific mechanics
class FFAMechanics {
public:
    // Spawn protection
    static constexpr float SPAWN_PROTECTION_TIME = 3.0f;
    static bool HasSpawnProtection(float timeSinceSpawn) {
        return timeSinceSpawn < SPAWN_PROTECTION_TIME;
    }
    
    // Score calculations
    static int CalculateKillScore(int streak, bool headshot = false) {
        int base = 100;
        if (headshot) base += 50;
        if (streak >= 3) base += streak * 10;
        return base;
    }
    
    static int CalculateAssistScore(float damagePercent) {
        return static_cast<int>(50 * damagePercent);
    }
    
    // Comeback mechanics
    static float GetDamageBonus(int playerRank, int totalPlayers) {
        if (playerRank > totalPlayers / 2) {
            float bonus = 0.1f * (playerRank - totalPlayers / 2);
            return std::min(1.5f, 1.0f + bonus);
        }
        return 1.0f;
    }
};

// Map configurations for Death Match
class DeathMatchMaps {
public:
    static std::vector<SpawnPoint> GetSpawnPoints(const std::string& mapName) {
        if (mapName == "Arena_Classic") {
            return {
                {{-10.0f, 0.0f, -10.0f}, {0.0f, 0.0f, 0.0f}},
                {{10.0f, 0.0f, -10.0f}, {0.0f, 180.0f, 0.0f}},
                {{10.0f, 0.0f, 10.0f}, {0.0f, 180.0f, 0.0f}},
                {{-10.0f, 0.0f, 10.0f}, {0.0f, 0.0f, 0.0f}},
                {{0.0f, 5.0f, 0.0f}, {0.0f, 90.0f, 0.0f}},
                {{-15.0f, 0.0f, 0.0f}, {0.0f, 90.0f, 0.0f}},
                {{15.0f, 0.0f, 0.0f}, {0.0f, -90.0f, 0.0f}},
                {{0.0f, 0.0f, -15.0f}, {0.0f, 0.0f, 0.0f}}
            };
        }
        // Add more maps
        return {};
    }
    
    static std::vector<DirectX::XMFLOAT3> GetPowerUpLocations(const std::string& mapName) {
        if (mapName == "Arena_Classic") {
            return {
                {0.0f, 1.0f, 0.0f},      // Center
                {-8.0f, 1.0f, -8.0f},    // Corners
                {8.0f, 1.0f, -8.0f},
                {8.0f, 1.0f, 8.0f},
                {-8.0f, 1.0f, 8.0f}
            };
        }
        return {};
    }
};

} // namespace ArenaFighter