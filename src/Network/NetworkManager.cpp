#include "NetworkManager.h"
#include "NetworkPacket.h"
#include "InputBuffer.h"
#include <iostream>
#include <algorithm>
#include <thread>

namespace ArenaFighter {

NetworkManager::NetworkManager() 
    : m_connectionState(ConnectionState::Disconnected)
    , m_localPlayerId(0)
    , m_tickAccumulator(0.0f)
    , m_sendAccumulator(0.0f)
    , m_sequenceNumber(0)
    , m_lastReceivedSequence(0)
    , m_currentMatchId(0)
    , m_currentGameMode(0)
    , m_randomSeed(0)
    , m_socket(nullptr)
    , m_server(nullptr) {
    
    m_lastTickTime = std::chrono::steady_clock::now();
    m_lastSendTime = std::chrono::steady_clock::now();
    m_stats.lastUpdate = std::chrono::steady_clock::now();
}

NetworkManager::~NetworkManager() {
    Shutdown();
}

bool NetworkManager::Initialize() {
    // Register default packet handlers
    RegisterPacketHandler(static_cast<uint16_t>(PacketType::PlayerStateUpdate),
        [this](NetworkPacket* packet) { HandlePlayerState(packet); });
    
    RegisterPacketHandler(static_cast<uint16_t>(PacketType::InputCommand),
        [this](NetworkPacket* packet) { HandleInput(packet); });
    
    RegisterPacketHandler(static_cast<uint16_t>(PacketType::AttackEvent),
        [this](NetworkPacket* packet) { HandleAttack(packet); });
    
    RegisterPacketHandler(static_cast<uint16_t>(PacketType::DamageConfirmation),
        [this](NetworkPacket* packet) { HandleDamage(packet); });
    
    RegisterPacketHandler(static_cast<uint16_t>(PacketType::MatchStart),
        [this](NetworkPacket* packet) { HandleMatchStart(packet); });
    
    // Initialize input buffer for local player
    m_playerInputBuffers[m_localPlayerId] = std::make_unique<InputBuffer>(m_localPlayerId);
    
    return true;
}

void NetworkManager::Shutdown() {
    Disconnect();
    
    m_playerInputBuffers.clear();
    m_packetHandlers.clear();
    
    while (!m_incomingPackets.empty()) {
        m_incomingPackets.pop();
    }
    
    while (!m_outgoingPackets.empty()) {
        m_outgoingPackets.pop();
    }
}

void NetworkManager::Update(float deltaTime) {
    if (m_connectionState == ConnectionState::Disconnected) {
        return;
    }
    
    // Update timing accumulators
    m_tickAccumulator += deltaTime;
    m_sendAccumulator += deltaTime;
    
    // Fixed tick update (60Hz)
    const float tickInterval = 1.0f / NetworkConfig::TICK_RATE;
    while (m_tickAccumulator >= tickInterval) {
        TickUpdate();
        m_tickAccumulator -= tickInterval;
    }
    
    // Fixed send update (30Hz)
    const float sendInterval = 1.0f / NetworkConfig::SEND_RATE;
    while (m_sendAccumulator >= sendInterval) {
        SendUpdate();
        m_sendAccumulator -= sendInterval;
    }
    
    // Process incoming packets
    ProcessIncomingPackets();
    
    // Update network stats
    UpdateNetworkStats();
}

void NetworkManager::TickUpdate() {
    // Update input buffers
    UpdateInputBuffers();
    
    // Check for rollback conditions
    for (auto& [playerId, buffer] : m_playerInputBuffers) {
        if (buffer->NeedsRollback(m_sequenceNumber)) {
            m_stats.rollbackFrames++;
        }
    }
}

void NetworkManager::SendUpdate() {
    // Send all queued packets
    while (!m_outgoingPackets.empty()) {
        auto packet = m_outgoingPackets.front();
        m_outgoingPackets.pop();
        
        // Set sequence and timestamp
        packet->SetSequence(m_sequenceNumber++);
        packet->SetTimestamp(static_cast<uint32_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()
            ).count()
        ));
        
        // Calculate checksum
        std::vector<uint8_t> buffer;
        packet->Serialize(buffer);
        packet->GetHeader().checksum = packet->CalculateChecksum(buffer);
        
        // TODO: Actually send packet via socket
        // For now, just track stats
        m_stats.packetsSent++;
        m_stats.bandwidth += static_cast<float>(buffer.size());
    }
}

