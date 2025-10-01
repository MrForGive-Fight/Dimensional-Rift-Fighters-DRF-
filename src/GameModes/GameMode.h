#pragma once

#include <memory>
#include <vector>
#include <functional>
#include "../Characters/CharacterBase.h"
#include "../Combat/CombatSystem.h"
#include "../Network/NetworkManager.h"
#include "../Physics/PhysicsEngine.h"
#include "../UI/GameModeUI.h"

namespace ArenaFighter {

// Forward declarations
class CharacterBase;
class CombatSystem;

// Game mode types
enum class GameModeType {
    Versus,           // Standard 1v1 or 3v3
    BeastMode,        // One player becomes giant beast
    DeathMatch,       // 8 player FFA with items
    ForGlory,         // 2 hero mode without switching
    DimensionalRift,  // Dungeon mode with AI companions
    Tournament,       // Player-created tournaments
    Survival,         // Wave-based survival
    Training          // Practice mode
};

// Match state enumeration
enum class MatchState {
    PreMatch,      // Character selection/loading
    RoundStart,    // Round intro animations
    InProgress,    // Active gameplay
    RoundEnd,      // Round victory/loss
    MatchEnd,      // Match complete
    Paused         // Game paused
};

// Win condition types
enum class WinCondition {
    Knockout,      // Reduce opponent's health to 0
    TimeOut,       // Timer reaches 0
    Survival,      // Survive waves (survival mode)
    Score,         // Reach target score
    BeastDamage,   // Most damage to beast (Beast Mode)
    Tournament,    // Bracket progression (Tournament Mode)
    LastStanding   // Last player alive (Death Match)
};

// Round result structure
struct RoundResult {
    int winnerId;
    WinCondition winType;
    float remainingHealth;
    float remainingMana;
    int maxCombo;
    float damageDealt;
    float timeTaken;
};

// Match configuration
struct MatchConfig {
    int roundsToWin = 2;        // Best of X rounds
    float roundTime = 99.0f;    // Seconds per round
    bool infiniteTime = false;  // Disable timer
    bool allowPause = true;     // Allow pausing
    int maxPlayers = 2;         // Player count
};

// Base game mode class
class GameMode {
protected:
    // Core systems
    std::shared_ptr<CombatSystem> m_combatSystem;
    std::shared_ptr<PhysicsEngine> m_physicsEngine;
    std::shared_ptr<NetworkManager> m_networkManager;
    std::shared_ptr<GameModeUI> m_gameUI;
    
    // Match state
    MatchState m_currentState;
    MatchConfig m_config;
    std::vector<std::shared_ptr<CharacterBase>> m_players;
    std::vector<RoundResult> m_roundResults;
    
    // Timing
    float m_roundTimer;
    float m_stateTimer;
    int m_currentRound;
    
    // Callbacks
    std::function<void(int)> m_onRoundEnd;
    std::function<void(int)> m_onMatchEnd;
    std::function<void()> m_onPause;
    
    // State transition helpers
    virtual void enterState(MatchState newState);
    virtual void exitState(MatchState oldState);
    
    // Round management
    virtual void startRound();
    virtual void endRound(int winnerId, WinCondition condition);
    virtual bool checkWinConditions();
    virtual int calculateRoundWinner();
    
    // Player management
    virtual void spawnPlayers();
    virtual void resetPlayerPositions();
    virtual void resetPlayerStats();

public:
    GameMode(const MatchConfig& config);
    virtual ~GameMode() = default;
    
    // Core game loop
    virtual void initialize();
    virtual void update(float deltaTime);
    virtual void render();
    virtual void shutdown();
    
    // Player management
    virtual void addPlayer(std::shared_ptr<CharacterBase> character);
    virtual void removePlayer(int playerId);
    virtual std::shared_ptr<CharacterBase> getPlayer(int index) const;
    virtual int getPlayerCount() const { return static_cast<int>(m_players.size()); }
    
    // State management
    virtual void setState(MatchState state);
    virtual MatchState getState() const { return m_currentState; }
    virtual bool isMatchActive() const;
    
    // Input handling
    virtual void handleInput(int playerId, const InputCommand& input);
    virtual void pauseGame();
    virtual void resumeGame();
    
    // Match flow
    virtual void startMatch();
    virtual void endMatch();
    virtual void restartMatch();
    virtual void quitMatch();
    
    // Results and statistics
    virtual RoundResult getCurrentRoundResult() const;
    virtual std::vector<RoundResult> getMatchResults() const { return m_roundResults; }
    virtual int getWinsForPlayer(int playerId) const;
    
    // Configuration
    virtual void setConfig(const MatchConfig& config) { m_config = config; }
    virtual MatchConfig getConfig() const { return m_config; }
    
    // UI interaction
    virtual void updateUI();
    virtual std::shared_ptr<GameModeUI> getUI() const { return m_gameUI; }
    
    // Callbacks
    void setRoundEndCallback(std::function<void(int)> callback) { m_onRoundEnd = callback; }
    void setMatchEndCallback(std::function<void(int)> callback) { m_onMatchEnd = callback; }
    void setPauseCallback(std::function<void()> callback) { m_onPause = callback; }
    
    // Game mode specific methods (override in derived classes)
    virtual std::string getModeName() const = 0;
    virtual GameModeType getModeType() const = 0;
    virtual bool supportsOnline() const = 0;
    virtual int getMinPlayers() const = 0;
    virtual int getMaxPlayers() const = 0;
};

} // namespace ArenaFighter