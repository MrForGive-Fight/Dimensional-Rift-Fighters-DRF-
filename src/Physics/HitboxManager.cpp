#include "HitboxManager.h"
#include <algorithm>

namespace ArenaFighter {

HitboxManager::HitboxManager()
    : m_currentFrame(0)
    , m_facingDirection(1)
    , m_invulnerable(false)
    , m_rigidBody(nullptr) {
    
    // Create collision boxes
    for (int i = 0; i < MAX_HURTBOXES; ++i) {
        m_hurtboxes[i] = std::make_unique<BoxCollider>();
        m_hurtboxes[i]->SetType(CollisionType::Hurtbox);
        m_hurtboxes[i]->SetLayer(CollisionLayer::Player);
    }
    
    for (int i = 0; i < MAX_HITBOXES; ++i) {
        m_hitboxes[i] = std::make_unique<BoxCollider>();
        m_hitboxes[i]->SetType(CollisionType::Hitbox);
        m_hitboxes[i]->SetLayer(CollisionLayer::Player);
        m_hitboxes[i]->SetActive(false);  // Hitboxes start inactive
    }
    
    m_pushbox = std::make_unique<BoxCollider>();
    m_pushbox->SetType(CollisionType::Pushbox);
    m_pushbox->SetLayer(CollisionLayer::Player);
    
    m_throwbox = std::make_unique<BoxCollider>();
    m_throwbox->SetType(CollisionType::Throwbox);
    m_throwbox->SetLayer(CollisionLayer::Player);
    m_throwbox->SetActive(false);  // Throwbox starts inactive
}

HitboxManager::~HitboxManager() = default;

void HitboxManager::Initialize(const std::string& characterName) {
    m_characterName = characterName;
    InitializeStandardBoxes();
}

void HitboxManager::InitializeStandardBoxes() {
    // Set up default idle stance boxes
    // This would typically load character-specific data
    
    // Default medium character setup
    const auto& idleFrame = StandardBoxes::MEDIUM_IDLE;
    
    // Set up hurtboxes
    for (int i = 0; i < MAX_HURTBOXES; ++i) {
        if (idleFrame.hurtboxes[i].active) {
            m_hurtboxes[i]->SetCenter({idleFrame.hurtboxes[i].x, idleFrame.hurtboxes[i].y});
            m_hurtboxes[i]->SetSize(idleFrame.hurtboxes[i].width, idleFrame.hurtboxes[i].height);
            m_hurtboxes[i]->SetActive(true);
        } else {
            m_hurtboxes[i]->SetActive(false);
        }
    }
    
    // Set up pushbox (standard size for medium character)
    m_pushbox->SetCenter({0, 30});
    m_pushbox->SetSize(30, 80);
    m_pushbox->SetActive(true);
    
    // Throwbox (positioned in front of character)
    m_throwbox->SetCenter({25, 30});
    m_throwbox->SetSize(50, 60);
    m_throwbox->SetActive(false);
}

void HitboxManager::UpdateBoxes(const std::string& animationName, int frame, int facingDirection) {
    m_currentAnimation = animationName;
    m_currentFrame = frame;
    m_facingDirection = facingDirection;
    
    // Look up animation data
    auto it = m_animationData.find(animationName);
    if (it != m_animationData.end() && frame < it->second.size()) {
        ApplyFrameData(it->second[frame]);
    }
}

void HitboxManager::ApplyFrameData(const FrameData& frameData) {
    // Update hurtboxes
    for (int i = 0; i < MAX_HURTBOXES; ++i) {
        const auto& hurtboxData = frameData.hurtboxes[i];
        
        if (hurtboxData.active && !m_invulnerable && !hurtboxData.invulnerable) {
            float x = hurtboxData.x;
            ApplyFacingDirection(x);
            
            m_hurtboxes[i]->SetCenter({x, hurtboxData.y});
            m_hurtboxes[i]->SetSize(hurtboxData.width, hurtboxData.height);
            m_hurtboxes[i]->SetActive(true);
        } else {
            m_hurtboxes[i]->SetActive(false);
        }
    }
    
    // Update hitboxes
    for (int i = 0; i < MAX_HITBOXES; ++i) {
        const auto& hitboxData = frameData.hitboxes[i];
        
        if (hitboxData.active) {
            float x = hitboxData.x;
            ApplyFacingDirection(x);
            
            m_hitboxes[i]->SetCenter({x, hitboxData.y});
            m_hitboxes[i]->SetSize(hitboxData.width, hitboxData.height);
            m_hitboxes[i]->SetActive(true);
            
            // Set combat properties
            m_hitboxes[i]->SetDamage(hitboxData.damage);
            m_hitboxes[i]->SetHitstun(hitboxData.hitstun);
            m_hitboxes[i]->SetBlockstun(hitboxData.blockstun);
            
            // Apply facing direction to knockback
            DirectX::XMFLOAT2 knockback = hitboxData.knockback;
            knockback.x *= m_facingDirection;
            m_hitboxes[i]->SetKnockback(knockback);
            
            m_hitboxes[i]->SetPriority(hitboxData.priority);
        } else {
            m_hitboxes[i]->SetActive(false);
        }
    }
    
    // Update throwbox
    m_throwbox->SetActive(frameData.throwboxActive);
    
    // Update pushbox offset
    float pushboxX = frameData.pushboxOffset.x;
    ApplyFacingDirection(pushboxX);
    m_pushbox->SetCenter({pushboxX, frameData.pushboxOffset.y + 30});  // 30 is base Y
}

void HitboxManager::ApplyFacingDirection(float& x) {
    x *= m_facingDirection;
}

std::vector<BoxCollider*> HitboxManager::GetActiveHitboxes() const {
    std::vector<BoxCollider*> active;
    
    for (const auto& hitbox : m_hitboxes) {
        if (hitbox->IsActive()) {
            active.push_back(hitbox.get());
        }
    }
    
    return active;
}

std::vector<BoxCollider*> HitboxManager::GetHurtboxes() const {
    std::vector<BoxCollider*> active;
    
    for (const auto& hurtbox : m_hurtboxes) {
        if (hurtbox->IsActive()) {
            active.push_back(hurtbox.get());
        }
    }
    
    return active;
}

void HitboxManager::EnableHitbox(int index, bool enable) {
    if (index >= 0 && index < MAX_HITBOXES) {
        m_hitboxes[index]->SetActive(enable);
    }
}

void HitboxManager::EnableThrowbox(bool enable) {
    m_throwbox->SetActive(enable);
}

void HitboxManager::SetInvulnerable(bool invulnerable) {
    m_invulnerable = invulnerable;
    
    // Disable all hurtboxes when invulnerable
    if (invulnerable) {
        for (auto& hurtbox : m_hurtboxes) {
            hurtbox->SetActive(false);
        }
    }
}

void HitboxManager::SetRigidBody(RigidBody* body) {
    m_rigidBody = body;
    
    // Set rigid body for all boxes
    for (auto& hurtbox : m_hurtboxes) {
        hurtbox->SetRigidBody(body);
    }
    
    for (auto& hitbox : m_hitboxes) {
        hitbox->SetRigidBody(body);
    }
    
    m_pushbox->SetRigidBody(body);
    m_throwbox->SetRigidBody(body);
}

void HitboxManager::LoadAnimationData(const std::string& animationName, const std::vector<FrameData>& frames) {
    m_animationData[animationName] = frames;
}

} // namespace ArenaFighter