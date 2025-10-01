# LSFDC Network Packet Structure

## Base Packet Format

All packets follow the LSFDC standard structure:

```cpp
struct PacketHeader {
    uint16_t size;        // Total packet size including header
    uint16_t type;        // Packet type identifier
    uint32_t sequence;    // Sequence number for ordering
    uint32_t timestamp;   // Server timestamp
    uint8_t  flags;       // Packet flags (compressed, encrypted, etc.)
    uint8_t  version;     // Protocol version
    uint16_t checksum;    // Packet integrity check
};

struct BasePacket {
    PacketHeader header;
    uint8_t data[];       // Variable length data
};
```

## Packet Types

### Game State Packets (0x1000 - 0x1FFF)

#### Player State Update (0x1001)
```cpp
struct PlayerStatePacket {
    PacketHeader header;
    uint32_t playerId;
    float position[3];
    float velocity[3];
    float rotation;
    uint16_t state;       // CharacterState enum
    uint16_t health;
    uint8_t mana;
    uint8_t currentGear;
};
```

#### Input Command (0x1002)
```cpp
struct InputPacket {
    PacketHeader header;
    uint32_t playerId;
    uint32_t inputMask;   // InputCommand flags
    uint16_t inputId;     // Input sequence number
    uint16_t timestamp;   // Client timestamp
};
```

### Combat Packets (0x2000 - 0x2FFF)

#### Attack Event (0x2001)
```cpp
struct AttackPacket {
    PacketHeader header;
    uint32_t attackerId;
    uint32_t targetId;
    uint16_t skillId;
    float damage;
    uint8_t hitType;      // Normal, Counter, Crush, etc.
    uint8_t comboCount;
    float position[3];    // Hit position
};
```

#### Damage Confirmation (0x2002)
```cpp
struct DamagePacket {
    PacketHeader header;
    uint32_t targetId;
    float damageDealt;
    float remainingHealth;
    uint8_t hitReaction;  // Hitstun, blockstun, knockdown
    uint16_t stunFrames;
};
```

### Match Packets (0x3000 - 0x3FFF)

#### Match Start (0x3001)
```cpp
struct MatchStartPacket {
    PacketHeader header;
    uint32_t matchId;
    uint32_t playerIds[8];     // Max 8 players
    uint8_t gameMode;
    uint8_t stageId;
    uint32_t randomSeed;       // For synchronized RNG
};
```

## Optimization Techniques

### Delta Compression
Only send changed values to reduce bandwidth:
```cpp
struct DeltaStatePacket {
    PacketHeader header;
    uint32_t playerId;
    uint8_t changedFields;     // Bitmask of changed fields
    // Only included fields marked in changedFields
};
```

### Input Prediction
Client-side prediction with server reconciliation:
```cpp
struct InputPredictionPacket {
    PacketHeader header;
    uint32_t playerId;
    uint16_t lastConfirmedInput;
    uint16_t predictedInputs[8];  // Up to 8 predicted inputs
};
```

### Priority System
Packets are prioritized based on gameplay importance:
1. **Critical** (Priority 0): Input, damage, state changes
2. **Important** (Priority 1): Position updates, skill activations
3. **Normal** (Priority 2): Animation, effects, sounds
4. **Low** (Priority 3): Statistics, non-gameplay data

## Network Configuration

### Recommended Settings
```json
{
  "tick_rate": 60,
  "send_rate": 30,
  "interpolation_delay": 100,
  "max_prediction_frames": 8,
  "packet_size_limit": 1400,
  "compression_threshold": 256
}
```

### Lag Compensation
- Input buffering: 3-5 frames
- Rollback frames: 7 frames max
- Interpolation: 100ms delay
- Extrapolation limit: 200ms

## Security Considerations

1. **Packet Validation**
   - Checksum verification
   - Sequence number validation
   - Timestamp sanity checks
   - Input rate limiting

2. **Anti-Cheat Measures**
   - Server-authoritative gameplay
   - Input validation
   - Position sanity checks
   - Damage calculation verification

3. **Encryption**
   - TLS for lobby/matchmaking
   - Custom XOR for gameplay packets
   - Session-based keys