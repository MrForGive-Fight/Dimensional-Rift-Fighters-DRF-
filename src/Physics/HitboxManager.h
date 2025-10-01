#pragma once

#include <vector>
#include <array>
#include <string>
#include <unordered_map>
#include "Collider.h"

namespace ArenaFighter {

// LSFDC standard hitbox/hurtbox manager
class HitboxManager {
public:
    // LSFDC standards
    static constexpr int MAX_HURTBOXES = 5;
    static constexpr int MAX_HITBOXES = 3;
    static constexpr int MAX_THROWBOXES = 1;
    
    HitboxManager();
    ~HitboxManager();
    
    // Initialize with character-specific data
    void Initialize(const std::string& characterName);
    
    // Update boxes based on animation state
    void UpdateBoxes(const std::string& animationName, int frame, int facingDirection = 1);
    
    // Get active collision boxes
    std::vector<BoxCollider*> GetActiveHitboxes() const;
    std::vector<BoxCollider*> GetHurtboxes() const;
    BoxCollider* GetPushbox() const { return m_pushbox.get(); }
    BoxCollider* GetThrowbox() const { return m_throwbox.get(); }
    
    // Enable/disable specific boxes
    void EnableHitbox(int index, bool enable);
    void EnableThrowbox(bool enable);
    void SetInvulnerable(bool invulnerable);
    
    // Set owner rigid body (for position updates)
    void SetRigidBody(RigidBody* body);
    
    // Frame data structures
    struct HitboxData {
        float x, y;           // Relative position
        float width, height;  // Size
        float damage;         // Damage amount
        int hitstun;         // Hitstun frames
        int blockstun;       // Blockstun frames
        DirectX::XMFLOAT2 knockback;  // Knockback force
        int priority;        // Hit priority
        bool active;         // Is active this frame
    };
    
    struct HurtboxData {
        float x, y;
        float width, height;
        bool active;
        bool invulnerable;   // Frame-specific invulnerability
    };
    
    struct FrameData {
        HitboxData hitboxes[MAX_HITBOXES];
        HurtboxData hurtboxes[MAX_HURTBOXES];
        bool throwboxActive;
        DirectX::XMFLOAT2 pushboxOffset;  // Pushbox position adjustment
    };
    
    // Load animation data
    void LoadAnimationData(const std::string& animationName, const std::vector<FrameData>& frames);
    
private:
    // Collision boxes
    std::array<std::unique_ptr<BoxCollider>, MAX_HURTBOXES> m_hurtboxes;
    std::array<std::unique_ptr<BoxCollider>, MAX_HITBOXES> m_hitboxes;
    std::unique_ptr<BoxCollider> m_pushbox;
    std::unique_ptr<BoxCollider> m_throwbox;
    
    // Animation data storage
    std::unordered_map<std::string, std::vector<FrameData>> m_animationData;
    
    // Current state
    std::string m_currentAnimation;
    int m_currentFrame;
    int m_facingDirection;
    bool m_invulnerable;
    RigidBody* m_rigidBody;
    
    // Character-specific base positions
    std::string m_characterName;
    
    // Initialize standard boxes based on character type
    void InitializeStandardBoxes();
    
    // Apply frame data to boxes
    void ApplyFrameData(const FrameData& frameData);
    
    // Flip box positions for facing direction
    void ApplyFacingDirection(float& x);
};

// Standard box configurations for different character archetypes
namespace StandardBoxes {
    // Small character (90 units tall)
    const HitboxManager::FrameData SMALL_IDLE = {
        // Hitboxes (none active in idle)
        {
            {0, 0, 0, 0, 0, 0, 0, {0, 0}, 0, false},
            {0, 0, 0, 0, 0, 0, 0, {0, 0}, 0, false},
            {0, 0, 0, 0, 0, 0, 0, {0, 0}, 0, false}
        },
        // Hurtboxes
        {
            {0, 50, 35, 30, true, false},   // Head
            {0, 25, 40, 40, true, false},   // Body
            {0, 0, 35, 30, true, false},    // Legs
            {0, 0, 0, 0, false, false},     // Extra 1
            {0, 0, 0, 0, false, false}      // Extra 2
        },
        false,  // Throwbox not active
        {0, 0}  // No pushbox offset
    };
    
    // Medium character (100 units tall)
    const HitboxManager::FrameData MEDIUM_IDLE = {
        // Hitboxes (none active in idle)
        {
            {0, 0, 0, 0, 0, 0, 0, {0, 0}, 0, false},
            {0, 0, 0, 0, 0, 0, 0, {0, 0}, 0, false},
            {0, 0, 0, 0, 0, 0, 0, {0, 0}, 0, false}
        },
        // Hurtboxes
        {
            {0, 60, 40, 35, true, false},   // Head
            {0, 30, 50, 50, true, false},   // Body
            {0, 0, 40, 35, true, false},    // Legs
            {0, 0, 0, 0, false, false},     // Extra 1
            {0, 0, 0, 0, false, false}      // Extra 2
        },
        false,  // Throwbox not active
        {0, 0}  // No pushbox offset
    };
    
    // Large character (120 units tall)
    const HitboxManager::FrameData LARGE_IDLE = {
        // Hitboxes (none active in idle)
        {
            {0, 0, 0, 0, 0, 0, 0, {0, 0}, 0, false},
            {0, 0, 0, 0, 0, 0, 0, {0, 0}, 0, false},
            {0, 0, 0, 0, 0, 0, 0, {0, 0}, 0, false}
        },
        // Hurtboxes
        {
            {0, 75, 50, 40, true, false},   // Head
            {0, 40, 60, 60, true, false},   // Body
            {0, 0, 50, 40, true, false},    // Legs
            {0, 0, 0, 0, false, false},     // Extra 1
            {0, 0, 0, 0, false, false}      // Extra 2
        },
        false,  // Throwbox not active
        {0, 0}  // No pushbox offset
    };
}

} // namespace ArenaFighter