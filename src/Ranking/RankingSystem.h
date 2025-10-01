#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include <functional>

namespace ArenaFighter {

// Ranking period types
enum class RankingPeriod {
    Weekly,        // Resets every week
    Monthly,       // Resets every month  
    AllTime        // Never resets
};

// Player rank data for a character
struct CharacterRankData {
    std::string m_characterId;
    std::string m_playerId;
    int m_wins;
    int m_losses;
    int m_totalMatches;
    float m_winRate;
    int m_weeklyRank;          // 0 if not ranked, 1-100 for top 100
    bool m_isWeeklyChampion;   // #1 for the week
    int m_monthlyRank;
    int m_allTimeRank;
    int m_maxCombo;
    float m_avgDamageDealt;
    float m_avgMatchTime;
    
    CharacterRankData();
    void updateWinRate();
    float getScore() const;  // Calculate ranking score
};

// Weekly ranking cycle info
struct WeeklyRankingCycle {
    int m_weekNumber;      // 1-4 for monthly cycle
    std::chrono::system_clock::time_point m_startDate;
    std::chrono::system_clock::time_point m_endDate;
    bool m_isActive;
    
    bool isInCycle(const std::chrono::system_clock::time_point& time) const;
};

// Character leaderboard entry
struct LeaderboardEntry {
    std::string m_playerId;
    std::string m_playerName;
    CharacterRankData m_rankData;
    int m_rank;
    
    // Badge info
    bool m_hasWeeklyChampionBadge;
    bool m_hasTop100Badge;
    int m_championshipsWon;
};

// Ranking rewards
struct RankingReward {
    enum Type {
        Currency,
        Character,
        Gear,
        Title,
        Badge
    };
    
    Type m_type;
    std::string m_itemId;
    int m_amount;
    std::string m_description;
};

class RankingSystem {
private:
    // Rankings per character per period
    std::map<std::string, std::vector<LeaderboardEntry>> m_weeklyRankings;
    std::map<std::string, std::vector<LeaderboardEntry>> m_monthlyRankings;
    std::map<std::string, std::vector<LeaderboardEntry>> m_allTimeRankings;
    
    // Player's personal rankings across all characters
    std::map<std::string, std::map<std::string, CharacterRankData>> m_playerRankings;
    
    // Current cycle info
    WeeklyRankingCycle m_currentWeeklyCycle;
    int m_currentMonth;
    
    // Callbacks
    std::function<void(const std::string&, const RankingReward&)> m_onRewardEarned;
    std::function<void(RankingPeriod)> m_onRankingsReset;
    
    // Configuration
    int m_top100Cutoff;
    int m_minMatchesForRanking;
    
public:
    RankingSystem();
    ~RankingSystem();
    
    // Initialize and load rankings
    bool initialize(const std::string& dataPath);
    void shutdown();
    
    // Match result recording
    void recordMatchResult(
        const std::string& winnerId,
        const std::string& loserId,
        const std::string& characterIdWinner,
        const std::string& characterIdLoser,
        int winnerCombo,
        float damageDealt,
        float matchTime
    );
    
    // Ranking queries
    std::vector<LeaderboardEntry> getCharacterLeaderboard(
        const std::string& characterId,
        RankingPeriod period,
        int topN = 100
    ) const;
    
    CharacterRankData getPlayerCharacterRank(
        const std::string& playerId,
        const std::string& characterId,
        RankingPeriod period
    ) const;
    
    std::vector<std::string> getPlayerTop5Characters(
        const std::string& playerId
    ) const;
    
    // Weekly cycle management
    void checkAndUpdateWeeklyCycle();
    void startNewWeeklyCycle();
    void finalizeWeeklyCycle();
    
    // Monthly rollup
    void performMonthlyRollup();
    
    // Rewards
    std::vector<RankingReward> calculateRewards(
        const std::string& playerId,
        RankingPeriod period
    );
    
    void distributeRewards();
    
    // Badges and titles
    bool hasWeeklyChampionBadge(
        const std::string& playerId,
        const std::string& characterId
    ) const;
    
    bool hasTop100Badge(
        const std::string& playerId,
        const std::string& characterId
    ) const;
    
    int getChampionshipCount(
        const std::string& playerId,
        const std::string& characterId
    ) const;
    
    // Statistics
    struct GlobalStats {
        int m_totalMatches;
        std::string m_mostPlayedCharacter;
        std::string m_highestWinRateCharacter;
        float m_avgMatchLength;
        int m_totalPlayers;
    };
    
    GlobalStats getGlobalStatistics() const;
    
    // Persistence
    void saveRankings();
    void loadRankings();
    
    // Callbacks
    void setRewardCallback(std::function<void(const std::string&, const RankingReward&)> callback);
    void setResetCallback(std::function<void(RankingPeriod)> callback);
    
private:
    void updateLeaderboards(const std::string& characterId);
    void sortLeaderboard(std::vector<LeaderboardEntry>& leaderboard);
    void assignRanks(std::vector<LeaderboardEntry>& leaderboard);
    void pruneInactivePlayers();
    void migrateWeeklyToMonthly();
    std::string generateRankingKey(const std::string& playerId, const std::string& characterId) const;
};

// Ranking display helper
class RankingDisplay {
public:
    static std::string formatRank(int rank);
    static std::string formatWinRate(float winRate);
    static std::string formatBadge(const LeaderboardEntry& entry);
    static XMFLOAT4 getRankColor(int rank);
    static std::string getRankTitle(int rank);
};

} // namespace ArenaFighter