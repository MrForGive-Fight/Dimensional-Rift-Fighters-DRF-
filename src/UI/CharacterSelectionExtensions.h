#pragma once

#include "CharacterSelection.h"
#include <fstream>
#include <sstream>
#include <set>
#include <algorithm>

namespace ArenaFighter {

// Team preset system
struct TeamPreset {
    std::string m_name;
    std::vector<std::string> m_characterIds;
    std::string m_gameMode;
    
    TeamPreset() = default;
    TeamPreset(const std::string& name, const std::vector<std::string>& chars, const std::string& mode)
        : m_name(name), m_characterIds(chars), m_gameMode(mode) {}
};

// Character animation states for visual feedback
enum class CharacterAnimState {
    Idle,
    Selected,
    Hover,
    Locked,
    Unlocking
};

// Achievement system for character collection
struct CharacterAchievement {
    std::string m_id;
    std::string m_name;
    std::string m_description;
    std::function<bool(const std::vector<CharacterData*>&)> m_condition;
    int m_rewardCurrency;
    bool m_unlocked;
};

// Character statistics tracker
struct CharacterStats {
    std::string m_characterId;
    int m_timesSelected;
    int m_battlesWon;
    int m_battlesLost;
    float m_winRate;
    
    CharacterStats() : m_timesSelected(0), m_battlesWon(0), m_battlesLost(0), m_winRate(0.0f) {}
    
    void updateWinRate() {
        int totalBattles = m_battlesWon + m_battlesLost;
        m_winRate = totalBattles > 0 ? static_cast<float>(m_battlesWon) / totalBattles : 0.0f;
    }
};

class CharacterSelectionExtensions {
public:
    // Save/Load presets using simple text format
    static bool savePreset(const TeamPreset& preset, const std::string& filepath);
    static bool loadPreset(TeamPreset& preset, const std::string& filepath);
    
    // Load character data from custom format
    static bool loadCharactersFromFile(const std::string& filepath, 
        std::vector<std::unique_ptr<CharacterData>>& characterRoster);
    
    // Save character data to file
    static bool saveCharactersToFile(const std::string& filepath,
        const std::vector<std::unique_ptr<CharacterData>>& characterRoster);
};

class AchievementManager {
private:
    std::vector<CharacterAchievement> m_achievements;
    
public:
    AchievementManager();
    
    void initializeAchievements();
    std::vector<CharacterAchievement> checkAchievements(const std::vector<CharacterData*>& data);
    const std::vector<CharacterAchievement>& getAllAchievements() const { return m_achievements; }
    void unlockAchievement(const std::string& achievementId);
    bool isAchievementUnlocked(const std::string& achievementId) const;
};

class CharacterStatsManager {
private:
    std::map<std::string, CharacterStats> m_statsMap;
    std::string m_statsFilePath;
    
public:
    CharacterStatsManager(const std::string& statsFile = "character_stats.dat");
    ~CharacterStatsManager();
    
    void recordSelection(const std::string& characterId);
    void recordBattleResult(const std::string& characterId, bool won);
    CharacterStats getStats(const std::string& characterId) const;
    std::vector<std::pair<std::string, int>> getMostUsedCharacters(int topN = 5) const;
    std::vector<std::pair<std::string, float>> getHighestWinRates(int topN = 5) const;
    
    void saveStats();
    void loadStats();
    
private:
    void ensureCharacterExists(const std::string& characterId);
};

// Helper class for character filtering and sorting
class CharacterFilter {
public:
    enum class SortBy {
        Name,
        Category,
        Level,
        Attack,
        Defense,
        Special,
        Rarity,
        WinRate,
        TimesSelected
    };
    
    enum class FilterBy {
        None,
        Category,
        Owned,
        Locked,
        Rarity,
        MinLevel,
        MaxLevel
    };
    
    struct FilterCriteria {
        FilterBy m_filterType;
        std::string m_filterValue;
        int m_filterIntValue;
        bool m_filterBoolValue;
        
        FilterCriteria() : m_filterType(FilterBy::None), m_filterIntValue(0), m_filterBoolValue(false) {}
    };
    
    static void sortCharacters(std::vector<CharacterData*>& characters, SortBy sortBy, bool ascending = true);
    static void filterCharacters(std::vector<CharacterData*>& characters, const std::vector<FilterCriteria>& criteria);
    static std::vector<CharacterData*> searchCharacters(
        const std::vector<CharacterData*>& characters, 
        const std::string& searchTerm);
};

// Character unlock animation helper
class UnlockAnimationHelper {
private:
    struct UnlockAnimation {
        std::string m_characterId;
        float m_timer;
        float m_duration;
        bool m_isPlaying;
        std::function<void()> m_onComplete;
    };
    
    std::map<std::string, UnlockAnimation> m_animations;
    
public:
    void startUnlockAnimation(const std::string& characterId, float duration, std::function<void()> onComplete);
    void update(float deltaTime);
    bool isAnimating(const std::string& characterId) const;
    float getAnimationProgress(const std::string& characterId) const;
    void stopAnimation(const std::string& characterId);
};

} // namespace ArenaFighter