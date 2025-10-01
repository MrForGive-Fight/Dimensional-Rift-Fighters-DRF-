#pragma once

#include "GameMode.h"
#include <vector>
#include <memory>

namespace ArenaFighter {

// Survival mode configuration
struct SurvivalConfig {
    float healthRecoveryPercent = 0.25f;    // Health recovery between rounds
    float manaRecoveryPercent = 0.5f;       // Mana recovery between rounds
    bool allowHealthItems = true;           // Random health pickups
    bool progressiveDifficulty = true;      // Enemies get harder
    float difficultyScaling = 0.1f;         // 10% harder per wave
    int enemiesPerWave = 1;                 // Start with 1v1
    float waveDelay = 3.0f;                 // Seconds between waves
};

// Wave information
struct WaveInfo {
    int waveNumber;
    int enemyCount;
    float difficultyMultiplier;
    std::vector<std::string> enemyTypes;
    bool isBossWave;
};

// Survival statistics
struct SurvivalStats {
    int wavesCompleted;
    int totalKills;
    float totalDamageDealt;
    float totalDamageTaken;
    int perfectWaves;           // Waves with no damage taken
    float survivalTime;
    int highestCombo;
    int itemsCollected;
};

// Power-up types
enum class PowerUpType {
    Health,         // Restore health
    Mana,           // Restore mana
    Damage,         // Temporary damage boost
    Speed,          // Temporary speed boost
    Shield,         // Temporary damage reduction
    FullRestore     // Full health and mana
};

// Power-up item
struct PowerUp {
    PowerUpType type;
    XMFLOAT3 position;
    float lifetime;
    float value;
    bool active;
};

class SurvivalMode : public GameMode {
private:
    SurvivalConfig m_survivalConfig;
    SurvivalStats m_survivalStats;
    
    // Wave management
    int m_currentWave;
    WaveInfo m_currentWaveInfo;
    std::vector<std::shared_ptr<CharacterBase>> m_waveEnemies;
    float m_waveTimer;
    bool m_waveInProgress;
    
    // Player state
    std::shared_ptr<CharacterBase> m_player;
    float m_playerMaxHealth;
    float m_playerMaxMana;
    
    // Power-ups
    std::vector<PowerUp> m_activePowerUps;
    float m_nextPowerUpTime;
    
    // Difficulty progression
    float m_currentDifficultyMultiplier;
    
    // Wave generation
    WaveInfo generateWave(int waveNumber);
    void spawnWaveEnemies();
    void setupEnemy(std::shared_ptr<CharacterBase> enemy, float difficulty);
    
    // Power-up management
    void updatePowerUps(float deltaTime);
    void spawnPowerUp();
    void collectPowerUp(PowerUp& powerUp);
    
    // Player management
    void recoverPlayer();
    void applyPowerUpEffect(PowerUpType type, float value);
    
    // Wave progression
    void completeWave();
    void prepareNextWave();
    bool isWaveComplete() const;

public:
    SurvivalMode(const SurvivalConfig& config = SurvivalConfig());
    virtual ~SurvivalMode() = default;
    
    // Override base methods
    void initialize() override;
    void update(float deltaTime) override;
    void render() override;
    void startMatch() override;
    void endMatch() override;
    
    // Wave management
    void startWave();
    void endWave();
    int getCurrentWave() const { return m_currentWave; }
    WaveInfo getCurrentWaveInfo() const { return m_currentWaveInfo; }
    
    // Player setup
    void setSurvivalPlayer(std::shared_ptr<CharacterBase> player);
    
    // Statistics
    SurvivalStats getStats() const { return m_survivalStats; }
    void resetStats() { m_survivalStats = SurvivalStats(); }
    
    // Configuration
    void setConfig(const SurvivalConfig& config) { m_survivalConfig = config; }
    SurvivalConfig getConfig() const { return m_survivalConfig; }
    
    // Mode specific implementations
    std::string getModeName() const override { return "Survival"; }
    GameModeType getModeType() const override { return GameModeType::BeastMode; }
    bool supportsOnline() const override { return false; }
    int getMinPlayers() const override { return 1; }
    int getMaxPlayers() const override { return 1; }
    
    // Survival specific
    bool isInfiniteMode() const { return true; }
    int getHighScore() const;
    void saveHighScore();
};

} // namespace ArenaFighter