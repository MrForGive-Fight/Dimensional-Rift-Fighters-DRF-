#pragma once

#include <memory>
#include <string>
#include "GameMode.h"
#include "SinglePlayerMode.h"
#include "VersusMode.h"
#include "OnlineMode.h"
#include "TrainingMode.h"
#include "SurvivalMode.h"

namespace ArenaFighter {

// Game mode identifiers
enum class GameModeID {
    SinglePlayer,
    Versus,
    Online,
    Training,
    Survival
};

// Factory for creating game modes
class GameModeFactory {
public:
    // Create a game mode by ID
    static std::shared_ptr<GameMode> createGameMode(GameModeID modeId) {
        switch (modeId) {
            case GameModeID::SinglePlayer:
                return std::make_shared<SinglePlayerMode>();
                
            case GameModeID::Versus:
                return std::make_shared<VersusMode>();
                
            case GameModeID::Online:
                return std::make_shared<OnlineMode>();
                
            case GameModeID::Training:
                return std::make_shared<TrainingMode>();
                
            case GameModeID::Survival:
                return std::make_shared<SurvivalMode>();
                
            default:
                return nullptr;
        }
    }
    
    // Create a game mode by name
    static std::shared_ptr<GameMode> createGameMode(const std::string& modeName) {
        if (modeName == "SinglePlayer" || modeName == "single" || modeName == "sp") {
            return createGameMode(GameModeID::SinglePlayer);
        } else if (modeName == "Versus" || modeName == "vs" || modeName == "local") {
            return createGameMode(GameModeID::Versus);
        } else if (modeName == "Online" || modeName == "net" || modeName == "network") {
            return createGameMode(GameModeID::Online);
        } else if (modeName == "Training" || modeName == "train" || modeName == "practice") {
            return createGameMode(GameModeID::Training);
        } else if (modeName == "Survival" || modeName == "survive" || modeName == "endless") {
            return createGameMode(GameModeID::Survival);
        }
        
        return nullptr;
    }
    
    // Get game mode name
    static std::string getGameModeName(GameModeID modeId) {
        switch (modeId) {
            case GameModeID::SinglePlayer: return "Single Player";
            case GameModeID::Versus: return "Versus";
            case GameModeID::Online: return "Online";
            case GameModeID::Training: return "Training";
            case GameModeID::Survival: return "Survival";
            default: return "Unknown";
        }
    }
    
    // Get game mode description
    static std::string getGameModeDescription(GameModeID modeId) {
        switch (modeId) {
            case GameModeID::SinglePlayer:
                return "Battle against AI opponents with adjustable difficulty";
                
            case GameModeID::Versus:
                return "Local 1v1 battles with a friend";
                
            case GameModeID::Online:
                return "Fight players worldwide with rollback netcode";
                
            case GameModeID::Training:
                return "Practice combos and techniques with training tools";
                
            case GameModeID::Survival:
                return "Survive endless waves of increasingly difficult enemies";
                
            default:
                return "";
        }
    }
    
    // Check if mode supports feature
    static bool supportsOnline(GameModeID modeId) {
        return modeId == GameModeID::Online;
    }
    
    static bool supportsPause(GameModeID modeId) {
        return modeId != GameModeID::Online; // All except online
    }
    
    static bool supportsAI(GameModeID modeId) {
        return modeId == GameModeID::SinglePlayer || 
               modeId == GameModeID::Training ||
               modeId == GameModeID::Survival;
    }
};

} // namespace ArenaFighter