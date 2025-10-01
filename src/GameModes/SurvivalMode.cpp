#include "SurvivalMode.h"
#include <algorithm>
#include <random>

namespace ArenaFighter {

static std::mt19937 s_rng(std::random_device{}());
static std::uniform_real_distribution<float> s_dist(0.0f, 1.0f);

SurvivalMode::SurvivalMode(const SurvivalConfig& config)
    : GameMode(MatchConfig()),
      m_survivalConfig(config),
      m_currentWave(0),
      m_waveTimer(0.0f),
      m_waveInProgress(false),
      m_playerMaxHealth(BASE_HEALTH),
      m_playerMaxMana(BASE_MANA),
      m_nextPowerUpTime(0.0f),
      m_currentDifficultyMultiplier(1.0f) {
    
    // Configure for survival
    m_config.maxPlayers = 1;
    m_config.infiniteTime = true;
    m_config.allowPause = true;
    
    // Initialize stats
    m_survivalStats = SurvivalStats();
}

void SurvivalMode::initialize() {
    GameMode::initialize();
    
    // Clear any existing power-ups
    m_activePowerUps.clear();
    
    // Set up for first wave
    m_currentWave = 0;
    m_currentDifficultyMultiplier = 1.0f;
}

void SurvivalMode::update(float deltaTime) {
    if (m_currentState != MatchState::InProgress) {
        GameMode::update(deltaTime);
        return;
    }
    
    // Update survival time
    m_survivalStats.survivalTime += deltaTime;
    
    // Update physics and combat
    m_physicsEngine->update(deltaTime);
    m_combatSystem->update(deltaTime);
    
    // Update player
    if (m_player) {
        m_player->update(deltaTime);
        
        // Regenerate mana
        float currentMana = m_player->getMana();
        m_player->setMana(std::min(currentMana + MANA_REGEN_RATE * deltaTime, m_playerMaxMana));
        
        // Check if player is defeated
        if (m_player->getHealth() <= 0) {
            endMatch();
            return;
        }
    }
    
    // Update enemies
    for (auto& enemy : m_waveEnemies) {
        if (enemy && enemy->getHealth() > 0) {
            enemy->update(deltaTime);
        }
    }
    
    // Update power-ups
    updatePowerUps(deltaTime);
    
    // Wave management
    if (m_waveInProgress) {
        // Check if wave is complete
        if (isWaveComplete()) {
            completeWave();
        }
    } else {
        // Wait between waves
        m_waveTimer += deltaTime;
        if (m_waveTimer >= m_survivalConfig.waveDelay) {
            prepareNextWave();
        }
    }
    
    // Update UI
    updateUI();
}

void SurvivalMode::render() {
    GameMode::render();
    
    // Render power-ups
    for (const auto& powerUp : m_activePowerUps) {
        if (powerUp.active) {
            // Render power-up sprite/effect
            // This would be handled by the rendering system
        }
    }
    
    // Render wave information
    // Show current wave number, enemies remaining, etc.
}

void SurvivalMode::startMatch() {
    m_currentState = MatchState::InProgress;
    m_currentWave = 0;
    m_survivalStats = SurvivalStats();
    
    // Start first wave
    prepareNextWave();
}

void SurvivalMode::endMatch() {
    // Save high score
    saveHighScore();
    
    // Show results
    setState(MatchState::MatchEnd);
    
    // Call match end callback
    if (m_onMatchEnd) {
        m_onMatchEnd(0); // Player is always ID 0 in survival
    }
}

WaveInfo SurvivalMode::generateWave(int waveNumber) {
    WaveInfo wave;
    wave.waveNumber = waveNumber;
    wave.isBossWave = (waveNumber % 10 == 0); // Every 10th wave is a boss
    
    if (wave.isBossWave) {
        // Boss wave - single powerful enemy
        wave.enemyCount = 1;
        wave.difficultyMultiplier = 2.0f + (waveNumber / 10) * 0.5f;
        wave.enemyTypes.push_back("Boss");
    } else {
        // Regular wave
        wave.enemyCount = 1 + (waveNumber / 5); // Add enemy every 5 waves
        wave.enemyCount = std::min(wave.enemyCount, 4); // Max 4 enemies
        
        // Calculate difficulty
        if (m_survivalConfig.progressiveDifficulty) {
            wave.difficultyMultiplier = 1.0f + (waveNumber * m_survivalConfig.difficultyScaling);
        } else {
            wave.difficultyMultiplier = 1.0f;
        }
        
        // Random enemy types
        std::vector<std::string> possibleEnemies = {
            "Fighter", "Mage", "Tank", "Assassin", "Berserker"
        };
        
        for (int i = 0; i < wave.enemyCount; ++i) {
            int index = static_cast<int>(s_dist(s_rng) * possibleEnemies.size());
            wave.enemyTypes.push_back(possibleEnemies[index]);
        }
    }
    
    return wave;
}

void SurvivalMode::spawnWaveEnemies() {
    m_waveEnemies.clear();
    
    // Spawn positions
    float baseX = 300.0f;
    float spacing = 150.0f;
    
    for (int i = 0; i < m_currentWaveInfo.enemyCount; ++i) {
        // Create enemy character (would use CharacterFactory)
        auto enemy = std::make_shared<CharacterBase>();
        
        // Setup enemy with appropriate difficulty
        setupEnemy(enemy, m_currentWaveInfo.difficultyMultiplier);
        
        // Position enemy
        float xPos = baseX + (i * spacing);
        enemy->setPosition(XMFLOAT3(xPos, 0, 0));
        enemy->setFacingRight(false);
        
        // Mark as AI controlled
        enemy->setAIControlled(true);
        
        // Add to wave
        m_waveEnemies.push_back(enemy);
        
        // Register with systems
        m_physicsEngine->registerCharacter(enemy.get());
        m_combatSystem->registerCharacter(enemy.get());
    }
}

void SurvivalMode::setupEnemy(std::shared_ptr<CharacterBase> enemy, float difficulty) {
    // Scale enemy stats based on difficulty
    float healthMultiplier = 0.7f + (difficulty * 0.3f); // 70% to 130%+ health
    float damageMultiplier = 0.8f + (difficulty * 0.2f); // 80% to 120%+ damage
    
    enemy->setMaxHealth(BASE_HEALTH * healthMultiplier);
    enemy->setHealth(BASE_HEALTH * healthMultiplier);
    enemy->setMana(BASE_MANA);
    
    // Set AI difficulty based on wave
    // Higher waves = smarter AI
}

void SurvivalMode::updatePowerUps(float deltaTime) {
    // Spawn power-ups periodically
    m_nextPowerUpTime -= deltaTime;
    if (m_nextPowerUpTime <= 0 && m_survivalConfig.allowHealthItems) {
        if (s_dist(s_rng) < 0.3f) { // 30% chance
            spawnPowerUp();
        }
        m_nextPowerUpTime = 10.0f + s_dist(s_rng) * 20.0f; // 10-30 seconds
    }
    
    // Update active power-ups
    for (auto& powerUp : m_activePowerUps) {
        if (!powerUp.active) continue;
        
        powerUp.lifetime -= deltaTime;
        if (powerUp.lifetime <= 0) {
            powerUp.active = false;
            continue;
        }
        
        // Check collection
        if (m_player) {
            XMFLOAT3 playerPos = m_player->getPosition();
            float distance = std::abs(playerPos.x - powerUp.position.x);
            
            if (distance < 50.0f && std::abs(playerPos.y - powerUp.position.y) < 100.0f) {
                collectPowerUp(powerUp);
            }
        }
    }
    
    // Remove inactive power-ups
    m_activePowerUps.erase(
        std::remove_if(m_activePowerUps.begin(), m_activePowerUps.end(),
            [](const PowerUp& p) { return !p.active; }),
        m_activePowerUps.end()
    );
}

void SurvivalMode::spawnPowerUp() {
    PowerUp powerUp;
    
    // Random type based on player needs
    float healthPercent = m_player ? m_player->getHealth() / m_playerMaxHealth : 1.0f;
    float manaPercent = m_player ? m_player->getMana() / m_playerMaxMana : 1.0f;
    
    float roll = s_dist(s_rng);
    if (healthPercent < 0.3f && roll < 0.5f) {
        powerUp.type = PowerUpType::Health;
        powerUp.value = 0.3f; // 30% health
    } else if (manaPercent < 0.3f && roll < 0.7f) {
        powerUp.type = PowerUpType::Mana;
        powerUp.value = 0.5f; // 50% mana
    } else if (roll < 0.85f) {
        // Random buff
        int buffType = static_cast<int>(s_dist(s_rng) * 3);
        switch (buffType) {
            case 0: powerUp.type = PowerUpType::Damage; break;
            case 1: powerUp.type = PowerUpType::Speed; break;
            case 2: powerUp.type = PowerUpType::Shield; break;
        }
        powerUp.value = 0.5f; // 50% boost
    } else {
        // Rare full restore
        powerUp.type = PowerUpType::FullRestore;
        powerUp.value = 1.0f;
    }
    
    // Random position in arena
    powerUp.position = XMFLOAT3(-300.0f + s_dist(s_rng) * 600.0f, 50.0f, 0);
    powerUp.lifetime = 15.0f; // 15 seconds to collect
    powerUp.active = true;
    
    m_activePowerUps.push_back(powerUp);
}

void SurvivalMode::collectPowerUp(PowerUp& powerUp) {
    if (!m_player || !powerUp.active) return;
    
    applyPowerUpEffect(powerUp.type, powerUp.value);
    powerUp.active = false;
    
    m_survivalStats.itemsCollected++;
}

void SurvivalMode::applyPowerUpEffect(PowerUpType type, float value) {
    switch (type) {
        case PowerUpType::Health:
            {
                float healAmount = m_playerMaxHealth * value;
                float newHealth = std::min(m_player->getHealth() + healAmount, m_playerMaxHealth);
                m_player->setHealth(newHealth);
            }
            break;
            
        case PowerUpType::Mana:
            {
                float manaAmount = m_playerMaxMana * value;
                float newMana = std::min(m_player->getMana() + manaAmount, m_playerMaxMana);
                m_player->setMana(newMana);
            }
            break;
            
        case PowerUpType::Damage:
            // Apply damage boost (would need character support)
            break;
            
        case PowerUpType::Speed:
            // Apply speed boost (would need character support)
            break;
            
        case PowerUpType::Shield:
            // Apply damage reduction (would need character support)
            break;
            
        case PowerUpType::FullRestore:
            m_player->setHealth(m_playerMaxHealth);
            m_player->setMana(m_playerMaxMana);
            break;
    }
}

void SurvivalMode::completeWave() {
    m_waveInProgress = false;
    m_survivalStats.wavesCompleted++;
    
    // Check for perfect wave
    if (m_player && m_player->getHealth() >= m_playerMaxHealth * 0.99f) {
        m_survivalStats.perfectWaves++;
    }
    
    // Update stats
    m_survivalStats.totalKills += m_currentWaveInfo.enemyCount;
    
    // Recover player
    recoverPlayer();
    
    // Clear enemies
    for (auto& enemy : m_waveEnemies) {
        if (enemy) {
            m_physicsEngine->unregisterCharacter(enemy.get());
            m_combatSystem->unregisterCharacter(enemy.get());
        }
    }
    m_waveEnemies.clear();
    
    // Start wave timer
    m_waveTimer = 0.0f;
    
    // Spawn celebration power-up for boss waves
    if (m_currentWaveInfo.isBossWave) {
        PowerUp reward;
        reward.type = PowerUpType::FullRestore;
        reward.value = 1.0f;
        reward.position = XMFLOAT3(0, 50, 0);
        reward.lifetime = 30.0f;
        reward.active = true;
        m_activePowerUps.push_back(reward);
    }
}

void SurvivalMode::prepareNextWave() {
    m_currentWave++;
    m_currentWaveInfo = generateWave(m_currentWave);
    
    // Update difficulty
    m_currentDifficultyMultiplier = m_currentWaveInfo.difficultyMultiplier;
    
    // Spawn enemies
    spawnWaveEnemies();
    
    // Start wave
    m_waveInProgress = true;
    m_waveTimer = 0.0f;
}

bool SurvivalMode::isWaveComplete() const {
    for (const auto& enemy : m_waveEnemies) {
        if (enemy && enemy->getHealth() > 0) {
            return false;
        }
    }
    return true;
}

void SurvivalMode::recoverPlayer() {
    if (!m_player) return;
    
    // Recover health
    float currentHealth = m_player->getHealth();
    float healAmount = m_playerMaxHealth * m_survivalConfig.healthRecoveryPercent;
    m_player->setHealth(std::min(currentHealth + healAmount, m_playerMaxHealth));
    
    // Recover mana
    float currentMana = m_player->getMana();
    float manaAmount = m_playerMaxMana * m_survivalConfig.manaRecoveryPercent;
    m_player->setMana(std::min(currentMana + manaAmount, m_playerMaxMana));
}

void SurvivalMode::setSurvivalPlayer(std::shared_ptr<CharacterBase> player) {
    m_player = player;
    addPlayer(player);
    
    // Store max values
    m_playerMaxHealth = player->getMaxHealth();
    m_playerMaxMana = BASE_MANA;
}

void SurvivalMode::startWave() {
    if (!m_waveInProgress) {
        prepareNextWave();
    }
}

void SurvivalMode::endWave() {
    if (m_waveInProgress) {
        completeWave();
    }
}

int SurvivalMode::getHighScore() const {
    // Would load from save file
    // For now, return current score
    return m_survivalStats.wavesCompleted;
}

void SurvivalMode::saveHighScore() {
    // Would save to file
    // Store: waves completed, total time, kills, etc.
}

} // namespace ArenaFighter