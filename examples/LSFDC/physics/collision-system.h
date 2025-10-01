// LSFDC Physics Collision System
// Based on Lost Saga collision detection patterns

#pragma once

#include <vector>
#include <array>

namespace LSFDC {
namespace Physics {

// Collision box types used in LSFDC
enum class CollisionType {
    Hurtbox,      // Can be hit
    Hitbox,       // Can hit others
    Pushbox,      // Physical collision
    Throwbox,     // Grab collision
    Projectile,   // Projectile collision
    Environmental // Stage collision
};

// LSFDC standard collision box
struct CollisionBox {
    float x, y;           // Center position (relative to character)
    float width, height;  // Box dimensions
    CollisionType type;
    int priority;         // Hit priority (higher wins)
    bool active;
    
    // LSFDC box interpolation for smooth animation
    CollisionBox interpolated;
    float interpolationFactor;
};

// Character collision data following LSFDC standards
class CharacterCollision {
public:
    // LSFDC standard: 5 hurtboxes max
    static constexpr int MAX_HURTBOXES = 5;
    // LSFDC standard: 3 active hitboxes max
    static constexpr int MAX_HITBOXES = 3;
    
    CharacterCollision() {
        InitializeStandardBoxes();
    }
    
    // Update collision boxes based on animation frame
    void UpdateBoxes(int animationFrame, const std::string& animationName) {
        // LSFDC frame data lookup
        auto frameData = GetFrameData(animationName, animationFrame);
        
        // Update hurtboxes
        for (int i = 0; i < MAX_HURTBOXES; ++i) {
            if (frameData.hurtboxData[i].active) {
                hurtboxes[i] = frameData.hurtboxData[i];
            }
        }
        
        // Update hitboxes
        for (int i = 0; i < MAX_HITBOXES; ++i) {
            if (frameData.hitboxData[i].active) {
                hitboxes[i] = frameData.hitboxData[i];
                hitboxes[i].active = true;
            } else {
                hitboxes[i].active = false;
            }
        }
    }
    
    // LSFDC collision detection algorithm
    bool CheckCollision(const CollisionBox& box1, const CollisionBox& box2) {
        // AABB collision with LSFDC adjustments
        float left1 = box1.x - box1.width / 2.0f;
        float right1 = box1.x + box1.width / 2.0f;
        float top1 = box1.y + box1.height / 2.0f;
        float bottom1 = box1.y - box1.height / 2.0f;
        
        float left2 = box2.x - box2.width / 2.0f;
        float right2 = box2.x + box2.width / 2.0f;
        float top2 = box2.y + box2.height / 2.0f;
        float bottom2 = box2.y - box2.height / 2.0f;
        
        // LSFDC overlap tolerance (1 pixel)
        const float OVERLAP_TOLERANCE = 1.0f;
        
        return !(left1 > right2 - OVERLAP_TOLERANCE ||
                 right1 < left2 + OVERLAP_TOLERANCE ||
                 top1 < bottom2 + OVERLAP_TOLERANCE ||
                 bottom1 > top2 - OVERLAP_TOLERANCE);
    }
    
    // Get active hitboxes
    std::vector<CollisionBox*> GetActiveHitboxes() {
        std::vector<CollisionBox*> active;
        for (auto& box : hitboxes) {
            if (box.active) {
                active.push_back(&box);
            }
        }
        return active;
    }
    
    // Get all hurtboxes
    std::array<CollisionBox, MAX_HURTBOXES>& GetHurtboxes() {
        return hurtboxes;
    }
    
private:
    std::array<CollisionBox, MAX_HURTBOXES> hurtboxes;
    std::array<CollisionBox, MAX_HITBOXES> hitboxes;
    CollisionBox pushbox;
    CollisionBox throwbox;
    
    // Initialize standard LSFDC collision boxes
    void InitializeStandardBoxes() {
        // Standing hurtboxes (LSFDC standard layout)
        hurtboxes[0] = {0, 60, 40, 40, CollisionType::Hurtbox, 0, true};  // Head
        hurtboxes[1] = {0, 30, 50, 60, CollisionType::Hurtbox, 0, true};  // Body
        hurtboxes[2] = {0, -10, 40, 40, CollisionType::Hurtbox, 0, true}; // Legs
        hurtboxes[3] = {0, 0, 0, 0, CollisionType::Hurtbox, 0, false};    // Extra 1
        hurtboxes[4] = {0, 0, 0, 0, CollisionType::Hurtbox, 0, false};    // Extra 2
        
        // Pushbox (LSFDC standard size)
        pushbox = {0, 0, 30, 80, CollisionType::Pushbox, 0, true};
        
        // Throwbox (slightly larger than pushbox)
        throwbox = {15, 30, 60, 60, CollisionType::Throwbox, 0, false};
    }
    
    // Frame data structure for LSFDC animations
    struct FrameData {
        CollisionBox hurtboxData[MAX_HURTBOXES];
        CollisionBox hitboxData[MAX_HITBOXES];
    };
    
    // Get frame data for specific animation and frame
    FrameData GetFrameData(const std::string& animation, int frame) {
        // This would typically load from animation data files
        // For now, return default data
        FrameData data;
        
        // Copy current hurtboxes
        for (int i = 0; i < MAX_HURTBOXES; ++i) {
            data.hurtboxData[i] = hurtboxes[i];
        }
        
        // Clear hitboxes by default
        for (int i = 0; i < MAX_HITBOXES; ++i) {
            data.hitboxData[i] = {0, 0, 0, 0, CollisionType::Hitbox, 0, false};
        }
        
        return data;
    }
};

// LSFDC hit detection result
struct HitResult {
    bool hit;
    float damage;
    int hitstun;
    float knockbackX;
    float knockbackY;
    CollisionBox* hitbox;
    CollisionBox* hurtbox;
    bool isCounter;
    bool isTradeHit;
};

// LSFDC priority system for simultaneous hits
class HitPrioritySystem {
public:
    // Resolve hit priority between two attacks
    static HitResult ResolveHitPriority(
        const CollisionBox& hitbox1, 
        const CollisionBox& hitbox2,
        const CharacterCollision& char1,
        const CharacterCollision& char2
    ) {
        HitResult result = {};
        
        // Check priority levels
        if (hitbox1.priority > hitbox2.priority) {
            // Player 1 wins
            result.hit = true;
            result.hitbox = const_cast<CollisionBox*>(&hitbox1);
            // Find colliding hurtbox
            for (auto& hurtbox : char2.GetHurtboxes()) {
                if (char1.CheckCollision(hitbox1, hurtbox)) {
                    result.hurtbox = &hurtbox;
                    break;
                }
            }
        } else if (hitbox2.priority > hitbox1.priority) {
            // Player 2 wins
            result.hit = true;
            result.hitbox = const_cast<CollisionBox*>(&hitbox2);
            // Find colliding hurtbox
            for (auto& hurtbox : char1.GetHurtboxes()) {
                if (char2.CheckCollision(hitbox2, hurtbox)) {
                    result.hurtbox = &hurtbox;
                    break;
                }
            }
        } else {
            // Same priority - trade hit
            result.isTradeHit = true;
        }
        
        return result;
    }
};

} // namespace Physics
} // namespace LSFDC