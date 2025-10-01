#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace ArenaFighter {

enum class NetworkMode {
    Offline,
    Host,
    Client
};

struct PlayerInput {
    uint32_t frame;
    uint8_t buttons;
    float moveX, moveY;
    float lookX, lookY;
};

struct NetworkStats {
    int ping = 0;
    float packetLoss = 0.0f;
    int rollbackFrames = 0;
};

class NetworkLayer {
public:
    NetworkLayer() = default;
    ~NetworkLayer() = default;

    bool Initialize();
    void Shutdown();
    void Update(float deltaTime);

    // Connection Management
    bool StartHost(int port);
    bool ConnectToHost(const std::string& address, int port);
    void Disconnect();

    // Rollback Netcode
    void SendInput(const PlayerInput& input);
    bool GetRemoteInput(int playerID, PlayerInput& input);
    void ConfirmFrame(uint32_t frame);
    
    // Match Management
    void CreateRoom(const std::string& roomName, int maxPlayers);
    void JoinRoom(const std::string& roomCode);
    void StartMatch();
    
    // Network Stats
    NetworkStats GetNetworkStats() const { return m_stats; }
    int GetPlayerCount() const { return m_playerCount; }
    
    // Callbacks
    using OnPlayerJoinedCallback = std::function<void(int playerID)>;
    using OnPlayerLeftCallback = std::function<void(int playerID)>;
    void SetPlayerJoinedCallback(OnPlayerJoinedCallback cb) { m_onPlayerJoined = cb; }
    void SetPlayerLeftCallback(OnPlayerLeftCallback cb) { m_onPlayerLeft = cb; }

private:
    NetworkMode m_mode = NetworkMode::Offline;
    NetworkStats m_stats;
    int m_playerCount = 1;
    
    // Rollback buffer
    static constexpr int ROLLBACK_FRAMES = 7;
    std::vector<PlayerInput> m_inputBuffer;
    
    // Callbacks
    OnPlayerJoinedCallback m_onPlayerJoined;
    OnPlayerLeftCallback m_onPlayerLeft;
    
    // Network implementation details
    void* m_socket = nullptr;
    void* m_server = nullptr;
};

} // namespace ArenaFighter