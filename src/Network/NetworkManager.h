#pragma once

#include <memory>
#include <vector>
#include <queue>
#include <unordered_map>
#include <functional>
#include <chrono>

namespace ArenaFighter {

// Forward declarations
class NetworkPacket;
class InputBuffer;
class PacketHandler;

enum class ConnectionState {
    Disconnected,
    Connecting,
    Connected,
    InMatch
};

struct NetworkConfig {
    static constexpr int TICK_RATE = 60;
    static constexpr int SEND_RATE = 30;
    static constexpr int MAX_ROLLBACK_FRAMES = 7;
    static constexpr int INTERPOLATION_DELAY_MS = 100;
    static constexpr int MAX_PREDICTION_FRAMES = 8;
    static constexpr int PACKET_SIZE_LIMIT = 1400;
    static constexpr int COMPRESSION_THRESHOLD = 256;
    static constexpr int INPUT_BUFFER_SIZE = 3;
};

struct NetworkStats {
    int ping = 0;
    float packetLoss = 0.0f;
    int rollbackFrames = 0;
    int packetsReceived = 0;
    int packetsSent = 0;
    float bandwidth = 0.0f;
    std::chrono::steady_clock::time_point lastUpdate;
};

class NetworkManager {
public:
    NetworkManager();
    ~NetworkManager();
    
    // Core networking functions
    bool Initialize();
    void Shutdown();
    void Update(float deltaTime);
    
    // Connection management
    bool StartHost(int port);
    bool ConnectToHost(const std::string& address, int port);
    void Disconnect();
    
    // Packet handling
    void SendPacket(std::shared_ptr<NetworkPacket> packet, bool reliable = true);
    void ProcessIncomingPackets();
    void RegisterPacketHandler(uint16_t packetType, std::function<void(NetworkPacket*)> handler);
    
    // Input handling for rollback
    void SendInput(uint32_t frame, uint32_t inputMask, uint16_t inputId);
    bool GetRemoteInput(uint32_t playerId, uint32_t frame, uint32_t& inputMask);
    void ConfirmFrame(uint32_t frame);
    
    // Match management
    void CreateMatch(const std::string& matchName, uint8_t gameMode, uint8_t stageId);
    void JoinMatch(const std::string& matchCode);
    void StartMatch();
    void EndMatch();
    
    // State queries
    ConnectionState GetConnectionState() const { return m_connectionState; }
    NetworkStats GetNetworkStats() const { return m_stats; }
    int GetLocalPlayerId() const { return m_localPlayerId; }
    int GetPlayerCount() const { return static_cast<int>(m_playerInputBuffers.size()); }
    
    // Callbacks
    using OnPlayerConnectedCallback = std::function<void(uint32_t playerId)>;
    using OnPlayerDisconnectedCallback = std::function<void(uint32_t playerId)>;
    using OnMatchStartCallback = std::function<void(uint32_t matchId, uint8_t gameMode)>;
    
    void SetOnPlayerConnected(OnPlayerConnectedCallback callback) { m_onPlayerConnected = callback; }
    void SetOnPlayerDisconnected(OnPlayerDisconnectedCallback callback) { m_onPlayerDisconnected = callback; }
    void SetOnMatchStart(OnMatchStartCallback callback) { m_onMatchStart = callback; }

private:
    // Tick rate control
    void TickUpdate();
    void SendUpdate();
    
    // Packet processing
    void ProcessPacket(NetworkPacket* packet);
    void HandlePlayerState(NetworkPacket* packet);
    void HandleInput(NetworkPacket* packet);
    void HandleAttack(NetworkPacket* packet);
    void HandleDamage(NetworkPacket* packet);
    void HandleMatchStart(NetworkPacket* packet);
    
    // Rollback support
    void UpdateInputBuffers();
    void PredictMissingInputs(uint32_t playerId, uint32_t frame);
    
    // Network stats
    void UpdateNetworkStats();
    void CalculatePing();
    void TrackPacketLoss();
    
private:
    ConnectionState m_connectionState;
    NetworkStats m_stats;
    
    // Player management
    uint32_t m_localPlayerId;
    std::unordered_map<uint32_t, std::unique_ptr<InputBuffer>> m_playerInputBuffers;
    
    // Packet handling
    std::queue<std::shared_ptr<NetworkPacket>> m_incomingPackets;
    std::queue<std::shared_ptr<NetworkPacket>> m_outgoingPackets;
    std::unordered_map<uint16_t, std::function<void(NetworkPacket*)>> m_packetHandlers;
    
    // Timing
    std::chrono::steady_clock::time_point m_lastTickTime;
    std::chrono::steady_clock::time_point m_lastSendTime;
    float m_tickAccumulator;
    float m_sendAccumulator;
    
    // Sequence tracking
    uint32_t m_sequenceNumber;
    uint32_t m_lastReceivedSequence;
    std::unordered_map<uint32_t, uint32_t> m_playerLastSequence;
    
    // Match state
    uint32_t m_currentMatchId;
    uint8_t m_currentGameMode;
    uint32_t m_randomSeed;
    
    // Callbacks
    OnPlayerConnectedCallback m_onPlayerConnected;
    OnPlayerDisconnectedCallback m_onPlayerDisconnected;
    OnMatchStartCallback m_onMatchStart;
    
    // Socket implementation (platform specific)
    void* m_socket;
    void* m_server;
};

} // namespace ArenaFighter