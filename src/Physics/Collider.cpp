#include "Collider.h"
#include <cmath>

namespace ArenaFighter {

// Base Collider Implementation
Collider::Collider()
    : m_type(CollisionType::Pushbox)
    , m_layer(CollisionLayer::Default)
    , m_layerMask(static_cast<int>(CollisionLayer::All))
    , m_active(true)
    , m_isTrigger(false)
    , m_offset(0, 0)
    , m_rigidBody(nullptr)
    , m_damage(0.0f)
    , m_hitstun(0)
    , m_blockstun(0)
    , m_knockback(0, 0)
    , m_priority(0) {
}

// BoxCollider Implementation
BoxCollider::BoxCollider()
    : m_center(0, 0)
    , m_width(50.0f)
    , m_height(50.0f) {
}

BoxCollider::BoxCollider(float width, float height)
    : m_center(0, 0)
    , m_width(width)
    , m_height(height) {
}

BoxCollider::BoxCollider(const DirectX::XMFLOAT2& center, float width, float height)
    : m_center(center)
    , m_width(width)
    , m_height(height) {
}

AABB BoxCollider::GetAABB() const {
    DirectX::XMFLOAT2 actualCenter = m_center;
    
    // Apply offset if rigid body exists
    if (m_rigidBody) {
        actualCenter.x = m_rigidBody->position.x + m_center.x + m_offset.x;
        actualCenter.y = m_rigidBody->position.y + m_center.y + m_offset.y;
    } else {
        actualCenter.x += m_offset.x;
        actualCenter.y += m_offset.y;
    }
    
    AABB aabb;
    aabb.min.x = actualCenter.x - m_width * 0.5f;
    aabb.min.y = actualCenter.y - m_height * 0.5f;
    aabb.max.x = actualCenter.x + m_width * 0.5f;
    aabb.max.y = actualCenter.y + m_height * 0.5f;
    
    return aabb;
}

bool BoxCollider::Contains(const DirectX::XMFLOAT2& point) const {
    AABB aabb = GetAABB();
    return point.x >= aabb.min.x && point.x <= aabb.max.x &&
           point.y >= aabb.min.y && point.y <= aabb.max.y;
}

// CircleCollider Implementation
CircleCollider::CircleCollider()
    : m_position(0, 0)
    , m_radius(25.0f) {
}

CircleCollider::CircleCollider(float radius)
    : m_position(0, 0)
    , m_radius(radius) {
}

CircleCollider::CircleCollider(const DirectX::XMFLOAT2& center, float radius)
    : m_position(center)
    , m_radius(radius) {
}

AABB CircleCollider::GetAABB() const {
    DirectX::XMFLOAT2 actualPosition = m_position;
    
    // Apply offset if rigid body exists
    if (m_rigidBody) {
        actualPosition.x = m_rigidBody->position.x + m_position.x + m_offset.x;
        actualPosition.y = m_rigidBody->position.y + m_position.y + m_offset.y;
    } else {
        actualPosition.x += m_offset.x;
        actualPosition.y += m_offset.y;
    }
    
    AABB aabb;
    aabb.min.x = actualPosition.x - m_radius;
    aabb.min.y = actualPosition.y - m_radius;
    aabb.max.x = actualPosition.x + m_radius;
    aabb.max.y = actualPosition.y + m_radius;
    
    return aabb;
}

bool CircleCollider::Contains(const DirectX::XMFLOAT2& point) const {
    DirectX::XMFLOAT2 actualPosition = m_position;
    
    // Apply offset if rigid body exists
    if (m_rigidBody) {
        actualPosition.x = m_rigidBody->position.x + m_position.x + m_offset.x;
        actualPosition.y = m_rigidBody->position.y + m_position.y + m_offset.y;
    } else {
        actualPosition.x += m_offset.x;
        actualPosition.y += m_offset.y;
    }
    
    float dx = point.x - actualPosition.x;
    float dy = point.y - actualPosition.y;
    float distanceSq = dx * dx + dy * dy;
    
    return distanceSq <= m_radius * m_radius;
}

} // namespace ArenaFighter