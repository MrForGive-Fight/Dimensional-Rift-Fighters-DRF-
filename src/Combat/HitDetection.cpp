#include "HitDetection.h"
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

namespace ArenaFighter {

// HitBox implementation
HitBox::HitBox() : m_position(0, 0, 0), m_size(1, 1, 1) {
    UpdateBounds();
}

HitBox::HitBox(const Vector3& position, const Vector3& size) 
    : m_position(position), m_size(size) {
    UpdateBounds();
}

void HitBox::UpdateBounds() {
    Vector3 halfSize(m_size.x * 0.5f, m_size.y * 0.5f, m_size.z * 0.5f);
    m_bounds.min = Vector3(m_position.x - halfSize.x, 
                          m_position.y - halfSize.y, 
                          m_position.z - halfSize.z);
    m_bounds.max = Vector3(m_position.x + halfSize.x, 
                          m_position.y + halfSize.y, 
                          m_position.z + halfSize.z);
}

// HurtBox implementation
HurtBox::HurtBox() : m_position(0, 0, 0), m_size(1, 1, 1) {
    UpdateBounds();
}

HurtBox::HurtBox(const Vector3& position, const Vector3& size) 
    : m_position(position), m_size(size) {
    UpdateBounds();
}

void HurtBox::UpdateBounds() {
    Vector3 halfSize(m_size.x * 0.5f, m_size.y * 0.5f, m_size.z * 0.5f);
    m_bounds.min = Vector3(m_position.x - halfSize.x, 
                          m_position.y - halfSize.y, 
                          m_position.z - halfSize.z);
    m_bounds.max = Vector3(m_position.x + halfSize.x, 
                          m_position.y + halfSize.y, 
                          m_position.z + halfSize.z);
}

// HitDetection implementation
struct HitDetection::HitDetectionImpl {
    std::vector<ActiveHitbox> activeHitboxes;
    std::unordered_map<int, std::unordered_set<int>> hitRegistry;  // hitboxId -> set of targetIds
    int nextHitboxId = 1;
};

HitDetection::HitDetection() : m_impl(std::make_unique<HitDetectionImpl>()) {
}

HitDetection::~HitDetection() = default;

bool HitDetection::Initialize() {
    m_impl->activeHitboxes.clear();
    m_impl->hitRegistry.clear();
    return true;
}

void HitDetection::Shutdown() {
    m_impl->activeHitboxes.clear();
    m_impl->hitRegistry.clear();
}

bool HitDetection::CheckCollision(const HitBox& hitbox, const HurtBox& hurtbox,
                                 float activeFrames, float currentFrame) {
    // Check if hitbox is active
    if (!hitbox.IsActive()) {
        return false;
    }
    
    // Check if hurtbox is invulnerable
    if (hurtbox.IsInvulnerable()) {
        return false;
    }
    
    // Check if within active window
    if (!IsWithinActiveWindow(activeFrames, currentFrame)) {
        return false;
    }
    
    // Perform AABB collision test
    if (!PerformAABBTest(hitbox.GetBounds(), hurtbox.GetBounds())) {
        return false;
    }
    
    // Perform precise collision test (can be expanded for non-box shapes)
    return PerformPreciseTest(hitbox, hurtbox);
}

void HitDetection::RegisterActiveHitbox(int ownerId, const HitBox& hitbox, int duration) {
    ActiveHitbox active;
    active.ownerId = ownerId;
    active.hitbox = hitbox;
    active.hitbox.SetHitID(m_impl->nextHitboxId++);
    active.framesRemaining = duration;
    
    m_impl->activeHitboxes.push_back(active);
}

void HitDetection::UpdateActiveHitboxes(float deltaTime) {
    int framesToUpdate = static_cast<int>(deltaTime * 60.0f); // 60 FPS
    
    // Update and remove expired hitboxes
    auto it = m_impl->activeHitboxes.begin();
    while (it != m_impl->activeHitboxes.end()) {
        it->framesRemaining -= framesToUpdate;
        
        if (it->framesRemaining <= 0) {
            // Clear hit registry for this hitbox
            ClearHitRegistry(it->hitbox.GetHitID());
            it = m_impl->activeHitboxes.erase(it);
        } else {
            ++it;
        }
    }
}

void HitDetection::ClearActiveHitboxes(int ownerId) {
    auto it = m_impl->activeHitboxes.begin();
    while (it != m_impl->activeHitboxes.end()) {
        if (it->ownerId == ownerId) {
            ClearHitRegistry(it->hitbox.GetHitID());
            it = m_impl->activeHitboxes.erase(it);
        } else {
            ++it;
        }
    }
}

bool HitDetection::HasAlreadyHit(int hitboxId, int targetId) const {
    auto it = m_impl->hitRegistry.find(hitboxId);
    if (it != m_impl->hitRegistry.end()) {
        return it->second.find(targetId) != it->second.end();
    }
    return false;
}

void HitDetection::RegisterHit(int hitboxId, int targetId) {
    m_impl->hitRegistry[hitboxId].insert(targetId);
}

void HitDetection::ClearHitRegistry(int hitboxId) {
    m_impl->hitRegistry.erase(hitboxId);
}

HitDetection::HitPriority HitDetection::ResolveClash(HitPriority attack1, HitPriority attack2) {
    // LSFDC priority system: higher priority wins
    if (attack1 == attack2) {
        return HitPriority::Medium;  // Trade
    }
    
    return (static_cast<int>(attack1) > static_cast<int>(attack2)) ? attack1 : attack2;
}

bool HitDetection::IsWithinActiveWindow(float activeFrames, float currentFrame) const {
    return currentFrame >= 0.0f && currentFrame <= activeFrames;
}

float HitDetection::GetInterpolatedPosition(float startFrame, float endFrame, float currentFrame) const {
    if (currentFrame <= startFrame) return 0.0f;
    if (currentFrame >= endFrame) return 1.0f;
    
    float duration = endFrame - startFrame;
    if (duration <= 0.0f) return 1.0f;
    
    return (currentFrame - startFrame) / duration;
}

bool HitDetection::PerformAABBTest(const AABB& box1, const AABB& box2) const {
    return box1.Intersects(box2);
}

bool HitDetection::PerformPreciseTest(const HitBox& hitbox, const HurtBox& hurtbox) const {
    // For now, AABB test is sufficient
    // This can be expanded for more complex shapes (spheres, capsules, etc.)
    return true;
}

} // namespace ArenaFighter