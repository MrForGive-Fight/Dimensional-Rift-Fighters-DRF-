#include "CharacterSelectionExtensions.h"
#include <iostream>
#include <chrono>
#include <ctime>

namespace ArenaFighter {

// CharacterSelectionExtensions Implementation
bool CharacterSelectionExtensions::savePreset(const TeamPreset& preset, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple text format: name|gameMode|char1,char2,char3...
    file << preset.m_name << "|" << preset.m_gameMode << "|";
    for (size_t i = 0; i < preset.m_characterIds.size(); ++i) {
        file << preset.m_characterIds[i];
        if (i < preset.m_characterIds.size() - 1) {
            file << ",";
        }
    }
    file << std::endl;
    
    file.close();
    return true;
}

bool CharacterSelectionExtensions::loadPreset(TeamPreset& preset, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        
        // Parse name
        if (!std::getline(iss, token, '|')) {
            return false;
        }
        preset.m_name = token;
        
        // Parse game mode
        if (!std::getline(iss, token, '|')) {
            return false;
        }
        preset.m_gameMode = token;
        
        // Parse character IDs
        if (!std::getline(iss, token)) {
            return false;
        }
        
        preset.m_characterIds.clear();
        std::istringstream charStream(token);
        std::string charId;
        while (std::getline(charStream, charId, ',')) {
            preset.m_characterIds.push_back(charId);
        }
    }
    
    file.close();
    return true;
}

bool CharacterSelectionExtensions::loadCharactersFromFile(const std::string& filepath, 
    std::vector<std::unique_ptr<CharacterData>>& characterRoster) {
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    characterRoster.clear();
    std::string line;
    
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        std::istringstream iss(line);
        auto character = std::make_unique<CharacterData>();
        
        // Format: id|name|category|rarity|level|attack|defense|special|owned|locked
        std::string token;
        
        if (!std::getline(iss, character->m_id, '|')) continue;
        if (!std::getline(iss, character->m_name, '|')) continue;
        if (!std::getline(iss, character->m_category, '|')) continue;
        if (!std::getline(iss, character->m_rarity, '|')) continue;
        
        if (std::getline(iss, token, '|')) character->m_level = std::stoi(token);
        if (std::getline(iss, token, '|')) character->m_attack = std::stoi(token);
        if (std::getline(iss, token, '|')) character->m_defense = std::stoi(token);
        if (std::getline(iss, token, '|')) character->m_special = std::stoi(token);
        if (std::getline(iss, token, '|')) character->m_owned = (token == "1");
        if (std::getline(iss, token, '|')) character->m_locked = (token == "1");
        
        characterRoster.push_back(std::move(character));
    }
    
    file.close();
    return !characterRoster.empty();
}

bool CharacterSelectionExtensions::saveCharactersToFile(const std::string& filepath,
    const std::vector<std::unique_ptr<CharacterData>>& characterRoster) {
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# DFR Character Roster Data" << std::endl;
    file << "# Format: id|name|category|rarity|level|attack|defense|special|owned|locked" << std::endl;
    
    for (const auto& character : characterRoster) {
        file << character->m_id << "|"
             << character->m_name << "|"
             << character->m_category << "|"
             << character->m_rarity << "|"
             << character->m_level << "|"
             << character->m_attack << "|"
             << character->m_defense << "|"
             << character->m_special << "|"
             << (character->m_owned ? "1" : "0") << "|"
             << (character->m_locked ? "1" : "0") << std::endl;
    }
    
    file.close();
    return true;
}

// AchievementManager Implementation
AchievementManager::AchievementManager() {
    initializeAchievements();
}

void AchievementManager::initializeAchievements() {
    m_achievements.clear();
    
    // DFR-specific achievements based on 7 character categories
    m_achievements.push_back({
        "system_collector", "System Specialist", "Collect 5 System category characters",
        [](const std::vector<CharacterData*>& data) {
            return std::count_if(data.begin(), data.end(),
                [](const CharacterData* c) { return c->m_category == "System" && c->m_owned; }) >= 5;
        },
        100, false
    });
    
    m_achievements.push_back({
        "gods_heroes_collector", "Divine Champion", "Collect 5 Gods/Heroes category characters",
        [](const std::vector<CharacterData*>& data) {
            return std::count_if(data.begin(), data.end(),
                [](const CharacterData* c) { return c->m_category == "GodsHeroes" && c->m_owned; }) >= 5;
        },
        100, false
    });
    
    m_achievements.push_back({
        "murim_master", "Murim Master", "Collect 10 Murim category characters",
        [](const std::vector<CharacterData*>& data) {
            return std::count_if(data.begin(), data.end(),
                [](const CharacterData* c) { return c->m_category == "Murim" && c->m_owned; }) >= 10;
        },
        200, false
    });
    
    m_achievements.push_back({
        "cultivation_expert", "Cultivation Expert", "Collect 8 Cultivation category characters",
        [](const std::vector<CharacterData*>& data) {
            return std::count_if(data.begin(), data.end(),
                [](const CharacterData* c) { return c->m_category == "Cultivation" && c->m_owned; }) >= 8;
        },
        150, false
    });
    
    m_achievements.push_back({
        "beast_tamer", "Beast Tamer", "Collect all Animal category characters",
        [](const std::vector<CharacterData*>& data) {
            auto animalCount = std::count_if(data.begin(), data.end(),
                [](const CharacterData* c) { return c->m_category == "Animal"; });
            auto ownedAnimalCount = std::count_if(data.begin(), data.end(),
                [](const CharacterData* c) { return c->m_category == "Animal" && c->m_owned; });
            return animalCount > 0 && animalCount == ownedAnimalCount;
        },
        300, false
    });
    
    m_achievements.push_back({
        "chaos_conqueror", "Chaos Conqueror", "Unlock a Chaos category character",
        [](const std::vector<CharacterData*>& data) {
            return std::any_of(data.begin(), data.end(),
                [](const CharacterData* c) { return c->m_category == "Chaos" && c->m_owned; });
        },
        500, false
    });
    
    m_achievements.push_back({
        "complete_roster", "Ultimate Fighter", "Collect 50 different characters",
        [](const std::vector<CharacterData*>& data) {
            return std::count_if(data.begin(), data.end(),
                [](const CharacterData* c) { return c->m_owned; }) >= 50;
        },
        1000, false
    });
}

