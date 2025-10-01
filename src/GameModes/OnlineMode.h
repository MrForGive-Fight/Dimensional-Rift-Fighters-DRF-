#pragma once

#include "GameMode.h"
#include "../Network/NetworkManager.h"
#include <queue>
#include <chrono>

namespace ArenaFighter {

// Online match states
enum class OnlineState {
    Disconnected,
    Searching,      // Looking for opponent
    Connecting,     // Establishing connection
    Syncing,        // Synchronizing game state
    Ready,          // Ready to start
    InMatch,        // Active gameplay
    Reconnecting    // Attempting to reconnect
};

// Network synchronization data
struct SyncData {
    int frameNumber;
    float gameTime;
    InputCommand inputs[2];
    XMFLOAT3 positions[2];
    float health[2];
    float mana[2];
    CharacterState states[2];
};

// Rollback data for lag compensation
struct RollbackFrame {
    int frameNumber;
    SyncData gameState;
    std::vector<InputCommand> confirmedInputs;
};

class OnlineMode : public GameMode {
private:
    OnlineState m_onlineState;
    
    // Network components
    bool m_isHost;
    int m_localPlayerId;
    int m_remotePlayerId;
    float m_pingTime;
    float m_lastSyncTime;
    
    // Rollback netcode
    static constexpr int MAX_ROLLBACK_FRAMES = 7;
    std::queue<RollbackFrame> m_rollbackBuffer;
    int m_currentFrame;
    int m_confirmedFrame;
    
    // Input prediction
    std::queue<InputCommand> m_inputBuffer;
    InputCommand m_lastRemoteInput;
    
    // Connection quality
    float m_connectionQuality;
    int m_droppedPackets;
    std::chrono::steady_clock::time_point m_lastPacketTime;
    
    // Synchronization methods
    void sendSyncData();
    void receiveSyncData();
    void performRollback(int toFrame);
    void predictInput(int playerId);
    
    // Network state management
    void updateNetworkState(float deltaTime);
    void handleDisconnection();
    void attemptReconnection();
    
    // Lag compensation
    void compensateForLag(float deltaTime);
    void interpolateRemotePlayer(float deltaTime);
    
    // Match validation
    bool validateGameState(const SyncData& local, const SyncData& remote);
    void resyncGameState();

public:
    OnlineMode(bool isHost = false);
    virtual ~OnlineMode() = default;
    
    // Override base methods
    void initialize() override;
    void update(float deltaTime) override;
    void shutdown() override;
    void handleInput(int playerId, const InputCommand& input) override;
    
    // Network setup
    void setLocalPlayer(int playerId) { m_localPlayerId = playerId; }
    void setRemotePlayer(int playerId) { m_remotePlayerId = playerId; }
    void setHost(bool isHost) { m_isHost = isHost; }
    
    // Connection management
    void startMatchmaking();
    void cancelMatchmaking();
    void connectToHost(const std::string& hostAddress);
    void hostMatch();
    
    // Network status
    OnlineState getOnlineState() const { return m_onlineState; }
    float getPing() const { return m_pingTime; }
    float getConnectionQuality() const { return m_connectionQuality; }
    bool isConnected() const;
    
    // Mode specific implementations
    std::string getModeName() const override { return "Online"; }
    GameModeType getModeType() const override { return GameModeType::Ranked1v1; }
    bool supportsOnline() const override { return true; }
    int getMinPlayers() const override { return 2; }
    int getMaxPlayers() const override { return 2; }
    
    // Rollback controls
    void setMaxRollbackFrames(int frames);
    int getCurrentDelay() const;
};

} // namespace ArenaFighter