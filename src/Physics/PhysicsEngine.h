#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <DirectXMath.h>
#include "Collider.h"
#include "SpatialGrid.h"

namespace ArenaFighter {

// Forward declarations
class CharacterBase;

// LSFDC Physics Engine
class PhysicsEngine {
public:
    PhysicsEngine();
    ~PhysicsEngine();

    bool Initialize();
    void Shutdown();
    void Update(float deltaTime);

    // Collision Detection
    bool CheckCollision(const Collider* a, const Collider* b) const;
    std::vector<CollisionResult> CheckAllCollisions();
    
    // Collider Management
    void AddCollider(Collider* collider);
    void RemoveCollider(Collider* collider);
    void ClearColliders();
    
    // LSFDC Hit Detection
    HitResult ProcessHitDetection(CharacterBase* attacker, CharacterBase* defender);
    void ResolveHitPriority(const HitResult& hit1, const HitResult& hit2);
    
    // Movement and Physics
    void ApplyGravity(RigidBody* body, float deltaTime);
    void ProcessMovement(RigidBody* body, float deltaTime);
    void ApplyForce(RigidBody* body, const DirectX::XMFLOAT3& force);
    void ApplyImpulse(RigidBody* body, const DirectX::XMFLOAT3& impulse);
    
    // Character-specific Physics
    void ProcessCharacterMovement(CharacterBase* character, const DirectX::XMFLOAT2& input, float deltaTime);
    void ProcessJump(CharacterBase* character, float jumpForce);
    void ProcessAirDash(CharacterBase* character, const DirectX::XMFLOAT2& direction);
    
    // Combat Physics
    void ApplyHitstun(CharacterBase* character, int hitstunFrames);
    void ApplyBlockstun(CharacterBase* character, int blockstunFrames);
    void ApplyKnockback(CharacterBase* character, const DirectX::XMFLOAT2& knockback);
    void ProcessPushback(CharacterBase* attacker, CharacterBase* defender, float pushDistance);
    
    // Ground and Wall Detection
    bool IsGrounded(const CharacterBase* character) const;
    bool IsNearWall(const CharacterBase* character, float& wallDirection) const;
    float GetGroundHeight(float x) const;
    
    // Spatial Optimization
    std::vector<Collider*> GetNearbyColliders(const DirectX::XMFLOAT2& position, float radius) const;
    
    // Debug
    void EnableDebugDraw(bool enable) { m_debugDraw = enable; }
    void DrawDebugInfo();

private:
    // Collider storage
    std::vector<Collider*> m_colliders;
    std::vector<Collider*> m_staticColliders;
    std::vector<Collider*> m_dynamicColliders;
    
    // Spatial partitioning
    std::unique_ptr<SpatialGrid> m_spatialGrid;
    
    // Physics constants (LSFDC standards)
    static constexpr float GRAVITY = -1200.0f;           // Arcade gravity
    static constexpr float MAX_FALL_SPEED = -800.0f;    // Terminal velocity
    static constexpr float GROUND_FRICTION = 0.85f;      // Ground friction
    static constexpr float AIR_FRICTION = 0.95f;         // Air friction
    static constexpr float WALL_BOUNCE_FACTOR = 0.7f;   // Wall bounce
    static constexpr float PUSHBACK_FRICTION = 0.9f;     // Pushback deceleration
    
    // Stage boundaries
    static constexpr float STAGE_LEFT = -400.0f;
    static constexpr float STAGE_RIGHT = 400.0f;
    static constexpr float STAGE_GROUND = 0.0f;
    static constexpr float STAGE_CEILING = 600.0f;
    
    // Debug
    bool m_debugDraw;
    
    // Helper methods
    void UpdateSpatialGrid();
    void ProcessCollisionPair(Collider* a, Collider* b, std::vector<CollisionResult>& results);
    void ResolveCollision(const CollisionResult& result);
    bool CheckAABB(const AABB& a, const AABB& b) const;
    bool CheckCircleCircle(const DirectX::XMFLOAT2& pos1, float radius1, 
                          const DirectX::XMFLOAT2& pos2, float radius2) const;
    bool CheckBoxCircle(const AABB& box, const DirectX::XMFLOAT2& circlePos, float radius) const;
};

} // namespace ArenaFighter