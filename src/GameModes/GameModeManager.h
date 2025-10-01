#pragma once

#include <memory>
#include <unordered_map>
#include <functional>
#include "GameMode.h"
#include "GameModeFactory.h"

namespace ArenaFighter {

// Game mode transition callbacks
using GameModeCallback = std::function<void(GameMode*)>;

class GameModeManager {
private:
    std::shared_ptr<GameMode> m_currentMode;
    GameModeID m_currentModeId;
    
    // Mode-specific settings cache
    std::unordered_map<GameModeID, std::shared_ptr<void>> m_modeSettings;
    
    // Callbacks
    GameModeCallback m_onModeChange;
    GameModeCallback m_onModeExit;
    
    // Singleton pattern
    static GameModeManager* s_instance;
    GameModeManager() : m_currentModeId(GameModeID::SinglePlayer) {}

public:
    // Get singleton instance
    static GameModeManager* getInstance() {
        if (!s_instance) {
            s_instance = new GameModeManager();
        }
        return s_instance;
    }
    
    // Mode management
    bool changeGameMode(GameModeID newMode);
    bool changeGameMode(const std::string& modeName);
    
    // Current mode access
    GameMode* getCurrentMode() const { return m_currentMode.get(); }
    GameModeID getCurrentModeId() const { return m_currentModeId; }
    
    // Type-safe mode access
    template<typename T>
    T* getCurrentModeAs() const {
        return dynamic_cast<T*>(m_currentMode.get());
    }
    
    // Update and render
    void update(float deltaTime);
    void render();
    
    // Input forwarding
    void handleInput(int playerId, const InputCommand& input);
    
    // Settings management
    template<typename T>
    void saveModeSettings(GameModeID modeId, const T& settings) {
        m_modeSettings[modeId] = std::make_shared<T>(settings);
    }
    
    template<typename T>
    T* getModeSettings(GameModeID modeId) {
        auto it = m_modeSettings.find(modeId);
        if (it != m_modeSettings.end()) {
            return static_cast<T*>(it->second.get());
        }
        return nullptr;
    }
    
    // Callbacks
    void setModeChangeCallback(GameModeCallback callback) {
        m_onModeChange = callback;
    }
    
    void setModeExitCallback(GameModeCallback callback) {
        m_onModeExit = callback;
    }
    
    // Utility
    void cleanup();
    void reset();
};

// Convenience macros for accessing specific modes
#define GET_SINGLE_PLAYER_MODE() \
    GameModeManager::getInstance()->getCurrentModeAs<SinglePlayerMode>()

#define GET_VERSUS_MODE() \
    GameModeManager::getInstance()->getCurrentModeAs<VersusMode>()

#define GET_ONLINE_MODE() \
    GameModeManager::getInstance()->getCurrentModeAs<OnlineMode>()

#define GET_TRAINING_MODE() \
    GameModeManager::getInstance()->getCurrentModeAs<TrainingMode>()

#define GET_SURVIVAL_MODE() \
    GameModeManager::getInstance()->getCurrentModeAs<SurvivalMode>()

} // namespace ArenaFighter