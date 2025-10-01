#pragma once

#include <vector>
#include <memory>

namespace ArenaFighter {

// Forward declarations
struct Vector3 {
    float x, y, z;
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct AABB {
    Vector3 min;
    Vector3 max;
    
    bool Intersects(const AABB& other) const {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
               (min.y <= other.max.y && max.y >= other.min.y) &&
               (min.z <= other.max.z && max.z >= other.min.z);
    }
};

/**
 * @brief Hitbox for attacks
 */
class HitBox {
public:
    HitBox();
    HitBox(const Vector3& position, const Vector3& size);
    
    void SetPosition(const Vector3& pos) { m_position = pos; UpdateBounds(); }
    void SetSize(const Vector3& size) { m_size = size; UpdateBounds(); }
    void SetActive(bool active) { m_active = active; }
    
    const Vector3& GetPosition() const { return m_position; }
    const Vector3& GetSize() const { return m_size; }
    const AABB& GetBounds() const { return m_bounds; }
    bool IsActive() const { return m_active; }
    
    // Hit properties
    void SetHitID(int id) { m_hitID = id; }
    int GetHitID() const { return m_hitID; }
    
private:
    Vector3 m_position;
    Vector3 m_size;
    AABB m_bounds;
    bool m_active = false;
    int m_hitID = -1;  // Unique ID to prevent multi-hits
    
    void UpdateBounds();
};

/**
 * @brief Hurtbox for receiving hits
 */
class HurtBox {
public:
    HurtBox();
    HurtBox(const Vector3& position, const Vector3& size);
    
    void SetPosition(const Vector3& pos) { m_position = pos; UpdateBounds(); }
    void SetSize(const Vector3& size) { m_size = size; UpdateBounds(); }
    void SetInvulnerable(bool invuln) { m_invulnerable = invuln; }
    
    const Vector3& GetPosition() const { return m_position; }
    const Vector3& GetSize() const { return m_size; }
    const AABB& GetBounds() const { return m_bounds; }
    bool IsInvulnerable() const { return m_invulnerable; }
    
private:
    Vector3 m_position;
    Vector3 m_size;
    AABB m_bounds;
    bool m_invulnerable = false;
    
    void UpdateBounds();
};

/**
 * @brief Frame-perfect hit detection system following LSFDC patterns
 */
class HitDetection {
public:
    HitDetection();
    ~HitDetection();
    
    bool Initialize();
    void Shutdown();
    
    // Core hit detection
    bool CheckCollision(const HitBox& hitbox, const HurtBox& hurtbox, 
                       float activeFrames, float currentFrame);
    
    // Active hitbox management
    void RegisterActiveHitbox(int ownerId, const HitBox& hitbox, int duration);
    void UpdateActiveHitboxes(float deltaTime);
    void ClearActiveHitboxes(int ownerId);
    
    // Hit confirmation system
    bool HasAlreadyHit(int hitboxId, int targetId) const;
    void RegisterHit(int hitboxId, int targetId);
    void ClearHitRegistry(int hitboxId);
    
    // Priority system
    enum class HitPriority {
        Low,
        Medium,
        High,
        Super
    };
    
    HitPriority ResolveClash(HitPriority attack1, HitPriority attack2);
    
    // Frame window helpers
    bool IsWithinActiveWindow(float activeFrames, float currentFrame) const;
    float GetInterpolatedPosition(float startFrame, float endFrame, float currentFrame) const;

private:
    struct ActiveHitbox {
        int ownerId;
        HitBox hitbox;
        int framesRemaining;
        std::vector<int> hitTargets;  // Targets already hit
    };
    
    struct HitDetectionImpl;
    std::unique_ptr<HitDetectionImpl> m_impl;
    
    // Internal collision detection
    bool PerformAABBTest(const AABB& box1, const AABB& box2) const;
    bool PerformPreciseTest(const HitBox& hitbox, const HurtBox& hurtbox) const;
};

} // namespace ArenaFighter