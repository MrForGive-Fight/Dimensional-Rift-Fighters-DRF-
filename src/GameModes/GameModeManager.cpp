#include "GameModeManager.h"
#include "SinglePlayerMode.h"
#include "VersusMode.h"
#include "OnlineMode.h"
#include "TrainingMode.h"
#include "SurvivalMode.h"

namespace ArenaFighter {

// Initialize static instance
GameModeManager* GameModeManager::s_instance = nullptr;

bool GameModeManager::changeGameMode(GameModeID newMode) {
    // Exit current mode
    if (m_currentMode) {
        if (m_onModeExit) {
            m_onModeExit(m_currentMode.get());
        }
        
        m_currentMode->shutdown();
    }
    
    // Create new mode
    auto newGameMode = GameModeFactory::createGameMode(newMode);
    if (!newGameMode) {
        return false;
    }
    
    // Apply saved settings if available
    switch (newMode) {
        case GameModeID::SinglePlayer:
            if (auto* settings = getModeSettings<AIDifficulty>(newMode)) {
                auto* spMode = dynamic_cast<SinglePlayerMode*>(newGameMode.get());
                if (spMode) {
                    spMode->setDifficulty(*settings);
                }
            }
            break;
            
        case GameModeID::Versus:
            if (auto* settings = getModeSettings<VersusSettings>(newMode)) {
                auto* vsMode = dynamic_cast<VersusMode*>(newGameMode.get());
                if (vsMode) {
                    vsMode->setSettings(*settings);
                }
            }
            break;
            
        case GameModeID::Training:
            if (auto* settings = getModeSettings<TrainingSettings>(newMode)) {
                auto* trainMode = dynamic_cast<TrainingMode*>(newGameMode.get());
                if (trainMode) {
                    trainMode->setSettings(*settings);
                }
            }
            break;
            
        case GameModeID::Survival:
            if (auto* settings = getModeSettings<SurvivalConfig>(newMode)) {
                auto* survMode = dynamic_cast<SurvivalMode*>(newGameMode.get());
                if (survMode) {
                    survMode->setConfig(*settings);
                }
            }
            break;
            
        default:
            break;
    }
    
    // Initialize new mode
    newGameMode->initialize();
    
    // Update current mode
    m_currentMode = newGameMode;
    m_currentModeId = newMode;
    
    // Trigger callback
    if (m_onModeChange) {
        m_onModeChange(m_currentMode.get());
    }
    
    return true;
}

bool GameModeManager::changeGameMode(const std::string& modeName) {
    auto mode = GameModeFactory::createGameMode(modeName);
    if (!mode) {
        return false;
    }
    
    // Determine mode ID from name
    GameModeID modeId = GameModeID::SinglePlayer;
    if (modeName.find("Versus") != std::string::npos || 
        modeName.find("vs") != std::string::npos) {
        modeId = GameModeID::Versus;
    } else if (modeName.find("Online") != std::string::npos || 
               modeName.find("net") != std::string::npos) {
        modeId = GameModeID::Online;
    } else if (modeName.find("Training") != std::string::npos || 
               modeName.find("train") != std::string::npos) {
        modeId = GameModeID::Training;
    } else if (modeName.find("Survival") != std::string::npos || 
               modeName.find("survive") != std::string::npos) {
        modeId = GameModeID::Survival;
    }
    
    return changeGameMode(modeId);
}

void GameModeManager::update(float deltaTime) {
    if (m_currentMode) {
        m_currentMode->update(deltaTime);
    }
}

void GameModeManager::render() {
    if (m_currentMode) {
        m_currentMode->render();
    }
}

void GameModeManager::handleInput(int playerId, const InputCommand& input) {
    if (m_currentMode) {
        m_currentMode->handleInput(playerId, input);
    }
}

void GameModeManager::cleanup() {
    if (m_currentMode) {
        m_currentMode->shutdown();
        m_currentMode.reset();
    }
    
    m_modeSettings.clear();
}

void GameModeManager::reset() {
    cleanup();
    
    // Reset to default mode
    changeGameMode(GameModeID::SinglePlayer);
}

} // namespace ArenaFighter