bool NetworkManager::StartHost(int port) {
    if (m_connectionState != ConnectionState::Disconnected) {
        return false;
    }
    
    // TODO: Implement actual socket creation and binding
    std::cout << "Starting host on port " << port << std::endl;
    
    m_connectionState = ConnectionState::Connected;
    m_localPlayerId = 1; // Host is always player 1
    
    return true;
}

bool NetworkManager::ConnectToHost(const std::string& address, int port) {
    if (m_connectionState != ConnectionState::Disconnected) {
        return false;
    }
    
    m_connectionState = ConnectionState::Connecting;
    
    // TODO: Implement actual socket connection
    std::cout << "Connecting to " << address << ":" << port << std::endl;
    
    // Simulate connection success
    m_connectionState = ConnectionState::Connected;
    m_localPlayerId = 2; // Client gets next available ID
    
    return true;
}

void NetworkManager::Disconnect() {
    if (m_connectionState == ConnectionState::Disconnected) {
        return;
    }
    
    // Send disconnect packet
    auto disconnectPacket = std::make_shared<NetworkPacket>(PacketType::Disconnect);
    SendPacket(disconnectPacket, true);
    
    m_connectionState = ConnectionState::Disconnected;
    
    // TODO: Close actual socket
    m_socket = nullptr;
    m_server = nullptr;
}

void NetworkManager::SendPacket(std::shared_ptr<NetworkPacket> packet, bool reliable) {
    if (m_connectionState == ConnectionState::Disconnected) {
        return;
    }
    
    if (reliable) {
        packet->AddFlag(PacketFlags::Reliable);
    }
    
    m_outgoingPackets.push(packet);
}

void NetworkManager::ProcessIncomingPackets() {
    // Process up to 10 packets per frame to avoid stalls
    int packetsProcessed = 0;
    while (!m_incomingPackets.empty() && packetsProcessed < 10) {
        auto packet = m_incomingPackets.front();
        m_incomingPackets.pop();
        
        ProcessPacket(packet.get());
        packetsProcessed++;
        m_stats.packetsReceived++;
    }
}

void NetworkManager::RegisterPacketHandler(uint16_t packetType, 
                                         std::function<void(NetworkPacket*)> handler) {
    m_packetHandlers[packetType] = handler;
}

void NetworkManager::ProcessPacket(NetworkPacket* packet) {
    // Update sequence tracking
    uint32_t sequence = packet->GetHeader().sequence;
    if (sequence > m_lastReceivedSequence) {
        // Track packet loss
        uint32_t expectedPackets = sequence - m_lastReceivedSequence;
        if (expectedPackets > 1) {
            uint32_t lostPackets = expectedPackets - 1;
            m_stats.packetLoss = (m_stats.packetLoss * 0.9f) + 
                                (static_cast<float>(lostPackets) / expectedPackets * 0.1f);
        }
        m_lastReceivedSequence = sequence;
    }
    
    // Find and execute handler
    auto it = m_packetHandlers.find(packet->GetHeader().type);
    if (it != m_packetHandlers.end()) {
        it->second(packet);
    }
}

void NetworkManager::SendInput(uint32_t frame, uint32_t inputMask, uint16_t inputId) {
    auto inputPacket = std::make_shared<InputPacket>();
    inputPacket->playerId = m_localPlayerId;
    inputPacket->inputMask = inputMask;
    inputPacket->inputId = inputId;
    inputPacket->timestamp = static_cast<uint16_t>(frame);
    
    SendPacket(inputPacket, true);
    
    // Store in local buffer
    InputFrame localInput;
    localInput.frame = frame;
    localInput.inputMask = inputMask;
    localInput.inputId = inputId;
    localInput.timestamp = inputPacket->timestamp;
    localInput.confirmed = false;
    localInput.predicted = false;
    
    if (auto buffer = m_playerInputBuffers[m_localPlayerId].get()) {
        buffer->AddInput(localInput);
    }
}

bool NetworkManager::GetRemoteInput(uint32_t playerId, uint32_t frame, uint32_t& inputMask) {
    auto it = m_playerInputBuffers.find(playerId);
    if (it == m_playerInputBuffers.end()) {
        return false;
    }
    
    auto input = it->second->GetInput(frame);
    if (input.has_value()) {
        inputMask = input->inputMask;
        return true;
    }
    
    // Try prediction if no input available
    PredictMissingInputs(playerId, frame);
    inputMask = it->second->GetInputMask(frame);
    return true;
}

void NetworkManager::ConfirmFrame(uint32_t frame) {
    for (auto& [playerId, buffer] : m_playerInputBuffers) {
        buffer->ConfirmFramesUpTo(frame);
    }
}

