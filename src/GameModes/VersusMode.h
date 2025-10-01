#pragma once

#include "GameMode.h"
#include <array>

namespace ArenaFighter {

// Versus mode character slot configuration
enum class CharacterSlotMode {
    Single,     // 1 character per player (certain weeks)
    Triple      // 3 characters per player (other weeks)
};

// Versus mode settings
struct VersusSettings {
    bool bestOf3 = true;           // Best of 3 or 5
    bool infiniteTime = false;     // Disable timer
    bool stageHazards = true;      // Enable stage interactions
    bool superMoves = true;        // Allow ultimate skills
    float damageMultiplier = 1.0f; // Damage scaling
    CharacterSlotMode slotMode = CharacterSlotMode::Triple; // Character slots
    bool allowCharacterSwitch = true; // Allow switching during match
    float switchCooldown = 3.0f;    // Cooldown between switches
};

class VersusMode : public GameMode {
private:
    VersusSettings m_settings;
    
    // Player ready states
    std::array<bool, 2> m_playerReady;
    
    // Character management
    std::map<int, std::vector<std::shared_ptr<CharacterBase>>> m_playerCharacters;
    std::map<int, int> m_activeCharacterIndex;
    std::map<int, float> m_switchCooldowns;
    
    // Special versus mode features
    bool m_doubleKO;
    float m_suddenDeathTimer;
    
    // Round statistics
    struct PlayerStats {
        int perfectRounds = 0;      // Rounds won with full health
        int firstHits = 0;          // First hit of the round
        float avgRoundTime = 0.0f;  // Average time to win
        int maxComboLength = 0;     // Longest combo
    };
    std::array<PlayerStats, 2> m_playerStats;
    
    // Versus-specific methods
    void checkDoubleKO();
    void handleSuddenDeath(float deltaTime);
    void updatePlayerStatistics();
    void showVersusIntro();
    void showVictoryScreen(int winner);
    
    // Character management
    void setupCharacterSlots();
    void switchCharacter(int playerId, int direction);
    bool canSwitchCharacter(int playerId) const;
    void updateSwitchCooldowns(float deltaTime);

public:
    VersusMode(const VersusSettings& settings = VersusSettings());
    virtual ~VersusMode() = default;
    
    // Override base methods
    void initialize() override;
    void update(float deltaTime) override;
    void enterState(MatchState newState) override;
    
    // Ready system
    void setPlayerReady(int playerId, bool ready);
    bool isPlayerReady(int playerId) const;
    bool areAllPlayersReady() const;
    
    // Settings management
    void setSettings(const VersusSettings& settings) { m_settings = settings; }
    VersusSettings getSettings() const { return m_settings; }
    
    // Character slot management
    void setPlayerCharacters(int playerId, const std::vector<std::shared_ptr<CharacterBase>>& characters);
    std::vector<std::shared_ptr<CharacterBase>> getPlayerCharacters(int playerId) const;
    std::shared_ptr<CharacterBase> getActiveCharacter(int playerId) const;
    int getActiveCharacterIndex(int playerId) const;
    void requestCharacterSwitch(int playerId, int direction = 1);
    
    // Week-based slot configuration
    void setWeeklySlotMode(CharacterSlotMode mode);
    CharacterSlotMode getCurrentSlotMode() const { return m_settings.slotMode; }
    int getRequiredCharacterCount() const;
    
    // Statistics
    PlayerStats getPlayerStats(int playerId) const;
    void resetStatistics();
    
    // Mode specific implementations
    std::string getModeName() const override { return "Versus"; }
    GameModeType getModeType() const override { return GameModeType::Versus; }
    bool supportsOnline() const override { return true; }
    int getMinPlayers() const override { return 2; }
    int getMaxPlayers() const override { return 2; }
    
    // Versus-specific features
    void enableSuddenDeath();
    bool isInSuddenDeath() const { return m_suddenDeathTimer > 0; }
};

} // namespace ArenaFighter