std::vector<CharacterAchievement> AchievementManager::checkAchievements(
    const std::vector<CharacterData*>& data) {
    
    std::vector<CharacterAchievement> newlyUnlocked;
    
    for (auto& achievement : m_achievements) {
        if (!achievement.m_unlocked && achievement.m_condition(data)) {
            achievement.m_unlocked = true;
            newlyUnlocked.push_back(achievement);
        }
    }
    
    return newlyUnlocked;
}

void AchievementManager::unlockAchievement(const std::string& achievementId) {
    auto it = std::find_if(m_achievements.begin(), m_achievements.end(),
        [&achievementId](const CharacterAchievement& a) { return a.m_id == achievementId; });
    
    if (it != m_achievements.end()) {
        it->m_unlocked = true;
    }
}

bool AchievementManager::isAchievementUnlocked(const std::string& achievementId) const {
    auto it = std::find_if(m_achievements.begin(), m_achievements.end(),
        [&achievementId](const CharacterAchievement& a) { return a.m_id == achievementId; });
    
    return it != m_achievements.end() && it->m_unlocked;
}

// CharacterStatsManager Implementation
CharacterStatsManager::CharacterStatsManager(const std::string& statsFile)
    : m_statsFilePath(statsFile) {
    loadStats();
}

CharacterStatsManager::~CharacterStatsManager() {
    saveStats();
}

void CharacterStatsManager::recordSelection(const std::string& characterId) {
    ensureCharacterExists(characterId);
    m_statsMap[characterId].m_timesSelected++;
}

void CharacterStatsManager::recordBattleResult(const std::string& characterId, bool won) {
    ensureCharacterExists(characterId);
    
    if (won) {
        m_statsMap[characterId].m_battlesWon++;
    } else {
        m_statsMap[characterId].m_battlesLost++;
    }
    
    m_statsMap[characterId].updateWinRate();
}

CharacterStats CharacterStatsManager::getStats(const std::string& characterId) const {
    auto it = m_statsMap.find(characterId);
    return it != m_statsMap.end() ? it->second : CharacterStats();
}