void NetworkManager::CreateMatch(const std::string& matchName, uint8_t gameMode, uint8_t stageId) {
    // TODO: Implement match creation
    m_currentGameMode = gameMode;
}

void NetworkManager::JoinMatch(const std::string& matchCode) {
    // TODO: Implement match joining
}

void NetworkManager::StartMatch() {
    if (m_connectionState != ConnectionState::Connected) {
        return;
    }
    
    m_connectionState = ConnectionState::InMatch;
    
    // Send match start packet
    auto matchPacket = std::make_shared<MatchStartPacket>();
    matchPacket->matchId = ++m_currentMatchId;
    matchPacket->gameMode = m_currentGameMode;
    matchPacket->randomSeed = static_cast<uint32_t>(std::time(nullptr));
    
    SendPacket(matchPacket, true);
    
    if (m_onMatchStart) {
        m_onMatchStart(m_currentMatchId, m_currentGameMode);
    }
}

void NetworkManager::EndMatch() {
    if (m_connectionState != ConnectionState::InMatch) {
        return;
    }
    
    m_connectionState = ConnectionState::Connected;
    
    // TODO: Send match end packet
}

void NetworkManager::HandlePlayerState(NetworkPacket* packet) {
    auto statePacket = static_cast<PlayerStatePacket*>(packet);
    
    // TODO: Update player state in game
}

void NetworkManager::HandleInput(NetworkPacket* packet) {
    auto inputPacket = static_cast<InputPacket*>(packet);
    
    // Add to appropriate input buffer
    auto it = m_playerInputBuffers.find(inputPacket->playerId);
    if (it == m_playerInputBuffers.end()) {
        // Create buffer for new player
        m_playerInputBuffers[inputPacket->playerId] = 
            std::make_unique<InputBuffer>(inputPacket->playerId);
        
        if (m_onPlayerConnected) {
            m_onPlayerConnected(inputPacket->playerId);
        }
    }
    
    InputFrame frame;
    frame.frame = inputPacket->timestamp;
    frame.inputMask = inputPacket->inputMask;
    frame.inputId = inputPacket->inputId;
    frame.timestamp = inputPacket->timestamp;
    frame.confirmed = true;
    frame.predicted = false;
    
    m_playerInputBuffers[inputPacket->playerId]->AddInput(frame);
}

void NetworkManager::HandleAttack(NetworkPacket* packet) {
    auto attackPacket = static_cast<AttackPacket*>(packet);
    
    // TODO: Process attack event
}

void NetworkManager::HandleDamage(NetworkPacket* packet) {
    auto damagePacket = static_cast<DamagePacket*>(packet);
    
    // TODO: Apply damage confirmation
}

void NetworkManager::HandleMatchStart(NetworkPacket* packet) {
    auto matchPacket = static_cast<MatchStartPacket*>(packet);
    
    m_currentMatchId = matchPacket->matchId;
    m_currentGameMode = matchPacket->gameMode;
    m_randomSeed = matchPacket->randomSeed;
    
    if (m_onMatchStart) {
        m_onMatchStart(m_currentMatchId, m_currentGameMode);
    }
}

void NetworkManager::UpdateInputBuffers() {
    // Remove old frames from all buffers
    uint32_t currentFrame = m_sequenceNumber;
    uint32_t oldFrameThreshold = currentFrame > 120 ? currentFrame - 120 : 0;
    
    for (auto& [playerId, buffer] : m_playerInputBuffers) {
        buffer->RemoveOldFrames(oldFrameThreshold);
    }
}

void NetworkManager::PredictMissingInputs(uint32_t playerId, uint32_t frame) {
    auto it = m_playerInputBuffers.find(playerId);
    if (it == m_playerInputBuffers.end()) {
        return;
    }
    
    uint32_t predictedInput = it->second->PredictNextInput();
    it->second->AddPredictedInput(frame, predictedInput);
}

void NetworkManager::UpdateNetworkStats() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - m_stats.lastUpdate).count();
    
    if (elapsed >= 1000) { // Update every second
        CalculatePing();
        TrackPacketLoss();
        
        // Calculate bandwidth in KB/s
        m_stats.bandwidth = (m_stats.bandwidth / elapsed) * 1000.0f / 1024.0f;
        
        m_stats.lastUpdate = now;
    }
}

void NetworkManager::CalculatePing() {
    // TODO: Implement actual ping calculation
    // For now, simulate ping
    m_stats.ping = 50 + (rand() % 20);
}

void NetworkManager::TrackPacketLoss() {
    // Packet loss is already tracked in ProcessPacket
    // This could implement additional tracking if needed
}

} // namespace ArenaFighter