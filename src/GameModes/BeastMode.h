#pragma once

#include "GameMode.h"
#include <random>
#include <map>

namespace ArenaFighter {

// Beast mode specific configuration
struct BeastModeConfig : public MatchConfig {
    float beastHealthMultiplier = 5.0f;    // Beast has 5x health
    float beastDamageMultiplier = 2.0f;    // Beast deals 2x damage
    float beastSizeMultiplier = 2.5f;      // Beast is 2.5x larger
    float beastManaMultiplier = 3.0f;      // Beast has 3x mana
    float beastTransformTime = 5.0f;       // Time for transformation
    float beastModeDuration = 120.0f;      // 2 minutes per round
    bool allowBeastSwitching = true;       // Can beast change mid-match
};

// Beast mode state
enum class BeastModeState {
    SelectingBeast,    // Choosing who becomes the beast
    Transforming,      // Beast transformation animation
    Fighting,          // Active combat phase
    BeastDefeated,     // Beast was defeated
    TimeExpired        // Time limit reached
};

// Player damage tracking
struct DamageTracker {
    int playerId;
    float totalDamage;
    float highestCombo;
    int hitCount;
};

// Beast Mode: One player becomes a giant beast, others fight to deal most damage
class BeastMode : public GameMode {
private:
    BeastModeConfig m_beastConfig;
    BeastModeState m_beastState;
    
    // Beast management
    int m_currentBeastId;
    std::shared_ptr<CharacterBase> m_beastCharacter;
    std::vector<std::shared_ptr<CharacterBase>> m_hunters;
    
    // Damage tracking
    std::map<int, DamageTracker> m_damageTrackers;
    float m_totalBeastDamage;
    
    // Round state
    float m_beastTimer;
    float m_transformTimer;
    std::mt19937 m_rng;
    
    // Beast transformation
    void selectBeast();
    void transformToBeast(int playerId);
    void revertFromBeast();
    void applyBeastModifiers();
    void removeBeastModifiers();
    
    // Damage tracking
    void trackDamage(int attackerId, float damage);
    void updateDamageRankings();
    int getTopDamageDealer() const;
    std::vector<DamageTracker> getSortedDamageRankings() const;
    
    // Beast AI (if no player is beast)
    void updateBeastAI(float deltaTime);
    void performBeastAttack();
    
    // Visual effects
    void playTransformationEffect();
    void updateBeastVisuals();
    
    // Round management override
    virtual void startRound() override;
    virtual void endRound(int winnerId, WinCondition condition) override;
    virtual bool checkWinConditions() override;
    virtual int calculateRoundWinner() override;

public:
    explicit BeastMode(const BeastModeConfig& config);
    virtual ~BeastMode() = default;
    
    // Game mode implementation
    virtual void initialize() override;
    virtual void update(float deltaTime) override;
    virtual void render() override;
    virtual void shutdown() override;
    
    // State management
    virtual void setState(MatchState state) override;
    void setBeastState(BeastModeState state);
    BeastModeState getBeastState() const { return m_beastState; }
    
    // Player management
    virtual void addPlayer(std::shared_ptr<CharacterBase> character) override;
    virtual void removePlayer(int playerId) override;
    
    // Input handling
    virtual void handleInput(int playerId, const InputCommand& input) override;
    
    // Beast mode specific
    void forceBeastSelection(int playerId);
    bool isBeast(int playerId) const { return playerId == m_currentBeastId; }
    int getCurrentBeastId() const { return m_currentBeastId; }
    float getBeastHealthPercentage() const;
    
    // Damage statistics
    float getPlayerDamageDealt(int playerId) const;
    DamageTracker getPlayerStats(int playerId) const;
    std::vector<DamageTracker> getAllPlayerStats() const;
    
    // Configuration
    void setBeastConfig(const BeastModeConfig& config) { m_beastConfig = config; }
    BeastModeConfig getBeastConfig() const { return m_beastConfig; }
    
    // Game mode properties
    virtual std::string getModeName() const override { return "Beast Mode"; }
    virtual GameModeType getModeType() const override { return GameModeType::BeastMode; }
    virtual bool supportsOnline() const override { return true; }
    virtual int getMinPlayers() const override { return 2; }
    virtual int getMaxPlayers() const override { return 8; }
};

} // namespace ArenaFighter