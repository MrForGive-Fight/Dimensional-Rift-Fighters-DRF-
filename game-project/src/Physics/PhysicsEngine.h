#pragma once

#include <vector>
#include <memory>
#include <DirectXMath.h>

namespace ArenaFighter {

struct CollisionBox {
    DirectX::XMFLOAT3 min;
    DirectX::XMFLOAT3 max;
    bool isTrigger = false;
};

struct HitBox : CollisionBox {
    float damage = 0.0f;
    float hitstun = 0.0f;
    float blockstun = 0.0f;
    DirectX::XMFLOAT3 knockback;
};

class RigidBody {
public:
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 velocity;
    DirectX::XMFLOAT3 acceleration;
    float mass = 1.0f;
    bool isKinematic = false;
};

class PhysicsEngine {
public:
    PhysicsEngine() = default;
    ~PhysicsEngine() = default;

    bool Initialize();
    void Shutdown();
    void Update(float deltaTime);

    // Collision Detection
    bool CheckCollision(const CollisionBox& a, const CollisionBox& b);
    bool CheckHitboxCollision(const HitBox& attack, const CollisionBox& target);
    
    // Movement
    void ApplyForce(RigidBody* body, const DirectX::XMFLOAT3& force);
    void ApplyImpulse(RigidBody* body, const DirectX::XMFLOAT3& impulse);
    
    // Character Physics
    void ProcessCharacterMovement(RigidBody* body, const DirectX::XMFLOAT3& input, float speed);
    void ProcessJump(RigidBody* body, float jumpForce);
    void ProcessAirDash(RigidBody* body, const DirectX::XMFLOAT3& direction, float dashSpeed);

    // Combat Physics
    void ApplyHitstun(RigidBody* body, float duration);
    void ApplyKnockback(RigidBody* body, const DirectX::XMFLOAT3& knockback);
    
    // Ground Detection
    bool IsGrounded(const RigidBody* body);

private:
    std::vector<RigidBody*> m_bodies;
    
    // Physics constants
    static constexpr float GRAVITY = -9.81f * 2.0f; // Arcade-style gravity
    static constexpr float AIR_FRICTION = 0.95f;
    static constexpr float GROUND_FRICTION = 0.85f;
};

} // namespace ArenaFighter