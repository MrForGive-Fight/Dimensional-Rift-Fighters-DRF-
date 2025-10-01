#pragma once

#include <DirectXMath.h>
#include <vector>
#include <functional>

namespace ArenaFighter {

// Forward declarations
class RigidBody;

// LSFDC Collision Types
enum class CollisionType {
    Hurtbox,      // Can be hit
    Hitbox,       // Can hit others  
    Pushbox,      // Physical collision
    Throwbox,     // Grab collision
    Projectile,   // Projectile collision
    Environmental // Stage collision
};

// Collision layers for filtering
enum class CollisionLayer {
    Default = 0,
    Player = 1,
    Enemy = 2,
    Projectile = 4,
    Environment = 8,
    Trigger = 16,
    All = 0xFFFF
};

// Collider shapes
enum class ColliderShape {
    Box,
    Circle
};

// Axis-Aligned Bounding Box
struct AABB {
    DirectX::XMFLOAT2 min;
    DirectX::XMFLOAT2 max;
    
    AABB() : min(0, 0), max(0, 0) {}
    AABB(const DirectX::XMFLOAT2& minPoint, const DirectX::XMFLOAT2& maxPoint)
        : min(minPoint), max(maxPoint) {}
    
    DirectX::XMFLOAT2 GetCenter() const {
        return DirectX::XMFLOAT2(
            (min.x + max.x) * 0.5f,
            (min.y + max.y) * 0.5f
        );
    }
    
    DirectX::XMFLOAT2 GetSize() const {
        return DirectX::XMFLOAT2(
            max.x - min.x,
            max.y - min.y
        );
    }
};

// Base Collider class
class Collider {
public:
    Collider();
    virtual ~Collider() = default;
    
    // Pure virtual methods
    virtual AABB GetAABB() const = 0;
    virtual ColliderShape GetShape() const = 0;
    virtual bool Contains(const DirectX::XMFLOAT2& point) const = 0;
    
    // Common properties
    void SetType(CollisionType type) { m_type = type; }
    CollisionType GetType() const { return m_type; }
    
    void SetLayer(CollisionLayer layer) { m_layer = layer; }
    CollisionLayer GetLayer() const { return m_layer; }
    
    void SetLayerMask(int mask) { m_layerMask = mask; }
    int GetLayerMask() const { return m_layerMask; }
    
    bool CanCollideWith(CollisionLayer otherLayer) const {
        return (m_layerMask & static_cast<int>(otherLayer)) != 0;
    }
    
    void SetActive(bool active) { m_active = active; }
    bool IsActive() const { return m_active; }
    
    void SetTrigger(bool trigger) { m_isTrigger = trigger; }
    bool IsTrigger() const { return m_isTrigger; }
    
    void SetOffset(const DirectX::XMFLOAT2& offset) { m_offset = offset; }
    DirectX::XMFLOAT2 GetOffset() const { return m_offset; }
    
    void SetRigidBody(RigidBody* body) { m_rigidBody = body; }
    RigidBody* GetRigidBody() const { return m_rigidBody; }
    
    // Combat properties (for hitboxes)
    void SetDamage(float damage) { m_damage = damage; }
    float GetDamage() const { return m_damage; }
    
    void SetHitstun(int frames) { m_hitstun = frames; }
    int GetHitstun() const { return m_hitstun; }
    
    void SetBlockstun(int frames) { m_blockstun = frames; }
    int GetBlockstun() const { return m_blockstun; }
    
    void SetKnockback(const DirectX::XMFLOAT2& knockback) { m_knockback = knockback; }
    DirectX::XMFLOAT2 GetKnockback() const { return m_knockback; }
    
    void SetPriority(int priority) { m_priority = priority; }
    int GetPriority() const { return m_priority; }
    
    // Callbacks
    using CollisionCallback = std::function<void(Collider*, Collider*)>;
    void SetOnCollisionEnter(CollisionCallback callback) { m_onCollisionEnter = callback; }
    void SetOnCollisionStay(CollisionCallback callback) { m_onCollisionStay = callback; }
    void SetOnCollisionExit(CollisionCallback callback) { m_onCollisionExit = callback; }
    
