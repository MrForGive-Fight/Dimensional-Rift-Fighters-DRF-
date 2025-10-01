#include "RankingSystem.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include "../Characters/CharacterFactory.h"

namespace ArenaFighter {

// CharacterRankData implementation
CharacterRankData::CharacterRankData()
    : m_characterId("")
    , m_playerId("")
    , m_wins(0)
    , m_losses(0)
    , m_totalMatches(0)
    , m_winRate(0.0f)
    , m_weeklyRank(0)
    , m_isWeeklyChampion(false)
    , m_monthlyRank(0)
    , m_allTimeRank(0)
    , m_maxCombo(0)
    , m_avgDamageDealt(0.0f)
    , m_avgMatchTime(0.0f) {
}

void CharacterRankData::updateWinRate() {
    if (m_totalMatches > 0) {
        m_winRate = static_cast<float>(m_wins) / static_cast<float>(m_totalMatches);
    } else {
        m_winRate = 0.0f;
    }
}

float CharacterRankData::getScore() const {
    // Grand Chase-inspired scoring: Win rate + activity bonus
    float score = m_winRate * 1000.0f;  // Base score from win rate
    
    // Activity bonus (encourages playing)
    float activityBonus = std::min(m_totalMatches * 2.0f, 200.0f);
    score += activityBonus;
    
    // Win streak consideration
    if (m_wins > 10) {
        score += m_wins * 5.0f;
    }
    
    // Performance metrics bonus
    score += m_maxCombo * 0.5f;
    score += m_avgDamageDealt * 0.1f;
    
    return score;
}

// WeeklyRankingCycle implementation
bool WeeklyRankingCycle::isInCycle(const std::chrono::system_clock::time_point& time) const {
    return m_isActive && time >= m_startDate && time < m_endDate;
}

// RankingSystem implementation
RankingSystem::RankingSystem()
    : m_currentMonth(1)
    , m_top100Cutoff(100)
    , m_minMatchesForRanking(10) {
    
    // Initialize current weekly cycle
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_now = std::localtime(&time_t_now);
    
    // Find current week of month (1-4)
    int dayOfMonth = tm_now->tm_mday;
    m_currentWeeklyCycle.m_weekNumber = ((dayOfMonth - 1) / 7) + 1;
    if (m_currentWeeklyCycle.m_weekNumber > 4) {
        m_currentWeeklyCycle.m_weekNumber = 4;
    }
    
    // Set cycle dates
    m_currentWeeklyCycle.m_startDate = now;
    m_currentWeeklyCycle.m_endDate = now + std::chrono::hours(24 * 7);
    m_currentWeeklyCycle.m_isActive = true;
    
    m_currentMonth = tm_now->tm_mon + 1;
}

RankingSystem::~RankingSystem() {
    saveRankings();
}

bool RankingSystem::initialize(const std::string& dataPath) {
    loadRankings();
    checkAndUpdateWeeklyCycle();
    return true;
}

void RankingSystem::shutdown() {
    saveRankings();
}

void RankingSystem::recordMatchResult(
    const std::string& winnerId,
    const std::string& loserId,
    const std::string& characterIdWinner,
    const std::string& characterIdLoser,
    int winnerCombo,
    float damageDealt,
    float matchTime) {
    
    // Update winner stats
    auto& winnerData = m_playerRankings[winnerId][characterIdWinner];
    winnerData.m_playerId = winnerId;
    winnerData.m_characterId = characterIdWinner;
    winnerData.m_wins++;
    winnerData.m_totalMatches++;
    winnerData.m_maxCombo = std::max(winnerData.m_maxCombo, winnerCombo);
    
    // Update averages
    float totalDamage = winnerData.m_avgDamageDealt * (winnerData.m_totalMatches - 1) + damageDealt;
    winnerData.m_avgDamageDealt = totalDamage / winnerData.m_totalMatches;
    
    float totalTime = winnerData.m_avgMatchTime * (winnerData.m_totalMatches - 1) + matchTime;
    winnerData.m_avgMatchTime = totalTime / winnerData.m_totalMatches;
    
    winnerData.updateWinRate();
    
    // Update loser stats
    auto& loserData = m_playerRankings[loserId][characterIdLoser];
    loserData.m_playerId = loserId;
    loserData.m_characterId = characterIdLoser;
    loserData.m_losses++;
    loserData.m_totalMatches++;
    loserData.updateWinRate();
    
    // Update leaderboards for both characters
    updateLeaderboards(characterIdWinner);
    updateLeaderboards(characterIdLoser);
}

std::vector<LeaderboardEntry> RankingSystem::getCharacterLeaderboard(
    const std::string& characterId,
    RankingPeriod period,
    int topN) const {
    
    std::vector<LeaderboardEntry> result;
    
    const auto* rankings = &m_weeklyRankings;
    if (period == RankingPeriod::Monthly) {
        rankings = &m_monthlyRankings;
    } else if (period == RankingPeriod::AllTime) {
        rankings = &m_allTimeRankings;
    }
    
    auto it = rankings->find(characterId);
    if (it != rankings->end()) {
        const auto& leaderboard = it->second;
        int count = std::min(topN, static_cast<int>(leaderboard.size()));
        result.reserve(count);
        
        for (int i = 0; i < count; i++) {
            result.push_back(leaderboard[i]);
        }
    }
    
    return result;
}

CharacterRankData RankingSystem::getPlayerCharacterRank(
    const std::string& playerId,
    const std::string& characterId,
    RankingPeriod period) const {
    
    auto playerIt = m_playerRankings.find(playerId);
    if (playerIt != m_playerRankings.end()) {
        auto charIt = playerIt->second.find(characterId);
        if (charIt != playerIt->second.end()) {
            return charIt->second;
        }
    }
    
    return CharacterRankData();
}

std::vector<std::string> RankingSystem::getPlayerTop5Characters(
    const std::string& playerId) const {
    
    std::vector<std::pair<float, std::string>> scoredChars;
    
    auto playerIt = m_playerRankings.find(playerId);
    if (playerIt != m_playerRankings.end()) {
        for (const auto& [charId, rankData] : playerIt->second) {
            if (rankData.m_totalMatches >= m_minMatchesForRanking) {
                scoredChars.push_back({rankData.getScore(), charId});
            }
        }
    }
    
    // Sort by score descending
    std::sort(scoredChars.begin(), scoredChars.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    std::vector<std::string> result;
    int count = std::min(5, static_cast<int>(scoredChars.size()));
    for (int i = 0; i < count; i++) {
        result.push_back(scoredChars[i].second);
    }
    
    return result;
}

void RankingSystem::checkAndUpdateWeeklyCycle() {
    auto now = std::chrono::system_clock::now();
    
    if (!m_currentWeeklyCycle.isInCycle(now)) {
        finalizeWeeklyCycle();
        startNewWeeklyCycle();
    }
}

void RankingSystem::startNewWeeklyCycle() {
    // Clear weekly rankings
    m_weeklyRankings.clear();
    
    // Reset weekly ranks for all players
    for (auto& [playerId, charData] : m_playerRankings) {
        for (auto& [charId, rankData] : charData) {
            rankData.m_weeklyRank = 0;
            rankData.m_isWeeklyChampion = false;
        }
    }
    
    // Set up new cycle
    auto now = std::chrono::system_clock::now();
    m_currentWeeklyCycle.m_weekNumber++;
    
    if (m_currentWeeklyCycle.m_weekNumber > 4) {
        performMonthlyRollup();
        m_currentWeeklyCycle.m_weekNumber = 1;
    }
    
    m_currentWeeklyCycle.m_startDate = now;
    m_currentWeeklyCycle.m_endDate = now + std::chrono::hours(24 * 7);
    m_currentWeeklyCycle.m_isActive = true;
    
    if (m_onRankingsReset) {
        m_onRankingsReset(RankingPeriod::Weekly);
    }
}

void RankingSystem::finalizeWeeklyCycle() {
    // Distribute rewards for top players
    distributeRewards();
    
    // Archive weekly results
    saveRankings();
}

void RankingSystem::performMonthlyRollup() {
    // Combine 4 weeks of data into monthly rankings
    m_monthlyRankings.clear();
    
    // Aggregate all character rankings
    for (const auto& [playerId, charData] : m_playerRankings) {
        for (const auto& [charId, rankData] : charData) {
            if (rankData.m_totalMatches >= m_minMatchesForRanking) {
                LeaderboardEntry entry;
                entry.m_playerId = playerId;
                entry.m_playerName = playerId; // Would be resolved from player system
                entry.m_rankData = rankData;
                entry.m_hasWeeklyChampionBadge = rankData.m_isWeeklyChampion;
                entry.m_hasTop100Badge = rankData.m_weeklyRank > 0 && rankData.m_weeklyRank <= 100;
                
                m_monthlyRankings[charId].push_back(entry);
            }
        }
    }
    
    // Sort and assign monthly ranks
    for (auto& [charId, leaderboard] : m_monthlyRankings) {
        sortLeaderboard(leaderboard);
        assignRanks(leaderboard);
    }
    
    if (m_onRankingsReset) {
        m_onRankingsReset(RankingPeriod::Monthly);
    }
}

std::vector<RankingReward> RankingSystem::calculateRewards(
    const std::string& playerId,
    RankingPeriod period) {
    
    std::vector<RankingReward> rewards;
    
    // Check all characters for this player
    for (const auto& [charId, rankData] : m_playerRankings[playerId]) {
        if (period == RankingPeriod::Weekly) {
            // Weekly #1 Champion rewards
            if (rankData.m_isWeeklyChampion) {
                RankingReward championReward;
                championReward.m_type = RankingReward::Badge;
                championReward.m_itemId = "weekly_champion_" + charId;
                championReward.m_description = "Weekly Champion - " + charId;
                rewards.push_back(championReward);
                
                // Currency reward
                RankingReward currencyReward;
                currencyReward.m_type = RankingReward::Currency;
                currencyReward.m_amount = 10000;
                currencyReward.m_description = "Weekly Champion Bonus";
                rewards.push_back(currencyReward);
            }
            
            // Top 100 rewards
            if (rankData.m_weeklyRank > 0 && rankData.m_weeklyRank <= 100) {
                RankingReward topReward;
                topReward.m_type = RankingReward::Currency;
                topReward.m_amount = 5000 - (rankData.m_weeklyRank * 40); // Scaled by rank
                topReward.m_description = "Top 100 Weekly Reward";
                rewards.push_back(topReward);
            }
        }
    }
    
    return rewards;
}

void RankingSystem::distributeRewards() {
    for (const auto& [playerId, charData] : m_playerRankings) {
        auto rewards = calculateRewards(playerId, RankingPeriod::Weekly);
        
        for (const auto& reward : rewards) {
            if (m_onRewardEarned) {
                m_onRewardEarned(playerId, reward);
            }
        }
    }
}

bool RankingSystem::hasWeeklyChampionBadge(
    const std::string& playerId,
    const std::string& characterId) const {
    
    auto playerIt = m_playerRankings.find(playerId);
    if (playerIt != m_playerRankings.end()) {
        auto charIt = playerIt->second.find(characterId);
        if (charIt != playerIt->second.end()) {
            return charIt->second.m_isWeeklyChampion;
        }
    }
    return false;
}

bool RankingSystem::hasTop100Badge(
    const std::string& playerId,
    const std::string& characterId) const {
    
    auto playerIt = m_playerRankings.find(playerId);
    if (playerIt != m_playerRankings.end()) {
        auto charIt = playerIt->second.find(characterId);
        if (charIt != playerIt->second.end()) {
            return charIt->second.m_weeklyRank > 0 && 
                   charIt->second.m_weeklyRank <= 100;
        }
    }
    return false;
}

void RankingSystem::updateLeaderboards(const std::string& characterId) {
    // Build weekly leaderboard for this character
    std::vector<LeaderboardEntry> weeklyBoard;
    
    for (const auto& [playerId, charData] : m_playerRankings) {
        auto it = charData.find(characterId);
        if (it != charData.end() && it->second.m_totalMatches >= m_minMatchesForRanking) {
            LeaderboardEntry entry;
            entry.m_playerId = playerId;
            entry.m_playerName = playerId; // Would be resolved from player system
            entry.m_rankData = it->second;
            weeklyBoard.push_back(entry);
        }
    }
    
    sortLeaderboard(weeklyBoard);
    assignRanks(weeklyBoard);
    
    m_weeklyRankings[characterId] = weeklyBoard;
}

void RankingSystem::sortLeaderboard(std::vector<LeaderboardEntry>& leaderboard) {
    std::sort(leaderboard.begin(), leaderboard.end(),
        [](const LeaderboardEntry& a, const LeaderboardEntry& b) {
            return a.m_rankData.getScore() > b.m_rankData.getScore();
        });
}

void RankingSystem::assignRanks(std::vector<LeaderboardEntry>& leaderboard) {
    for (size_t i = 0; i < leaderboard.size(); i++) {
        leaderboard[i].m_rank = static_cast<int>(i + 1);
        
        // Update rank in player data
        auto& rankData = m_playerRankings[leaderboard[i].m_playerId]
                                         [leaderboard[i].m_rankData.m_characterId];
        rankData.m_weeklyRank = leaderboard[i].m_rank;
        rankData.m_isWeeklyChampion = (i == 0);
        
        // Update badge flags
        leaderboard[i].m_hasWeeklyChampionBadge = (i == 0);
        leaderboard[i].m_hasTop100Badge = (i < 100);
    }
}

void RankingSystem::saveRankings() {
    // Implementation would save to file/database
    // Format: JSON or binary serialization
}

void RankingSystem::loadRankings() {
    // Implementation would load from file/database
}

void RankingSystem::setRewardCallback(
    std::function<void(const std::string&, const RankingReward&)> callback) {
    m_onRewardEarned = callback;
}

void RankingSystem::setResetCallback(
    std::function<void(RankingPeriod)> callback) {
    m_onRankingsReset = callback;
}

// RankingDisplay helper implementation
std::string RankingDisplay::formatRank(int rank) {
    if (rank == 0) return "Unranked";
    if (rank == 1) return "#1 Champion";
    if (rank <= 10) return "Top 10";
    if (rank <= 100) return "Top 100";
    return std::to_string(rank);
}

std::string RankingDisplay::formatWinRate(float winRate) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << (winRate * 100.0f) << "%";
    return ss.str();
}

std::string RankingDisplay::formatBadge(const LeaderboardEntry& entry) {
    if (entry.m_hasWeeklyChampionBadge) {
        return "[Weekly #1]";
    } else if (entry.m_hasTop100Badge) {
        return "[Top 100]";
    }
    return "";
}

XMFLOAT4 RankingDisplay::getRankColor(int rank) {
    if (rank == 1) return XMFLOAT4(1.0f, 0.84f, 0.0f, 1.0f);  // Gold
    if (rank <= 10) return XMFLOAT4(0.75f, 0.75f, 0.75f, 1.0f); // Silver
    if (rank <= 100) return XMFLOAT4(0.8f, 0.5f, 0.2f, 1.0f);  // Bronze
    return XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // White
}

std::string RankingDisplay::getRankTitle(int rank) {
    if (rank == 1) return "Champion";
    if (rank <= 10) return "Master";
    if (rank <= 50) return "Expert";
    if (rank <= 100) return "Elite";
    return "Challenger";
}

} // namespace ArenaFighter