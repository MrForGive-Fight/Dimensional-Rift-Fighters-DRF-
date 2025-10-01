#pragma once

#include "GameModesManager.h"
#include <queue>
#include <chrono>

namespace ArenaFighter {

enum class RankTier {
    Bronze,
    Silver,
    Gold,
    Platinum,
    Diamond,
    Master,
    Grandmaster,
    Challenger
};

struct RankedPlayer {
    int playerID;
    int rankPoints;
    RankTier tier;
    int wins = 0;
    int losses = 0;
    float winRate = 0.0f;
    std::chrono::steady_clock::time_point queueTime;
    
    // Matchmaking factors
    int recentPerformance = 0;  // Last 10 games
    float averageMatchDuration = 180.0f;
    int preferredMode = 1;  // 1v1 or 3v3
};

class MatchmakingSystem {
public:
    MatchmakingSystem();
    
    // Queue management
    void AddToQueue(const RankedPlayer& player, int mode);
    void RemoveFromQueue(int playerID);
    bool IsInQueue(int playerID) const;
    
    // Matchmaking algorithm
    struct Match {
        std::vector<int> team1;
        std::vector<int> team2;
        int averageRank;
        float fairnessScore;  // 0-1, higher is more fair
    };
    
    std::vector<Match> FindMatches();
    float CalculateFairness(const std::vector<RankedPlayer*>& team1, 
                           const std::vector<RankedPlayer*>& team2);
    
    // MMR calculations
    void UpdateMMR(int winnerID, int loserID, float matchDuration);
    int CalculateRankChange(const RankedPlayer& winner, const RankedPlayer& loser);
    
private:
    std::unordered_map<int, RankedPlayer> m_players;
    std::queue<int> m_1v1Queue;
    std::queue<int> m_3v3Queue;
    
    // Matchmaking parameters
    static constexpr int RANK_DIFFERENCE_THRESHOLD = 200;
    static constexpr float QUEUE_TIME_EXPANSION = 50.0f; // Points per second
    static constexpr float MAX_RANK_DIFFERENCE = 500.0f;
};

class Ranked1v1Mode : public GameModeBase {
public:
    Ranked1v1Mode();
    bool Initialize() override;
    void Start() override;
    void Update(float deltaTime) override;
    void End() override;
    bool IsMatchComplete() const override;
    int GetWinner() const override;
    
    // Ranked specific
    void SetPlayers(int player1ID, int player2ID);
    void RecordRound(int winnerID);
    
private:
    int m_player1ID = -1;
    int m_player2ID = -1;
    
    // Best of 5 rounds
    static constexpr int ROUNDS_TO_WIN = 3;
    int m_player1Rounds = 0;
    int m_player2Rounds = 0;
    int m_currentRound = 1;
    
    float m_roundTimer = 99.0f;
    static constexpr float ROUND_TIME = 99.0f;
    
    // Performance tracking
    float m_matchStartTime = 0.0f;
    int m_totalDamageDealt[2] = {0, 0};
    int m_comboCount[2] = {0, 0};
};

class Ranked3v3Mode : public GameModeBase {
public:
    Ranked3v3Mode();
    bool Initialize() override;
    void Start() override;
    void Update(float deltaTime) override;
    void End() override;
    bool IsMatchComplete() const override;
    int GetWinner() const override;
    
    // Team management
    void SetTeams(const std::vector<int>& team1, const std::vector<int>& team2);
    void OnPlayerEliminated(int playerID);
    
private:
    std::vector<int> m_team1;
    std::vector<int> m_team2;
    std::vector<int> m_eliminatedPlayers;
    
    // Elimination rules
    int GetAliveCount(const std::vector<int>& team) const;
    int GetWinningTeam() const;
    
    // Match timer
    float m_matchTimer = 300.0f;  // 5 minutes
    static constexpr float MAX_MATCH_TIME = 300.0f;
    
    // Respawn system (optional)
    bool m_allowRespawn = false;
    std::unordered_map<int, float> m_respawnTimers;
    static constexpr float RESPAWN_TIME = 10.0f;
};

// Ranking calculation utilities
class RankingSystem {
public:
    static RankTier CalculateTier(int rankPoints);
    static int GetTierThreshold(RankTier tier);
    static std::string GetTierName(RankTier tier);
    static std::string GetTierIcon(RankTier tier);
    
    // Season management
    static void StartNewSeason();
    static void ApplySeasonRewards(int playerID, RankTier finalTier);
    static int CalculatePlacementRank(int wins, int losses);
    
    // Leaderboards
    struct LeaderboardEntry {
        int playerID;
        std::string playerName;
        int rankPoints;
        RankTier tier;
        int wins;
        int losses;
        float winRate;
    };
    
    static std::vector<LeaderboardEntry> GetTopPlayers(int count = 100);
    static int GetPlayerRanking(int playerID);
};

} // namespace ArenaFighter