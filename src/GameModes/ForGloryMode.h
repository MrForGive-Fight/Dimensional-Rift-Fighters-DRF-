#pragma once

#include "GameMode.h"

namespace ArenaFighter {

// For Glory configuration
struct ForGloryConfig : public MatchConfig {
    int heroCount = 2;                  // Number of heroes per player
    bool allowHeroSwitching = false;    // No switching in For Glory mode
    float heroSwitchCooldown = 0.0f;    // Switching disabled
    float roundTime = 99.0f;            // Standard round time
    bool bestOf3 = true;                // Best of 3 rounds
    bool rankedMode = true;             // Affects ranking/ELO
};

// Hero slot information
struct HeroSlot {
    std::shared_ptr<CharacterBase> character;
    bool isActive;
    bool isDead;
    float currentHealth;
    float currentMana;
};

// For Glory Mode: 2 hero mode without switching
class ForGloryMode : public GameMode {
private:
    ForGloryConfig m_forGloryConfig;
    
    // Hero management
    std::map<int, std::vector<HeroSlot>> m_playerHeroes;  // playerId -> heroes
    std::map<int, int> m_activeHeroIndex;                 // playerId -> active hero
    
    // Round tracking
    std::vector<std::pair<int, int>> m_heroesLostPerRound; // Round -> (P1 lost, P2 lost)
    int m_currentHeroRound;
    
    // Glory points (special scoring)
    std::map<int, int> m_gloryPoints;
    
    // Hero management
    void setupHeroes();
    void switchToNextHero(int playerId);
    bool hasAliveHeroes(int playerId) const;
    int getAliveHeroCount(int playerId) const;
    std::shared_ptr<CharacterBase> getActiveHero(int playerId) const;
    
    // Glory point system
    void awardGloryPoints(int playerId, int points);
    void calculateRoundGloryPoints();
    
    // Round management overrides
    virtual void startRound() override;
    virtual void endRound(int winnerId, WinCondition condition) override;
    virtual bool checkWinConditions() override;
    virtual int calculateRoundWinner() override;
    
    // Hero death handling
    void onHeroDeath(int playerId, int heroIndex);
    void checkHeroTransition(int playerId);

public:
    explicit ForGloryMode(const ForGloryConfig& config);
    virtual ~ForGloryMode() = default;
    
    // Game mode implementation
    virtual void initialize() override;
    virtual void update(float deltaTime) override;
    virtual void render() override;
    virtual void shutdown() override;
    
    // Player management
    virtual void addPlayer(std::shared_ptr<CharacterBase> character) override;
    virtual void removePlayer(int playerId) override;
    
    // Hero management
    void setPlayerHeroes(int playerId, const std::vector<std::shared_ptr<CharacterBase>>& heroes);
    std::vector<HeroSlot> getPlayerHeroes(int playerId) const;
    int getActiveHeroIndex(int playerId) const;
    
    // Input handling
    virtual void handleInput(int playerId, const InputCommand& input) override;
    
    // Hero status
    bool isHeroDead(int playerId, int heroIndex) const;
    float getHeroHealth(int playerId, int heroIndex) const;
    float getHeroMana(int playerId, int heroIndex) const;
    
    // Glory points
    int getGloryPoints(int playerId) const;
    std::map<int, int> getAllGloryPoints() const { return m_gloryPoints; }
    
    // Configuration
    void setForGloryConfig(const ForGloryConfig& config) { m_forGloryConfig = config; }
    ForGloryConfig getForGloryConfig() const { return m_forGloryConfig; }
    
    // Game mode properties
    virtual std::string getModeName() const override { return "For Glory"; }
    virtual GameModeType getModeType() const override { return GameModeType::ForGlory; }
    virtual bool supportsOnline() const override { return true; }
    virtual int getMinPlayers() const override { return 2; }
    virtual int getMaxPlayers() const override { return 2; }
    
    // Match statistics
    std::vector<std::pair<int, int>> getHeroLossHistory() const { return m_heroesLostPerRound; }
    int getTotalHeroesLost(int playerId) const;
};

} // namespace ArenaFighter