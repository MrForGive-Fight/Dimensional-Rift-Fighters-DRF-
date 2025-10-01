#pragma once

#include <cstdint>
#include <vector>
#include <memory>

namespace ArenaFighter {

// Packet type ranges following LSFDC standard
enum class PacketType : uint16_t {
    // Game State Packets (0x1000 - 0x1FFF)
    PlayerStateUpdate = 0x1001,
    InputCommand = 0x1002,
    InputPrediction = 0x1003,
    DeltaState = 0x1004,
    
    // Combat Packets (0x2000 - 0x2FFF)
    AttackEvent = 0x2001,
    DamageConfirmation = 0x2002,
    SkillActivation = 0x2003,
    ComboUpdate = 0x2004,
    
    // Match Packets (0x3000 - 0x3FFF)
    MatchStart = 0x3001,
    MatchEnd = 0x3002,
    PlayerJoined = 0x3003,
    PlayerLeft = 0x3004,
    MatchSync = 0x3005,
    
    // System Packets (0x4000 - 0x4FFF)
    Ping = 0x4001,
    Pong = 0x4002,
    Acknowledge = 0x4003,
    Disconnect = 0x4004
};

// Packet priority levels
enum class PacketPriority : uint8_t {
    Critical = 0,   // Input, damage, state changes
    Important = 1,  // Position updates, skill activations
    Normal = 2,     // Animation, effects, sounds
    Low = 3        // Statistics, non-gameplay data
};

// Packet flags
enum class PacketFlags : uint8_t {
    None = 0,
    Compressed = 1 << 0,
    Encrypted = 1 << 1,
    Reliable = 1 << 2,
    Ordered = 1 << 3,
    Urgent = 1 << 4
};

// LSFDC standard packet header
struct PacketHeader {
    uint16_t size;        // Total packet size including header
    uint16_t type;        // Packet type identifier
    uint32_t sequence;    // Sequence number for ordering
    uint32_t timestamp;   // Server timestamp
    uint8_t flags;        // Packet flags
    uint8_t version;      // Protocol version
    uint16_t checksum;    // Packet integrity check
    
    static constexpr uint8_t PROTOCOL_VERSION = 1;
};

// Base network packet class
class NetworkPacket {
public:
    NetworkPacket(PacketType type);
    virtual ~NetworkPacket() = default;
    
    // Serialization
    virtual void Serialize(std::vector<uint8_t>& buffer) const = 0;
    virtual void Deserialize(const uint8_t* data, size_t size) = 0;
    
    // Header access
    PacketHeader& GetHeader() { return m_header; }
    const PacketHeader& GetHeader() const { return m_header; }
    PacketType GetType() const { return static_cast<PacketType>(m_header.type); }
    PacketPriority GetPriority() const { return m_priority; }
    
    // Utility functions
    void SetSequence(uint32_t seq) { m_header.sequence = seq; }
    void SetTimestamp(uint32_t timestamp) { m_header.timestamp = timestamp; }
    void AddFlag(PacketFlags flag) { m_header.flags |= static_cast<uint8_t>(flag); }
    bool HasFlag(PacketFlags flag) const { return m_header.flags & static_cast<uint8_t>(flag); }
    
    // Calculate checksum for packet integrity
    uint16_t CalculateChecksum(const std::vector<uint8_t>& data) const;
    
protected:
    PacketHeader m_header;
    PacketPriority m_priority;
    
    // Helper functions for serialization
    void WriteHeader(std::vector<uint8_t>& buffer) const;
    void ReadHeader(const uint8_t* data);
};

// Game State Packets

class PlayerStatePacket : public NetworkPacket {
public:
    PlayerStatePacket() : NetworkPacket(PacketType::PlayerStateUpdate) {}
    
    void Serialize(std::vector<uint8_t>& buffer) const override;
    void Deserialize(const uint8_t* data, size_t size) override;
    
    uint32_t playerId;
    float position[3];
    float velocity[3];
    float rotation;
    uint16_t state;       // CharacterState enum
    uint16_t health;
    uint8_t mana;
    uint8_t currentGear;
};

class InputPacket : public NetworkPacket {
public:
    InputPacket() : NetworkPacket(PacketType::InputCommand) {
        m_priority = PacketPriority::Critical;
    }
    
    void Serialize(std::vector<uint8_t>& buffer) const override;
    void Deserialize(const uint8_t* data, size_t size) override;
    
    uint32_t playerId;
    uint32_t inputMask;   // InputCommand flags
    uint16_t inputId;     // Input sequence number
    uint16_t timestamp;   // Client timestamp
};

class InputPredictionPacket : public NetworkPacket {
public:
    InputPredictionPacket() : NetworkPacket(PacketType::InputPrediction) {
        m_priority = PacketPriority::Critical;
    }
    
    void Serialize(std::vector<uint8_t>& buffer) const override;
    void Deserialize(const uint8_t* data, size_t size) override;
    
    uint32_t playerId;
    uint16_t lastConfirmedInput;
    uint16_t predictedInputs[8];  // Up to 8 predicted inputs
};

class DeltaStatePacket : public NetworkPacket {
public:
    DeltaStatePacket() : NetworkPacket(PacketType::DeltaState) {}
    
    void Serialize(std::vector<uint8_t>& buffer) const override;
    void Deserialize(const uint8_t* data, size_t size) override;
    
    uint32_t playerId;
    uint8_t changedFields;     // Bitmask of changed fields
    
    // Optional fields based on changedFields bitmask
    float* position = nullptr;
    float* velocity = nullptr;
    float* rotation = nullptr;
    uint16_t* health = nullptr;
    uint8_t* mana = nullptr;
};

// Combat Packets

class AttackPacket : public NetworkPacket {
public:
    AttackPacket() : NetworkPacket(PacketType::AttackEvent) {
        m_priority = PacketPriority::Critical;
    }
    
    void Serialize(std::vector<uint8_t>& buffer) const override;
    void Deserialize(const uint8_t* data, size_t size) override;
    
    uint32_t attackerId;
    uint32_t targetId;
    uint16_t skillId;
    float damage;
    uint8_t hitType;      // Normal, Counter, Crush, etc.
    uint8_t comboCount;
    float position[3];    // Hit position
};

class DamagePacket : public NetworkPacket {
public:
    DamagePacket() : NetworkPacket(PacketType::DamageConfirmation) {
        m_priority = PacketPriority::Critical;
    }
    
    void Serialize(std::vector<uint8_t>& buffer) const override;
    void Deserialize(const uint8_t* data, size_t size) override;
    
    uint32_t targetId;
    float damageDealt;
    float remainingHealth;
    uint8_t hitReaction;  // Hitstun, blockstun, knockdown
    uint16_t stunFrames;
};

// Match Packets

class MatchStartPacket : public NetworkPacket {
public:
    MatchStartPacket() : NetworkPacket(PacketType::MatchStart) {}
    
    void Serialize(std::vector<uint8_t>& buffer) const override;
    void Deserialize(const uint8_t* data, size_t size) override;
    
    uint32_t matchId;
    uint32_t playerIds[8];     // Max 8 players
    uint8_t playerCount;
    uint8_t gameMode;
    uint8_t stageId;
    uint32_t randomSeed;       // For synchronized RNG
};

// Packet factory
class PacketFactory {
public:
    static std::unique_ptr<NetworkPacket> CreatePacket(PacketType type);
    static std::unique_ptr<NetworkPacket> CreateFromData(const uint8_t* data, size_t size);
};

} // namespace ArenaFighter