    void OnCollisionEnter(Collider* other) {
        if (m_onCollisionEnter) m_onCollisionEnter(this, other);
    }
    
    void OnCollisionStay(Collider* other) {
        if (m_onCollisionStay) m_onCollisionStay(this, other);
    }
    
    void OnCollisionExit(Collider* other) {
        if (m_onCollisionExit) m_onCollisionExit(this, other);
    }

protected:
    CollisionType m_type;
    CollisionLayer m_layer;
    int m_layerMask;
    bool m_active;
    bool m_isTrigger;
    DirectX::XMFLOAT2 m_offset;
    RigidBody* m_rigidBody;
    
    // Combat properties
    float m_damage;
    int m_hitstun;
    int m_blockstun;
    DirectX::XMFLOAT2 m_knockback;
    int m_priority;
    
    // Callbacks
    CollisionCallback m_onCollisionEnter;
    CollisionCallback m_onCollisionStay;
    CollisionCallback m_onCollisionExit;
};

// Box Collider
class BoxCollider : public Collider {
public:
    BoxCollider();
    BoxCollider(float width, float height);
    BoxCollider(const DirectX::XMFLOAT2& center, float width, float height);
    
    void SetSize(float width, float height) { 
        m_width = width; 
        m_height = height; 
    }
    
    void SetCenter(const DirectX::XMFLOAT2& center) { m_center = center; }
    DirectX::XMFLOAT2 GetCenter() const { return m_center; }
    
    float GetWidth() const { return m_width; }
    float GetHeight() const { return m_height; }
    
    // Overrides
    AABB GetAABB() const override;
    ColliderShape GetShape() const override { return ColliderShape::Box; }
    bool Contains(const DirectX::XMFLOAT2& point) const override;
    
private:
    DirectX::XMFLOAT2 m_center;
    float m_width;
    float m_height;
};

// Circle Collider
class CircleCollider : public Collider {
public:
    CircleCollider();
    CircleCollider(float radius);
    CircleCollider(const DirectX::XMFLOAT2& center, float radius);
    
    void SetRadius(float radius) { m_radius = radius; }
    float GetRadius() const { return m_radius; }
    
    void SetPosition(const DirectX::XMFLOAT2& position) { m_position = position; }
    DirectX::XMFLOAT2 GetPosition() const { return m_position; }
    
    // Overrides
    AABB GetAABB() const override;
    ColliderShape GetShape() const override { return ColliderShape::Circle; }
    bool Contains(const DirectX::XMFLOAT2& point) const override;
    
private:
    DirectX::XMFLOAT2 m_position;
    float m_radius;
};

// Collision result structure
struct CollisionResult {
    Collider* colliderA;
    Collider* colliderB;
    DirectX::XMFLOAT2 normal;
    float penetrationDepth;
    DirectX::XMFLOAT2 contactPoint;
};

// Hit result for combat
struct HitResult {
    bool hit;
    float damage;
    int hitstun;
    int blockstun;
    DirectX::XMFLOAT2 knockback;
    Collider* hitbox;
    Collider* hurtbox;
    class CharacterBase* attacker;
    class CharacterBase* defender;
    bool isCounter;
    bool isTradeHit;
};

// Rigid body for physics
class RigidBody {
public:
    RigidBody() 
        : position(0, 0, 0)
        , velocity(0, 0, 0)
        , acceleration(0, 0, 0)
        , mass(1.0f)
        , isKinematic(false)
        , isGrounded(false)
        , useGravity(true) {}
    
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 velocity;
    DirectX::XMFLOAT3 acceleration;
    float mass;
    bool isKinematic;
    bool isGrounded;
    bool useGravity;
    
    bool IsKinematic() const { return isKinematic; }
    void SetKinematic(bool kinematic) { isKinematic = kinematic; }
};

} // namespace ArenaFighter