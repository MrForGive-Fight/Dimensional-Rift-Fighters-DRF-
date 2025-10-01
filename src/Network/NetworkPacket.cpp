#include "NetworkPacket.h"
#include <cstring>
#include <algorithm>

namespace ArenaFighter {

// Base NetworkPacket implementation

NetworkPacket::NetworkPacket(PacketType type) : m_priority(PacketPriority::Normal) {
    std::memset(&m_header, 0, sizeof(m_header));
    m_header.type = static_cast<uint16_t>(type);
    m_header.version = PacketHeader::PROTOCOL_VERSION;
}

uint16_t NetworkPacket::CalculateChecksum(const std::vector<uint8_t>& data) const {
    uint32_t sum = 0;
    
    // Skip checksum field in calculation
    size_t checksumOffset = offsetof(PacketHeader, checksum);
    
    for (size_t i = 0; i < data.size(); ++i) {
        if (i >= checksumOffset && i < checksumOffset + sizeof(uint16_t)) {
            continue; // Skip checksum field
        }
        sum += data[i];
    }
    
    // Fold 32-bit sum to 16 bits
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    return static_cast<uint16_t>(~sum);
}

void NetworkPacket::WriteHeader(std::vector<uint8_t>& buffer) const {
    size_t startSize = buffer.size();
    buffer.resize(startSize + sizeof(PacketHeader));
    std::memcpy(buffer.data() + startSize, &m_header, sizeof(PacketHeader));
}

void NetworkPacket::ReadHeader(const uint8_t* data) {
    std::memcpy(&m_header, data, sizeof(PacketHeader));
}

// PlayerStatePacket implementation

void PlayerStatePacket::Serialize(std::vector<uint8_t>& buffer) const {
    // Reserve space for efficiency
    buffer.reserve(sizeof(PacketHeader) + 64);
    
    // Write header
    WriteHeader(buffer);
    
    // Write packet data
    size_t offset = buffer.size();
    buffer.resize(offset + sizeof(playerId) + sizeof(position) + sizeof(velocity) + 
                 sizeof(rotation) + sizeof(state) + sizeof(health) + 
                 sizeof(mana) + sizeof(currentGear));
    
    uint8_t* ptr = buffer.data() + offset;
    
    std::memcpy(ptr, &playerId, sizeof(playerId)); ptr += sizeof(playerId);
    std::memcpy(ptr, position, sizeof(position)); ptr += sizeof(position);
    std::memcpy(ptr, velocity, sizeof(velocity)); ptr += sizeof(velocity);
    std::memcpy(ptr, &rotation, sizeof(rotation)); ptr += sizeof(rotation);
    std::memcpy(ptr, &state, sizeof(state)); ptr += sizeof(state);
    std::memcpy(ptr, &health, sizeof(health)); ptr += sizeof(health);
    std::memcpy(ptr, &mana, sizeof(mana)); ptr += sizeof(mana);
    std::memcpy(ptr, &currentGear, sizeof(currentGear));
    
    // Update packet size in header
    const_cast<PacketHeader&>(m_header).size = static_cast<uint16_t>(buffer.size());
}

void PlayerStatePacket::Deserialize(const uint8_t* data, size_t size) {
    if (size < sizeof(PacketHeader)) return;
    
    ReadHeader(data);
    
    const uint8_t* ptr = data + sizeof(PacketHeader);
    size_t remaining = size - sizeof(PacketHeader);
    
    if (remaining < sizeof(playerId) + sizeof(position) + sizeof(velocity) + 
                   sizeof(rotation) + sizeof(state) + sizeof(health) + 
                   sizeof(mana) + sizeof(currentGear)) {
        return; // Invalid packet
    }
    
    std::memcpy(&playerId, ptr, sizeof(playerId)); ptr += sizeof(playerId);
    std::memcpy(position, ptr, sizeof(position)); ptr += sizeof(position);
    std::memcpy(velocity, ptr, sizeof(velocity)); ptr += sizeof(velocity);
    std::memcpy(&rotation, ptr, sizeof(rotation)); ptr += sizeof(rotation);
    std::memcpy(&state, ptr, sizeof(state)); ptr += sizeof(state);
    std::memcpy(&health, ptr, sizeof(health)); ptr += sizeof(health);
    std::memcpy(&mana, ptr, sizeof(mana)); ptr += sizeof(mana);
    std::memcpy(&currentGear, ptr, sizeof(currentGear));
}

// InputPacket implementation

void InputPacket::Serialize(std::vector<uint8_t>& buffer) const {
    buffer.reserve(sizeof(PacketHeader) + 16);
    
    WriteHeader(buffer);
    
    size_t offset = buffer.size();
    buffer.resize(offset + sizeof(playerId) + sizeof(inputMask) + 
                 sizeof(inputId) + sizeof(timestamp));
    
    uint8_t* ptr = buffer.data() + offset;
    
    std::memcpy(ptr, &playerId, sizeof(playerId)); ptr += sizeof(playerId);
    std::memcpy(ptr, &inputMask, sizeof(inputMask)); ptr += sizeof(inputMask);
    std::memcpy(ptr, &inputId, sizeof(inputId)); ptr += sizeof(inputId);
    std::memcpy(ptr, &timestamp, sizeof(timestamp));
    
    const_cast<PacketHeader&>(m_header).size = static_cast<uint16_t>(buffer.size());
}

void InputPacket::Deserialize(const uint8_t* data, size_t size) {
    if (size < sizeof(PacketHeader)) return;
    
    ReadHeader(data);
    
    const uint8_t* ptr = data + sizeof(PacketHeader);
    size_t remaining = size - sizeof(PacketHeader);
    
    if (remaining < sizeof(playerId) + sizeof(inputMask) + 
                   sizeof(inputId) + sizeof(timestamp)) {
        return;
    }
    
    std::memcpy(&playerId, ptr, sizeof(playerId)); ptr += sizeof(playerId);
    std::memcpy(&inputMask, ptr, sizeof(inputMask)); ptr += sizeof(inputMask);
    std::memcpy(&inputId, ptr, sizeof(inputId)); ptr += sizeof(inputId);
    std::memcpy(&timestamp, ptr, sizeof(timestamp));
}

// InputPredictionPacket implementation

void InputPredictionPacket::Serialize(std::vector<uint8_t>& buffer) const {
    buffer.reserve(sizeof(PacketHeader) + 32);
    
    WriteHeader(buffer);
    
    size_t offset = buffer.size();
    buffer.resize(offset + sizeof(playerId) + sizeof(lastConfirmedInput) + 
                 sizeof(predictedInputs));
    
    uint8_t* ptr = buffer.data() + offset;
    
    std::memcpy(ptr, &playerId, sizeof(playerId)); ptr += sizeof(playerId);
    std::memcpy(ptr, &lastConfirmedInput, sizeof(lastConfirmedInput)); ptr += sizeof(lastConfirmedInput);
    std::memcpy(ptr, predictedInputs, sizeof(predictedInputs));
    
    const_cast<PacketHeader&>(m_header).size = static_cast<uint16_t>(buffer.size());
}

void InputPredictionPacket::Deserialize(const uint8_t* data, size_t size) {
    if (size < sizeof(PacketHeader)) return;
    
    ReadHeader(data);
    
    const uint8_t* ptr = data + sizeof(PacketHeader);
    size_t remaining = size - sizeof(PacketHeader);
    
    if (remaining < sizeof(playerId) + sizeof(lastConfirmedInput) + 
                   sizeof(predictedInputs)) {
        return;
    }
    
    std::memcpy(&playerId, ptr, sizeof(playerId)); ptr += sizeof(playerId);
    std::memcpy(&lastConfirmedInput, ptr, sizeof(lastConfirmedInput)); ptr += sizeof(lastConfirmedInput);
    std::memcpy(predictedInputs, ptr, sizeof(predictedInputs));
}

// DeltaStatePacket implementation

void DeltaStatePacket::Serialize(std::vector<uint8_t>& buffer) const {
    buffer.reserve(sizeof(PacketHeader) + 64);
    
    WriteHeader(buffer);
    
    size_t offset = buffer.size();
    buffer.resize(offset + sizeof(playerId) + sizeof(changedFields));
    
    uint8_t* ptr = buffer.data() + offset;
    
    std::memcpy(ptr, &playerId, sizeof(playerId)); ptr += sizeof(playerId);
    std::memcpy(ptr, &changedFields, sizeof(changedFields)); ptr += sizeof(changedFields);
    
    // Write only changed fields
    if (changedFields & 0x01 && position) {
        size_t currentSize = buffer.size();
        buffer.resize(currentSize + sizeof(float) * 3);
        ptr = buffer.data() + currentSize;
        std::memcpy(ptr, position, sizeof(float) * 3);
    }
    
    if (changedFields & 0x02 && velocity) {
        size_t currentSize = buffer.size();
        buffer.resize(currentSize + sizeof(float) * 3);
        ptr = buffer.data() + currentSize;
        std::memcpy(ptr, velocity, sizeof(float) * 3);
    }
    
    if (changedFields & 0x04 && rotation) {
        size_t currentSize = buffer.size();
        buffer.resize(currentSize + sizeof(float));
        ptr = buffer.data() + currentSize;
        std::memcpy(ptr, rotation, sizeof(float));
    }
    
    if (changedFields & 0x08 && health) {
        size_t currentSize = buffer.size();
        buffer.resize(currentSize + sizeof(uint16_t));
        ptr = buffer.data() + currentSize;
        std::memcpy(ptr, health, sizeof(uint16_t));
    }
    
    if (changedFields & 0x10 && mana) {
        size_t currentSize = buffer.size();
        buffer.resize(currentSize + sizeof(uint8_t));
        ptr = buffer.data() + currentSize;
        std::memcpy(ptr, mana, sizeof(uint8_t));
    }
    
    const_cast<PacketHeader&>(m_header).size = static_cast<uint16_t>(buffer.size());
}

void DeltaStatePacket::Deserialize(const uint8_t* data, size_t size) {
    if (size < sizeof(PacketHeader)) return;
    
    ReadHeader(data);
    
    const uint8_t* ptr = data + sizeof(PacketHeader);
    size_t remaining = size - sizeof(PacketHeader);
    
    if (remaining < sizeof(playerId) + sizeof(changedFields)) {
        return;
    }
    
    std::memcpy(&playerId, ptr, sizeof(playerId)); ptr += sizeof(playerId);
    std::memcpy(&changedFields, ptr, sizeof(changedFields)); ptr += sizeof(changedFields);
    remaining -= sizeof(playerId) + sizeof(changedFields);
    
    // Read only changed fields
    // Note: In real implementation, we'd allocate memory for these
    // For now, we skip the data to move ptr forward
    
    if (changedFields & 0x01) { // position
        if (remaining >= sizeof(float) * 3) {
            ptr += sizeof(float) * 3;
            remaining -= sizeof(float) * 3;
        }
    }
    
    if (changedFields & 0x02) { // velocity
        if (remaining >= sizeof(float) * 3) {
            ptr += sizeof(float) * 3;
            remaining -= sizeof(float) * 3;
        }
    }
    
    if (changedFields & 0x04) { // rotation
        if (remaining >= sizeof(float)) {
            ptr += sizeof(float);
            remaining -= sizeof(float);
        }
    }
    
    if (changedFields & 0x08) { // health
        if (remaining >= sizeof(uint16_t)) {
            ptr += sizeof(uint16_t);
            remaining -= sizeof(uint16_t);
        }
    }
    
    if (changedFields & 0x10) { // mana
        if (remaining >= sizeof(uint8_t)) {
            ptr += sizeof(uint8_t);
            remaining -= sizeof(uint8_t);
        }
    }
}

// AttackPacket implementation

void AttackPacket::Serialize(std::vector<uint8_t>& buffer) const {
    buffer.reserve(sizeof(PacketHeader) + 48);
    
    WriteHeader(buffer);
    
    size_t offset = buffer.size();
    buffer.resize(offset + sizeof(attackerId) + sizeof(targetId) + sizeof(skillId) +
                 sizeof(damage) + sizeof(hitType) + sizeof(comboCount) + sizeof(position));
    
    uint8_t* ptr = buffer.data() + offset;
    
    std::memcpy(ptr, &attackerId, sizeof(attackerId)); ptr += sizeof(attackerId);
    std::memcpy(ptr, &targetId, sizeof(targetId)); ptr += sizeof(targetId);
    std::memcpy(ptr, &skillId, sizeof(skillId)); ptr += sizeof(skillId);
    std::memcpy(ptr, &damage, sizeof(damage)); ptr += sizeof(damage);
    std::memcpy(ptr, &hitType, sizeof(hitType)); ptr += sizeof(hitType);
    std::memcpy(ptr, &comboCount, sizeof(comboCount)); ptr += sizeof(comboCount);
    std::memcpy(ptr, position, sizeof(position));
    
    const_cast<PacketHeader&>(m_header).size = static_cast<uint16_t>(buffer.size());
}

void AttackPacket::Deserialize(const uint8_t* data, size_t size) {
    if (size < sizeof(PacketHeader)) return;
    
    ReadHeader(data);
    
    const uint8_t* ptr = data + sizeof(PacketHeader);
    size_t remaining = size - sizeof(PacketHeader);
    
    if (remaining < sizeof(attackerId) + sizeof(targetId) + sizeof(skillId) +
                   sizeof(damage) + sizeof(hitType) + sizeof(comboCount) + sizeof(position)) {
        return;
    }
    
    std::memcpy(&attackerId, ptr, sizeof(attackerId)); ptr += sizeof(attackerId);
    std::memcpy(&targetId, ptr, sizeof(targetId)); ptr += sizeof(targetId);
    std::memcpy(&skillId, ptr, sizeof(skillId)); ptr += sizeof(skillId);
    std::memcpy(&damage, ptr, sizeof(damage)); ptr += sizeof(damage);
    std::memcpy(&hitType, ptr, sizeof(hitType)); ptr += sizeof(hitType);
    std::memcpy(&comboCount, ptr, sizeof(comboCount)); ptr += sizeof(comboCount);
    std::memcpy(position, ptr, sizeof(position));
}

// DamagePacket implementation

void DamagePacket::Serialize(std::vector<uint8_t>& buffer) const {
    buffer.reserve(sizeof(PacketHeader) + 24);
    
    WriteHeader(buffer);
    
    size_t offset = buffer.size();
    buffer.resize(offset + sizeof(targetId) + sizeof(damageDealt) + 
                 sizeof(remainingHealth) + sizeof(hitReaction) + sizeof(stunFrames));
    
    uint8_t* ptr = buffer.data() + offset;
    
    std::memcpy(ptr, &targetId, sizeof(targetId)); ptr += sizeof(targetId);
    std::memcpy(ptr, &damageDealt, sizeof(damageDealt)); ptr += sizeof(damageDealt);
    std::memcpy(ptr, &remainingHealth, sizeof(remainingHealth)); ptr += sizeof(remainingHealth);
    std::memcpy(ptr, &hitReaction, sizeof(hitReaction)); ptr += sizeof(hitReaction);
    std::memcpy(ptr, &stunFrames, sizeof(stunFrames));
    
    const_cast<PacketHeader&>(m_header).size = static_cast<uint16_t>(buffer.size());
}

void DamagePacket::Deserialize(const uint8_t* data, size_t size) {
    if (size < sizeof(PacketHeader)) return;
    
    ReadHeader(data);
    
    const uint8_t* ptr = data + sizeof(PacketHeader);
    size_t remaining = size - sizeof(PacketHeader);
    
    if (remaining < sizeof(targetId) + sizeof(damageDealt) + 
                   sizeof(remainingHealth) + sizeof(hitReaction) + sizeof(stunFrames)) {
        return;
    }
    
    std::memcpy(&targetId, ptr, sizeof(targetId)); ptr += sizeof(targetId);
    std::memcpy(&damageDealt, ptr, sizeof(damageDealt)); ptr += sizeof(damageDealt);
    std::memcpy(&remainingHealth, ptr, sizeof(remainingHealth)); ptr += sizeof(remainingHealth);
    std::memcpy(&hitReaction, ptr, sizeof(hitReaction)); ptr += sizeof(hitReaction);
    std::memcpy(&stunFrames, ptr, sizeof(stunFrames));
}

// MatchStartPacket implementation

void MatchStartPacket::Serialize(std::vector<uint8_t>& buffer) const {
    buffer.reserve(sizeof(PacketHeader) + 64);
    
    WriteHeader(buffer);
    
    size_t offset = buffer.size();
    buffer.resize(offset + sizeof(matchId) + sizeof(playerIds) + sizeof(playerCount) +
                 sizeof(gameMode) + sizeof(stageId) + sizeof(randomSeed));
    
    uint8_t* ptr = buffer.data() + offset;
    
    std::memcpy(ptr, &matchId, sizeof(matchId)); ptr += sizeof(matchId);
    std::memcpy(ptr, playerIds, sizeof(playerIds)); ptr += sizeof(playerIds);
    std::memcpy(ptr, &playerCount, sizeof(playerCount)); ptr += sizeof(playerCount);
    std::memcpy(ptr, &gameMode, sizeof(gameMode)); ptr += sizeof(gameMode);
    std::memcpy(ptr, &stageId, sizeof(stageId)); ptr += sizeof(stageId);
    std::memcpy(ptr, &randomSeed, sizeof(randomSeed));
    
    const_cast<PacketHeader&>(m_header).size = static_cast<uint16_t>(buffer.size());
}

void MatchStartPacket::Deserialize(const uint8_t* data, size_t size) {
    if (size < sizeof(PacketHeader)) return;
    
    ReadHeader(data);
    
    const uint8_t* ptr = data + sizeof(PacketHeader);
    size_t remaining = size - sizeof(PacketHeader);
    
    if (remaining < sizeof(matchId) + sizeof(playerIds) + sizeof(playerCount) +
                   sizeof(gameMode) + sizeof(stageId) + sizeof(randomSeed)) {
        return;
    }
    
    std::memcpy(&matchId, ptr, sizeof(matchId)); ptr += sizeof(matchId);
    std::memcpy(playerIds, ptr, sizeof(playerIds)); ptr += sizeof(playerIds);
    std::memcpy(&playerCount, ptr, sizeof(playerCount)); ptr += sizeof(playerCount);
    std::memcpy(&gameMode, ptr, sizeof(gameMode)); ptr += sizeof(gameMode);
    std::memcpy(&stageId, ptr, sizeof(stageId)); ptr += sizeof(stageId);
    std::memcpy(&randomSeed, ptr, sizeof(randomSeed));
}

// PacketFactory implementation

std::unique_ptr<NetworkPacket> PacketFactory::CreatePacket(PacketType type) {
    switch (type) {
        case PacketType::PlayerStateUpdate:
            return std::make_unique<PlayerStatePacket>();
        case PacketType::InputCommand:
            return std::make_unique<InputPacket>();
        case PacketType::InputPrediction:
            return std::make_unique<InputPredictionPacket>();
        case PacketType::DeltaState:
            return std::make_unique<DeltaStatePacket>();
        case PacketType::AttackEvent:
            return std::make_unique<AttackPacket>();
        case PacketType::DamageConfirmation:
            return std::make_unique<DamagePacket>();
        case PacketType::MatchStart:
            return std::make_unique<MatchStartPacket>();
        default:
            return nullptr;
    }
}

std::unique_ptr<NetworkPacket> PacketFactory::CreateFromData(const uint8_t* data, size_t size) {
    if (size < sizeof(PacketHeader)) {
        return nullptr;
    }
    
    PacketHeader header;
    std::memcpy(&header, data, sizeof(header));
    
    auto packet = CreatePacket(static_cast<PacketType>(header.type));
    if (packet) {
        packet->Deserialize(data, size);
    }
    
    return packet;
}

} // namespace ArenaFighter