std::vector<std::pair<std::string, int>> CharacterStatsManager::getMostUsedCharacters(int topN) const {
    std::vector<std::pair<std::string, int>> usage;
    
    for (const auto& pair : m_statsMap) {
        usage.push_back({pair.first, pair.second.m_timesSelected});
    }
    
    std::sort(usage.begin(), usage.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    
    if (usage.size() > static_cast<size_t>(topN)) {
        usage.resize(topN);
    }
    
    return usage;
}

std::vector<std::pair<std::string, float>> CharacterStatsManager::getHighestWinRates(int topN) const {
    std::vector<std::pair<std::string, float>> winRates;
    
    for (const auto& pair : m_statsMap) {
        // Only include characters with at least 10 battles
        if (pair.second.m_battlesWon + pair.second.m_battlesLost >= 10) {
            winRates.push_back({pair.first, pair.second.m_winRate});
        }
    }
    
    std::sort(winRates.begin(), winRates.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    
    if (winRates.size() > static_cast<size_t>(topN)) {
        winRates.resize(topN);
    }
    
    return winRates;
}

void CharacterStatsManager::saveStats() {
    std::ofstream file(m_statsFilePath);
    if (!file.is_open()) return;
    
    for (const auto& pair : m_statsMap) {
        const auto& stats = pair.second;
        file << stats.m_characterId << "|"
             << stats.m_timesSelected << "|"
             << stats.m_battlesWon << "|"
             << stats.m_battlesLost << std::endl;
    }
}

void CharacterStatsManager::loadStats() {
    std::ifstream file(m_statsFilePath);
    if (!file.is_open()) return;
    
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        CharacterStats stats;
        
        std::string token;
        if (std::getline(iss, stats.m_characterId, '|')) {
            if (std::getline(iss, token, '|')) stats.m_timesSelected = std::stoi(token);
            if (std::getline(iss, token, '|')) stats.m_battlesWon = std::stoi(token);
            if (std::getline(iss, token, '|')) stats.m_battlesLost = std::stoi(token);
            
            stats.updateWinRate();
            m_statsMap[stats.m_characterId] = stats;
        }
    }
}

void CharacterStatsManager::ensureCharacterExists(const std::string& characterId) {
    if (m_statsMap.find(characterId) == m_statsMap.end()) {
        CharacterStats stats;
        stats.m_characterId = characterId;
        m_statsMap[characterId] = stats;
    }
}

// CharacterFilter Implementation
void CharacterFilter::sortCharacters(std::vector<CharacterData*>& characters, 
    SortBy sortBy, bool ascending) {
    
    auto compareFunc = [sortBy, ascending](CharacterData* a, CharacterData* b) -> bool {
        bool result = false;
        
        switch (sortBy) {
            case SortBy::Name:
                result = a->m_name < b->m_name;
                break;
            case SortBy::Category:
                result = a->m_category < b->m_category;
                break;
            case SortBy::Level:
                result = a->m_level < b->m_level;
                break;
            case SortBy::Attack:
                result = a->m_attack < b->m_attack;
                break;
            case SortBy::Defense:
                result = a->m_defense < b->m_defense;
                break;
            case SortBy::Special:
                result = a->m_special < b->m_special;
                break;
            case SortBy::Rarity:
                result = a->m_rarity < b->m_rarity;
                break;
            default:
                result = a->m_name < b->m_name;
        }
        
        return ascending ? result : !result;
    };
    
    std::sort(characters.begin(), characters.end(), compareFunc);
}

void CharacterFilter::filterCharacters(std::vector<CharacterData*>& characters, 
    const std::vector<FilterCriteria>& criteria) {
    
    characters.erase(std::remove_if(characters.begin(), characters.end(),
        [&criteria](CharacterData* character) {
            for (const auto& criterion : criteria) {
                switch (criterion.m_filterType) {
                    case FilterBy::Category:
                        if (character->m_category != criterion.m_filterValue) return true;
                        break;
                    case FilterBy::Owned:
                        if (character->m_owned != criterion.m_filterBoolValue) return true;
                        break;
                    case FilterBy::Locked:
                        if (character->m_locked != criterion.m_filterBoolValue) return true;
                        break;
                    case FilterBy::Rarity:
                        if (character->m_rarity != criterion.m_filterValue) return true;
                        break;
                    case FilterBy::MinLevel:
                        if (character->m_level < criterion.m_filterIntValue) return true;
                        break;
                    case FilterBy::MaxLevel:
                        if (character->m_level > criterion.m_filterIntValue) return true;
                        break;
                    default:
                        break;
                }
            }
            return false;
        }), characters.end());
}

std::vector<CharacterData*> CharacterFilter::searchCharacters(
    const std::vector<CharacterData*>& characters, const std::string& searchTerm) {
    
    std::vector<CharacterData*> results;
    std::string lowerSearch = searchTerm;
    std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::tolower);
    
    for (auto* character : characters) {
        std::string lowerName = character->m_name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        
        if (lowerName.find(lowerSearch) != std::string::npos) {
            results.push_back(character);
        }
    }
    
    return results;
}

// UnlockAnimationHelper Implementation
void UnlockAnimationHelper::startUnlockAnimation(const std::string& characterId, 
    float duration, std::function<void()> onComplete) {
    
    UnlockAnimation anim;
    anim.m_characterId = characterId;
    anim.m_timer = 0.0f;
    anim.m_duration = duration;
    anim.m_isPlaying = true;
    anim.m_onComplete = onComplete;
    
    m_animations[characterId] = anim;
}

void UnlockAnimationHelper::update(float deltaTime) {
    for (auto& pair : m_animations) {
        auto& anim = pair.second;
        
        if (anim.m_isPlaying) {
            anim.m_timer += deltaTime;
            
            if (anim.m_timer >= anim.m_duration) {
                anim.m_isPlaying = false;
                
                if (anim.m_onComplete) {
                    anim.m_onComplete();
                }
            }
        }
    }
    
    // Remove finished animations
    for (auto it = m_animations.begin(); it != m_animations.end();) {
        if (!it->second.m_isPlaying) {
            it = m_animations.erase(it);
        } else {
            ++it;
        }
    }
}

bool UnlockAnimationHelper::isAnimating(const std::string& characterId) const {
    auto it = m_animations.find(characterId);
    return it != m_animations.end() && it->second.m_isPlaying;
}

float UnlockAnimationHelper::getAnimationProgress(const std::string& characterId) const {
    auto it = m_animations.find(characterId);
    if (it != m_animations.end() && it->second.m_duration > 0.0f) {
        return std::min(1.0f, it->second.m_timer / it->second.m_duration);
    }
    return 0.0f;
}

void UnlockAnimationHelper::stopAnimation(const std::string& characterId) {
    auto it = m_animations.find(characterId);
    if (it != m_animations.end()) {
        it->second.m_isPlaying = false;
    }
}

} // namespace ArenaFighter