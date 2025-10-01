#include "OnlineMode.h"
#include <algorithm>
#include <cstring>

namespace ArenaFighter {

OnlineMode::OnlineMode(bool isHost)
    : GameMode(MatchConfig()),
      m_onlineState(OnlineState::Disconnected),
      m_isHost(isHost),
      m_localPlayerId(isHost ? 0 : 1),
      m_remotePlayerId(isHost ? 1 : 0),
      m_pingTime(0.0f),
      m_lastSyncTime(0.0f),
      m_currentFrame(0),
      m_confirmedFrame(0),
      m_connectionQuality(1.0f),
      m_droppedPackets(0) {
    
    // Configure for online play
    m_config.maxPlayers = 2;
    m_config.allowPause = false; // No pausing in online matches
    
    m_lastPacketTime = std::chrono::steady_clock::now();
}

void OnlineMode::initialize() {
    GameMode::initialize();
    
    // Initialize network manager for online play
    m_networkManager->initialize();
    m_networkManager->setTickRate(60); // 60Hz update rate
    m_networkManager->setSendRate(30); // 30Hz send rate
    
    // Set up rollback buffer
    m_rollbackBuffer = std::queue<RollbackFrame>();
    
    // Start in disconnected state
    m_onlineState = OnlineState::Disconnected;
}

void OnlineMode::update(float deltaTime) {
    // Update network state
    updateNetworkState(deltaTime);
    
    // Only update game if connected and in match
    if (m_onlineState == OnlineState::InMatch) {
        // Receive network data
        receiveSyncData();
        
        // Update with rollback
        m_currentFrame++;
        
        // Store current state for rollback
        RollbackFrame currentFrame;
        currentFrame.frameNumber = m_currentFrame;
        currentFrame.gameState.frameNumber = m_currentFrame;
        currentFrame.gameState.gameTime = m_stateTimer;
        
        // Capture game state before update
        for (int i = 0; i < 2; ++i) {
            if (i < m_players.size() && m_players[i]) {
                currentFrame.gameState.positions[i] = m_players[i]->getPosition();
                currentFrame.gameState.health[i] = m_players[i]->getHealth();
                currentFrame.gameState.mana[i] = m_players[i]->getMana();
                currentFrame.gameState.states[i] = m_players[i]->getCurrentState();
            }
        }
        
        // Add to rollback buffer
        m_rollbackBuffer.push(currentFrame);
        
        // Limit rollback buffer size
        while (m_rollbackBuffer.size() > MAX_ROLLBACK_FRAMES) {
            m_rollbackBuffer.pop();
        }
        
        // Update base game
        GameMode::update(deltaTime);
        
        // Send sync data
        m_lastSyncTime += deltaTime;
        if (m_lastSyncTime >= 1.0f / 30.0f) { // 30Hz send rate
            sendSyncData();
            m_lastSyncTime = 0.0f;
        }
        
        // Lag compensation
        compensateForLag(deltaTime);
    }
}

void OnlineMode::shutdown() {
    // Disconnect from match
    if (m_onlineState != OnlineState::Disconnected) {
        // Send disconnect packet
        NetworkPacket packet;
        packet.type = PacketType::Disconnect;
        packet.timestamp = m_currentFrame;
        m_networkManager->sendPacket(packet);
    }
    
    // Clean up network
    m_networkManager->shutdown();
    
    GameMode::shutdown();
}

void OnlineMode::handleInput(int playerId, const InputCommand& input) {
    // Only handle local player input
    if (playerId == m_localPlayerId) {
        // Add to input buffer for sending
        m_inputBuffer.push(input);
        
        // Apply input locally (with prediction)
        GameMode::handleInput(playerId, input);
        
        // Send input immediately for responsiveness
        if (m_onlineState == OnlineState::InMatch) {
            NetworkPacket packet;
            packet.type = PacketType::Input;
            packet.playerId = m_localPlayerId;
            packet.timestamp = m_currentFrame;
            packet.inputData = input;
            m_networkManager->sendPacket(packet);
        }
    }
}

void OnlineMode::updateNetworkState(float deltaTime) {
    auto now = std::chrono::steady_clock::now();
    auto timeSinceLastPacket = std::chrono::duration_cast<std::chrono::milliseconds>
                               (now - m_lastPacketTime).count();
    
    switch (m_onlineState) {
        case OnlineState::Disconnected:
            // Waiting to start matchmaking or direct connect
            break;
            
        case OnlineState::Searching:
            // Matchmaking logic would go here
            // For now, simulate finding a match
            if (m_stateTimer > 5.0f) {
                m_onlineState = OnlineState::Connecting;
                m_stateTimer = 0.0f;
            }
            break;
            
        case OnlineState::Connecting:
            // Establish connection
            if (m_isHost) {
                if (m_networkManager->hasIncomingConnection()) {
                    m_onlineState = OnlineState::Syncing;
                    m_stateTimer = 0.0f;
                }
            } else {
                if (m_networkManager->isConnected()) {
                    m_onlineState = OnlineState::Syncing;
                    m_stateTimer = 0.0f;
                }
            }
            
            // Timeout after 30 seconds
            if (m_stateTimer > 30.0f) {
                handleDisconnection();
            }
            break;
            
        case OnlineState::Syncing:
            // Synchronize initial game state
            resyncGameState();
            
            if (m_stateTimer > 2.0f) {
                m_onlineState = OnlineState::Ready;
                m_stateTimer = 0.0f;
            }
            break;
            
        case OnlineState::Ready:
            // Both players ready, start match
            if (m_players.size() >= 2) {
                m_onlineState = OnlineState::InMatch;
                startMatch();
            }
            break;
            
        case OnlineState::InMatch:
            // Monitor connection quality
            if (timeSinceLastPacket > 1000) { // No packet for 1 second
                m_connectionQuality = std::max(0.0f, m_connectionQuality - deltaTime);
                
                if (timeSinceLastPacket > 5000) { // 5 seconds timeout
                    m_onlineState = OnlineState::Reconnecting;
                    m_stateTimer = 0.0f;
                }
            } else {
                m_connectionQuality = std::min(1.0f, m_connectionQuality + deltaTime * 2.0f);
            }
            break;
            
        case OnlineState::Reconnecting:
            attemptReconnection();
            
            // Give up after 10 seconds
            if (m_stateTimer > 10.0f) {
                handleDisconnection();
            }
            break;
    }
    
    m_stateTimer += deltaTime;
}

void OnlineMode::sendSyncData() {
    NetworkPacket packet;
    packet.type = PacketType::GameState;
    packet.playerId = m_localPlayerId;
    packet.timestamp = m_currentFrame;
    
    // Pack sync data
    SyncData& sync = packet.syncData;
    sync.frameNumber = m_currentFrame;
    sync.gameTime = m_roundTimer;
    
    // Add player states
    if (m_localPlayerId < m_players.size() && m_players[m_localPlayerId]) {
        auto& player = m_players[m_localPlayerId];
        sync.positions[m_localPlayerId] = player->getPosition();
        sync.health[m_localPlayerId] = player->getHealth();
        sync.mana[m_localPlayerId] = player->getMana();
        sync.states[m_localPlayerId] = player->getCurrentState();
    }
    
    // Send packet
    m_networkManager->sendPacket(packet);
}

void OnlineMode::receiveSyncData() {
    NetworkPacket packet;
    while (m_networkManager->receivePacket(packet)) {
        m_lastPacketTime = std::chrono::steady_clock::now();
        
        switch (packet.type) {
            case PacketType::Input:
                // Process remote input
                if (packet.playerId == m_remotePlayerId) {
                    m_lastRemoteInput = packet.inputData;
                    
                    // Check if we need to rollback
                    if (packet.timestamp < m_currentFrame) {
                        performRollback(packet.timestamp);
                    }
                    
                    // Apply input
                    GameMode::handleInput(m_remotePlayerId, packet.inputData);
                }
                break;
                
            case PacketType::GameState:
                // Validate and sync state
                if (packet.playerId == m_remotePlayerId) {
                    const SyncData& remoteSync = packet.syncData;
                    
                    // Update remote player state
                    if (m_remotePlayerId < m_players.size() && m_players[m_remotePlayerId]) {
                        auto& remotePlayer = m_players[m_remotePlayerId];
                        
                        // Interpolate position for smoothness
                        XMFLOAT3 targetPos = remoteSync.positions[m_remotePlayerId];
                        XMFLOAT3 currentPos = remotePlayer->getPosition();
                        
                        // Simple lerp
                        float lerpFactor = 0.5f;
                        XMFLOAT3 newPos;
                        newPos.x = currentPos.x + (targetPos.x - currentPos.x) * lerpFactor;
                        newPos.y = currentPos.y + (targetPos.y - currentPos.y) * lerpFactor;
                        newPos.z = currentPos.z + (targetPos.z - currentPos.z) * lerpFactor;
                        
                        remotePlayer->setPosition(newPos);
                        remotePlayer->setHealth(remoteSync.health[m_remotePlayerId]);
                        remotePlayer->setMana(remoteSync.mana[m_remotePlayerId]);
                    }
                    
                    // Update confirmed frame
                    m_confirmedFrame = std::max(m_confirmedFrame, remoteSync.frameNumber);
                }
                break;
                
            case PacketType::Disconnect:
                handleDisconnection();
                break;
        }
        
        // Calculate ping
        m_pingTime = m_networkManager->getPing();
    }
}

void OnlineMode::performRollback(int toFrame) {
    // Find the frame to rollback to
    std::queue<RollbackFrame> tempQueue;
    RollbackFrame* targetFrame = nullptr;
    
    while (!m_rollbackBuffer.empty()) {
        RollbackFrame& frame = m_rollbackBuffer.front();
        if (frame.frameNumber == toFrame) {
            targetFrame = &frame;
            break;
        }
        tempQueue.push(frame);
        m_rollbackBuffer.pop();
    }
    
    if (!targetFrame) {
        // Can't rollback that far
        return;
    }
    
    // Restore game state
    for (int i = 0; i < 2; ++i) {
        if (i < m_players.size() && m_players[i]) {
            m_players[i]->setPosition(targetFrame->gameState.positions[i]);
            m_players[i]->setHealth(targetFrame->gameState.health[i]);
            m_players[i]->setMana(targetFrame->gameState.mana[i]);
        }
    }
    
    // Re-simulate frames
    int framesToSimulate = m_currentFrame - toFrame;
    for (int i = 0; i < framesToSimulate; ++i) {
        // Update with stored/predicted inputs
        GameMode::update(1.0f / 60.0f); // Fixed timestep
    }
}

void OnlineMode::compensateForLag(float deltaTime) {
    // Simple lag compensation
    float lagCompensation = m_pingTime * 0.001f; // Convert to seconds
    
    // Predict ahead based on ping
    if (m_localPlayerId < m_players.size() && m_players[m_localPlayerId]) {
        // Local player prediction already handled by immediate input
    }
    
    // Interpolate remote player
    interpolateRemotePlayer(deltaTime);
}

void OnlineMode::interpolateRemotePlayer(float deltaTime) {
    // Smooth remote player movement
    if (m_remotePlayerId < m_players.size() && m_players[m_remotePlayerId]) {
        // Interpolation handled in receiveSyncData for now
        // Could be expanded with more sophisticated interpolation
    }
}

void OnlineMode::handleDisconnection() {
    m_onlineState = OnlineState::Disconnected;
    
    // Pause the game
    if (m_currentState == MatchState::InProgress) {
        setState(MatchState::Paused);
    }
    
    // Notify UI of disconnection
    // This would trigger a disconnect message
}

void OnlineMode::attemptReconnection() {
    // Try to reconnect to the match
    if (m_networkManager->attemptReconnect()) {
        m_onlineState = OnlineState::Syncing;
        m_stateTimer = 0.0f;
    }
}

void OnlineMode::resyncGameState() {
    // Full state synchronization
    NetworkPacket packet;
    packet.type = PacketType::FullSync;
    packet.playerId = m_localPlayerId;
    packet.timestamp = m_currentFrame;
    
    // Pack complete game state
    // This would include all necessary data to restore the match
    
    m_networkManager->sendReliablePacket(packet);
}

void OnlineMode::startMatchmaking() {
    if (m_onlineState == OnlineState::Disconnected) {
        m_onlineState = OnlineState::Searching;
        m_stateTimer = 0.0f;
        
        // Start matchmaking process
        // This would connect to matchmaking servers
    }
}

void OnlineMode::cancelMatchmaking() {
    if (m_onlineState == OnlineState::Searching ||
        m_onlineState == OnlineState::Connecting) {
        m_onlineState = OnlineState::Disconnected;
        m_stateTimer = 0.0f;
    }
}

void OnlineMode::connectToHost(const std::string& hostAddress) {
    if (m_onlineState == OnlineState::Disconnected) {
        m_isHost = false;
        m_localPlayerId = 1;
        m_remotePlayerId = 0;
        
        if (m_networkManager->connectTo(hostAddress)) {
            m_onlineState = OnlineState::Connecting;
            m_stateTimer = 0.0f;
        }
    }
}

void OnlineMode::hostMatch() {
    if (m_onlineState == OnlineState::Disconnected) {
        m_isHost = true;
        m_localPlayerId = 0;
        m_remotePlayerId = 1;
        
        if (m_networkManager->startHosting()) {
            m_onlineState = OnlineState::Connecting;
            m_stateTimer = 0.0f;
        }
    }
}

bool OnlineMode::isConnected() const {
    return m_onlineState == OnlineState::InMatch ||
           m_onlineState == OnlineState::Ready ||
           m_onlineState == OnlineState::Syncing;
}

void OnlineMode::setMaxRollbackFrames(int frames) {
    // Would adjust rollback buffer size
    // Limited to MAX_ROLLBACK_FRAMES
}

int OnlineMode::getCurrentDelay() const {
    return m_currentFrame - m_confirmedFrame;
}

} // namespace ArenaFighter