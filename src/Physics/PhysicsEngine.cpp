#include "PhysicsEngine.h"
#include "../Characters/CharacterBase.h"
#include <algorithm>

namespace ArenaFighter {

PhysicsEngine::PhysicsEngine()
    : m_debugDraw(false) {
}

PhysicsEngine::~PhysicsEngine() {
    Shutdown();
}

bool PhysicsEngine::Initialize() {
    // Initialize spatial grid for optimization
    // Grid size based on typical fighting game stage dimensions
    m_spatialGrid = std::make_unique<SpatialGrid>(
        STAGE_LEFT - 100.0f,  // Extra padding
        STAGE_GROUND - 100.0f,
        STAGE_RIGHT + 100.0f,
        STAGE_CEILING + 100.0f,
        50.0f  // Cell size
    );
    
    return true;
}

void PhysicsEngine::Shutdown() {
    ClearColliders();
    m_spatialGrid.reset();
}

void PhysicsEngine::Update(float deltaTime) {
    // Update spatial grid
    UpdateSpatialGrid();
    
    // Process all collisions
    auto collisionResults = CheckAllCollisions();
    
    // Resolve collisions
    for (const auto& result : collisionResults) {
        ResolveCollision(result);
    }
    
    // Update physics for dynamic bodies
    for (auto* collider : m_dynamicColliders) {
        if (collider->GetRigidBody()) {
            ProcessMovement(collider->GetRigidBody(), deltaTime);
        }
    }
}

bool PhysicsEngine::CheckCollision(const Collider* a, const Collider* b) const {
    if (!a || !b || !a->IsActive() || !b->IsActive()) {
        return false;
    }
    
    // Get bounding boxes
    AABB boxA = a->GetAABB();
    AABB boxB = b->GetAABB();
    
    // Perform collision check based on collider types
    if (a->GetShape() == ColliderShape::Box && b->GetShape() == ColliderShape::Box) {
        return CheckAABB(boxA, boxB);
    } else if (a->GetShape() == ColliderShape::Circle && b->GetShape() == ColliderShape::Circle) {
        auto* circleA = static_cast<const CircleCollider*>(a);
        auto* circleB = static_cast<const CircleCollider*>(b);
        return CheckCircleCircle(
            circleA->GetPosition(), circleA->GetRadius(),
            circleB->GetPosition(), circleB->GetRadius()
        );
    } else {
        // Mixed box/circle collision
        if (a->GetShape() == ColliderShape::Box) {
            auto* circle = static_cast<const CircleCollider*>(b);
            return CheckBoxCircle(boxA, circle->GetPosition(), circle->GetRadius());
        } else {
            auto* circle = static_cast<const CircleCollider*>(a);
            return CheckBoxCircle(boxB, circle->GetPosition(), circle->GetRadius());
        }
    }
}

std::vector<CollisionResult> PhysicsEngine::CheckAllCollisions() {
    std::vector<CollisionResult> results;
    
    // Use spatial grid for broad phase
    auto activeCells = m_spatialGrid->GetActiveCells();
    
    for (const auto& cellColliders : activeCells) {
        // Check collisions within each cell
        for (size_t i = 0; i < cellColliders.size(); ++i) {
            for (size_t j = i + 1; j < cellColliders.size(); ++j) {
                ProcessCollisionPair(cellColliders[i], cellColliders[j], results);
            }
        }
    }
    
    return results;
}

void PhysicsEngine::AddCollider(Collider* collider) {
    if (!collider) return;
    
    m_colliders.push_back(collider);
    
    if (collider->GetRigidBody() && !collider->GetRigidBody()->IsKinematic()) {
        m_dynamicColliders.push_back(collider);
    } else {
        m_staticColliders.push_back(collider);
    }
}

void PhysicsEngine::RemoveCollider(Collider* collider) {
    if (!collider) return;
    
    auto removeFromVector = [collider](std::vector<Collider*>& vec) {
        vec.erase(std::remove(vec.begin(), vec.end(), collider), vec.end());
    };
    
    removeFromVector(m_colliders);
    removeFromVector(m_dynamicColliders);
    removeFromVector(m_staticColliders);
}

void PhysicsEngine::ClearColliders() {
    m_colliders.clear();
    m_dynamicColliders.clear();
    m_staticColliders.clear();
}

HitResult PhysicsEngine::ProcessHitDetection(CharacterBase* attacker, CharacterBase* defender) {
    HitResult result = {};
    
    if (!attacker || !defender) {
        return result;
    }
    
    // Get active hitboxes from attacker
    auto hitboxes = attacker->GetActiveHitboxes();
    auto hurtboxes = defender->GetHurtboxes();
    
    // Check each hitbox against each hurtbox
    for (const auto& hitbox : hitboxes) {
        for (const auto& hurtbox : hurtboxes) {
            if (CheckCollision(hitbox, hurtbox)) {
                result.hit = true;
                result.hitbox = hitbox;
                result.hurtbox = hurtbox;
                result.attacker = attacker;
                result.defender = defender;
                
                // Calculate hit properties
                result.damage = hitbox->GetDamage();
                result.hitstun = hitbox->GetHitstun();
                result.blockstun = hitbox->GetBlockstun();
                result.knockback = hitbox->GetKnockback();
                
                // Check for counter hit
                if (defender->IsInStartup()) {
                    result.isCounter = true;
                    result.hitstun = static_cast<int>(result.hitstun * 1.5f);
                }
                
                return result;  // Return first hit
            }
        }
    }
    
    return result;
}

void PhysicsEngine::ResolveHitPriority(const HitResult& hit1, const HitResult& hit2) {
    // LSFDC priority system
    if (hit1.hitbox->GetPriority() > hit2.hitbox->GetPriority()) {
        // Hit 1 wins
        hit1.defender->TakeDamage(hit1);
    } else if (hit2.hitbox->GetPriority() > hit1.hitbox->GetPriority()) {
        // Hit 2 wins
        hit2.defender->TakeDamage(hit2);
    } else {
        // Trade - both take damage
        hit1.defender->TakeDamage(hit1);
        hit2.defender->TakeDamage(hit2);
    }
}

void PhysicsEngine::ApplyGravity(RigidBody* body, float deltaTime) {
    if (!body || body->IsKinematic()) return;
    
    // Apply gravity
    body->velocity.y += GRAVITY * deltaTime;
    
    // Clamp to max fall speed
    if (body->velocity.y < MAX_FALL_SPEED) {
        body->velocity.y = MAX_FALL_SPEED;
    }
}

void PhysicsEngine::ProcessMovement(RigidBody* body, float deltaTime) {
    if (!body) return;
    
    // Apply gravity if not grounded
    if (!body->isGrounded) {
        ApplyGravity(body, deltaTime);
    }
    
    // Apply friction
    float friction = body->isGrounded ? GROUND_FRICTION : AIR_FRICTION;
    body->velocity.x *= friction;
    
    // Update position
    body->position.x += body->velocity.x * deltaTime;
    body->position.y += body->velocity.y * deltaTime;
    
    // Check stage boundaries
    if (body->position.x < STAGE_LEFT) {
        body->position.x = STAGE_LEFT;
        body->velocity.x = 0;
    } else if (body->position.x > STAGE_RIGHT) {
        body->position.x = STAGE_RIGHT;
        body->velocity.x = 0;
    }
    
    // Ground check
    if (body->position.y <= STAGE_GROUND) {
        body->position.y = STAGE_GROUND;
        body->velocity.y = 0;
        body->isGrounded = true;
    } else {
        body->isGrounded = false;
    }
    
    // Ceiling check
    if (body->position.y > STAGE_CEILING) {
        body->position.y = STAGE_CEILING;
        body->velocity.y = 0;
    }
}

void PhysicsEngine::ApplyForce(RigidBody* body, const DirectX::XMFLOAT3& force) {
    if (!body || body->IsKinematic()) return;
    
    body->velocity.x += force.x / body->mass;
    body->velocity.y += force.y / body->mass;
    body->velocity.z += force.z / body->mass;
}

void PhysicsEngine::ApplyImpulse(RigidBody* body, const DirectX::XMFLOAT3& impulse) {
    if (!body || body->IsKinematic()) return;
    
    body->velocity.x += impulse.x;
    body->velocity.y += impulse.y;
    body->velocity.z += impulse.z;
}

void PhysicsEngine::ProcessCharacterMovement(CharacterBase* character, const DirectX::XMFLOAT2& input, float deltaTime) {
    if (!character) return;
    
    RigidBody* body = character->GetRigidBody();
    if (!body) return;
    
    // Get character's movement speed
    float speed = character->GetMovementSpeed();
    
    // Apply movement
    if (body->isGrounded) {
        // Ground movement
        body->velocity.x = input.x * speed;
    } else {
        // Air movement (reduced control)
        body->velocity.x += input.x * speed * 0.3f * deltaTime;
    }
    
    // Face direction based on input
    if (input.x > 0.1f) {
        character->SetFacingDirection(1);
    } else if (input.x < -0.1f) {
        character->SetFacingDirection(-1);
    }
}

void PhysicsEngine::ProcessJump(CharacterBase* character, float jumpForce) {
    if (!character) return;
    
    RigidBody* body = character->GetRigidBody();
    if (!body || !body->isGrounded) return;
    
    // Apply jump impulse
    body->velocity.y = jumpForce;
    body->isGrounded = false;
}

void PhysicsEngine::ProcessAirDash(CharacterBase* character, const DirectX::XMFLOAT2& direction) {
    if (!character || !character->CanAirDash()) return;
    
    RigidBody* body = character->GetRigidBody();
    if (!body || body->isGrounded) return;
    
    // Normalize direction
    float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
    if (length > 0.0f) {
        DirectX::XMFLOAT2 normalizedDir = {
            direction.x / length,
            direction.y / length
        };
        
        // Apply air dash velocity
        const float AIR_DASH_SPEED = 600.0f;
        body->velocity.x = normalizedDir.x * AIR_DASH_SPEED;
        body->velocity.y = normalizedDir.y * AIR_DASH_SPEED * 0.5f;  // Reduced vertical dash
        
        character->ConsumeAirDash();
    }
}

void PhysicsEngine::ApplyHitstun(CharacterBase* character, int hitstunFrames) {
    if (!character) return;
    character->SetHitstun(hitstunFrames);
}

void PhysicsEngine::ApplyBlockstun(CharacterBase* character, int blockstunFrames) {
    if (!character) return;
    character->SetBlockstun(blockstunFrames);
}

void PhysicsEngine::ApplyKnockback(CharacterBase* character, const DirectX::XMFLOAT2& knockback) {
    if (!character) return;
    
    RigidBody* body = character->GetRigidBody();
    if (!body) return;
    
    // Apply knockback velocity
    body->velocity.x = knockback.x * character->GetFacingDirection() * -1;  // Opposite of attacker's facing
    body->velocity.y = knockback.y;
    
    // Force airborne if vertical knockback
    if (knockback.y > 0) {
        body->isGrounded = false;
    }
}

void PhysicsEngine::ProcessPushback(CharacterBase* attacker, CharacterBase* defender, float pushDistance) {
    if (!attacker || !defender) return;
    
    RigidBody* attackerBody = attacker->GetRigidBody();
    RigidBody* defenderBody = defender->GetRigidBody();
    
    if (!attackerBody || !defenderBody) return;
    
    // Calculate push direction
    float direction = (defenderBody->position.x > attackerBody->position.x) ? 1.0f : -1.0f;
    
    // Apply pushback (split between both characters if both grounded)
    if (attackerBody->isGrounded && defenderBody->isGrounded) {
        attackerBody->position.x -= direction * pushDistance * 0.5f;
        defenderBody->position.x += direction * pushDistance * 0.5f;
    } else if (defenderBody->isGrounded) {
        defenderBody->position.x += direction * pushDistance;
    } else if (attackerBody->isGrounded) {
        attackerBody->position.x -= direction * pushDistance;
    }
}

bool PhysicsEngine::IsGrounded(const CharacterBase* character) const {
    if (!character) return false;
    
    const RigidBody* body = character->GetRigidBody();
    return body ? body->isGrounded : false;
}

bool PhysicsEngine::IsNearWall(const CharacterBase* character, float& wallDirection) const {
    if (!character) return false;
    
    const RigidBody* body = character->GetRigidBody();
    if (!body) return false;
    
    const float WALL_CHECK_DISTANCE = 50.0f;
    
    if (body->position.x <= STAGE_LEFT + WALL_CHECK_DISTANCE) {
        wallDirection = -1.0f;
        return true;
    } else if (body->position.x >= STAGE_RIGHT - WALL_CHECK_DISTANCE) {
        wallDirection = 1.0f;
        return true;
    }
    
    return false;
}

float PhysicsEngine::GetGroundHeight(float x) const {
    // For now, flat ground. Can be extended for uneven terrain
    return STAGE_GROUND;
}

std::vector<Collider*> PhysicsEngine::GetNearbyColliders(const DirectX::XMFLOAT2& position, float radius) const {
    return m_spatialGrid->GetCollidersInRadius(position, radius);
}

void PhysicsEngine::DrawDebugInfo() {
    if (!m_debugDraw) return;
    
    // Debug drawing would be implemented here
    // This would interface with the rendering system
}

void PhysicsEngine::UpdateSpatialGrid() {
    m_spatialGrid->Clear();
    
    for (auto* collider : m_colliders) {
        if (collider->IsActive()) {
            m_spatialGrid->Insert(collider);
        }
    }
}

void PhysicsEngine::ProcessCollisionPair(Collider* a, Collider* b, std::vector<CollisionResult>& results) {
    // Check collision layers
    if (!a->CanCollideWith(b->GetLayer()) || !b->CanCollideWith(a->GetLayer())) {
        return;
    }
    
    if (CheckCollision(a, b)) {
        CollisionResult result;
        result.colliderA = a;
        result.colliderB = b;
        result.penetrationDepth = 0.0f;  // Calculate actual penetration
        result.normal = {0, 0};  // Calculate collision normal
        
        results.push_back(result);
    }
}

void PhysicsEngine::ResolveCollision(const CollisionResult& result) {
    // Handle different collision types
    if (result.colliderA->GetType() == CollisionType::Hitbox && 
        result.colliderB->GetType() == CollisionType::Hurtbox) {
        // Combat collision - handled by combat system
        return;
    }
    
    if (result.colliderA->GetType() == CollisionType::Pushbox && 
        result.colliderB->GetType() == CollisionType::Pushbox) {
        // Character vs character push
        // Separate characters to prevent overlap
        RigidBody* bodyA = result.colliderA->GetRigidBody();
        RigidBody* bodyB = result.colliderB->GetRigidBody();
        
        if (bodyA && bodyB) {
            float separation = result.penetrationDepth * 0.5f;
            bodyA->position.x -= result.normal.x * separation;
            bodyB->position.x += result.normal.x * separation;
        }
    }
}

bool PhysicsEngine::CheckAABB(const AABB& a, const AABB& b) const {
    // LSFDC overlap tolerance
    const float OVERLAP_TOLERANCE = 1.0f;
    
    return !(a.min.x > b.max.x - OVERLAP_TOLERANCE ||
             a.max.x < b.min.x + OVERLAP_TOLERANCE ||
             a.min.y > b.max.y - OVERLAP_TOLERANCE ||
             a.max.y < b.min.y + OVERLAP_TOLERANCE);
}

bool PhysicsEngine::CheckCircleCircle(const DirectX::XMFLOAT2& pos1, float radius1, 
                                     const DirectX::XMFLOAT2& pos2, float radius2) const {
    float dx = pos2.x - pos1.x;
    float dy = pos2.y - pos1.y;
    float distSq = dx * dx + dy * dy;
    float radiusSum = radius1 + radius2;
    
    return distSq <= radiusSum * radiusSum;
}

bool PhysicsEngine::CheckBoxCircle(const AABB& box, const DirectX::XMFLOAT2& circlePos, float radius) const {
    // Find closest point on box to circle center
    float closestX = std::max(box.min.x, std::min(circlePos.x, box.max.x));
    float closestY = std::max(box.min.y, std::min(circlePos.y, box.max.y));
    
    // Calculate distance from circle center to closest point
    float dx = circlePos.x - closestX;
    float dy = circlePos.y - closestY;
    
    return (dx * dx + dy * dy) <= radius * radius;
}

} // namespace ArenaFighter