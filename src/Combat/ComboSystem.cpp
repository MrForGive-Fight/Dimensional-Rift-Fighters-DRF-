#include "ComboSystem.h"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace ArenaFighter {

// ComboSystem implementation
ComboSystem::ComboSystem() 
    : m_totalDamage(0.0f)
    , m_comboTimer(0.0f) {
}

ComboSystem::~ComboSystem() = default;

void ComboSystem::RegisterHit(AttackType type, float damage, int targetId) {
    // Check if combo can be extended
    if (GetHitCount() >= MAX_COMBO_LENGTH) {
        return;  // Max combo length reached
    }
    
    // Create new hit
    ComboHit hit;
    hit.attackType = type;
    hit.damage = damage;
    hit.targetId = targetId;
    hit.timestamp = std::chrono::steady_clock::now();
    hit.hitNumber = GetHitCount() + 1;
    
    // If this is the first hit, record combo start time
    if (m_comboHits.empty()) {
        m_comboStartTime = hit.timestamp;
    }
    
    // Add hit and update totals
    m_comboHits.push_back(hit);
    m_totalDamage += damage;
    m_lastHitTime = hit.timestamp;
    
    // Reset combo timer
    m_comboTimer = COMBO_TIMEOUT;
    
    // Update move usage tracking
    UpdateMoveUsage(type);
}

void ComboSystem::Reset() {
    m_comboHits.clear();
    m_totalDamage = 0.0f;
    m_comboTimer = 0.0f;
    m_moveUsage.clear();
}

void ComboSystem::Update(float deltaTime) {
    if (!IsActive()) {
        return;
    }
    
    // Update combo timer
    m_comboTimer -= deltaTime;
    
    // Reset if timeout
    if (m_comboTimer <= 0.0f) {
        Reset();
    }
}

float ComboSystem::GetCurrentScaling() const {
    if (m_comboHits.empty()) {
        return 1.0f;
    }
    
    // Base scaling: 0.9^n
    float scaling = std::pow(DAMAGE_SCALING, static_cast<float>(GetHitCount() - 1));
    
    // Apply repetition penalty if using same moves
    if (IsRepetitive()) {
        scaling *= CalculateRepetitionPenalty();
    }
    
    // Minimum scaling
    return std::max(scaling, 0.1f);
}

float ComboSystem::GetHitstunScaling() const {
    if (m_comboHits.empty()) {
        return 1.0f;
    }
    
    // Hitstun decay: 0.95^n
    return std::pow(HITSTUN_DECAY, static_cast<float>(GetHitCount() - 1));
}

bool ComboSystem::CanExtendCombo() const {
    // Check hit count limit
    if (GetHitCount() >= MAX_COMBO_LENGTH) {
        return false;
    }
    
    // Check if combo is still active
    if (!IsActive()) {
        return false;
    }
    
    return true;
}

float ComboSystem::GetDamagePerSecond() const {
    if (m_comboHits.empty()) {
        return 0.0f;
    }
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        m_lastHitTime - m_comboStartTime).count() / 1000.0f;
    
    if (duration <= 0.0f) {
        return m_totalDamage;
    }
    
    return m_totalDamage / duration;
}

float ComboSystem::GetAverageHitDamage() const {
    if (m_comboHits.empty()) {
        return 0.0f;
    }
    
    return m_totalDamage / static_cast<float>(GetHitCount());
}

AttackType ComboSystem::GetLastHitType() const {
    if (m_comboHits.empty()) {
        return AttackType::Light;
    }
    
    return m_comboHits.back().attackType;
}

bool ComboSystem::IsRepetitive() const {
    // Check if using same attack type too much
    for (const auto& usage : m_moveUsage) {
        if (usage.count >= 3) {  // Using same move 3+ times
            return true;
        }
    }
    return false;
}

void ComboSystem::UpdateMoveUsage(AttackType type) {
    // Find existing usage
    auto it = std::find_if(m_moveUsage.begin(), m_moveUsage.end(),
        [type](const MoveUsage& usage) { return usage.type == type; });
    
    if (it != m_moveUsage.end()) {
        it->count++;
    } else {
        m_moveUsage.push_back({type, 1});
    }
}

float ComboSystem::CalculateRepetitionPenalty() const {
    float penalty = 1.0f;
    
    for (const auto& usage : m_moveUsage) {
        if (usage.count >= 3) {
            // Each repetition beyond 2 reduces damage by 20%
            int excess = usage.count - 2;
            penalty *= std::pow(0.8f, static_cast<float>(excess));
        }
    }
    
    return penalty;
}

// ProrationSystem implementation
ProrationSystem::ProrationSystem() {
}

float ProrationSystem::CalculateProratedDamage(float baseDamage, int comboHit,
                                              bool isStarter, bool isRepetitive) {
    float scaling = 1.0f;
    
    // Apply combo scaling
    scaling *= GetComboScaling(comboHit);
    
    // Apply starter scaling
    scaling *= GetStarterScaling(isStarter, comboHit);
    
    // Apply repetition penalty
    if (isRepetitive) {
        scaling *= REPETITION_PENALTY;
    }
    
    // Clamp to minimum
    scaling = ClampScaling(scaling);
    
    return baseDamage * scaling;
}

float ProrationSystem::GetComboScaling(int hitCount) const {
    if (hitCount <= 1) {
        return STARTER_SCALING;
    }
    
    // LSFDC formula: 0.9^(n-1)
    return std::pow(ComboSystem::DAMAGE_SCALING, static_cast<float>(hitCount - 1));
}

float ProrationSystem::GetStarterScaling(bool isStarter, int hitCount) const {
    if (hitCount == 1) {
        return STARTER_SCALING;  // First hit always 100%
    }
    
    if (isStarter && hitCount <= 3) {
        return STARTER_BONUS;  // Starter moves get bonus on early hits
    }
    
    return 1.0f;
}

float ProrationSystem::GetRepetitionScaling(int sameMovesCount) const {
    if (sameMovesCount <= 2) {
        return 1.0f;
    }
    
    // Each repetition beyond 2 applies penalty
    int excess = sameMovesCount - 2;
    return std::pow(REPETITION_PENALTY, static_cast<float>(excess));
}

float ProrationSystem::ClampScaling(float scaling) const {
    return std::max(scaling, MINIMUM_SCALING);
}

// ComboRoute implementation
ComboRoute::ComboRoute() 
    : m_totalDamage(0.0f)
    , m_totalFrames(0)
    , m_totalMana(0.0f) {
}

void ComboRoute::AddNode(AttackType attack, float damage, int frames, float manaUsed) {
    RouteNode node;
    node.attack = attack;
    node.damage = damage;
    node.frames = frames;
    node.manaUsed = manaUsed;
    
    m_route.push_back(node);
    m_totalDamage += damage;
    m_totalFrames += frames;
    m_totalMana += manaUsed;
}

void ComboRoute::Clear() {
    m_route.clear();
    m_totalDamage = 0.0f;
    m_totalFrames = 0;
    m_totalMana = 0.0f;
}

float ComboRoute::GetTotalDamage() const {
    return m_totalDamage;
}

int ComboRoute::GetTotalFrames() const {
    return m_totalFrames;
}

float ComboRoute::GetTotalManaUsed() const {
    return m_totalMana;
}

float ComboRoute::GetDamageEfficiency() const {
    if (m_totalMana <= 0.0f) {
        return 0.0f;
    }
    return m_totalDamage / m_totalMana;
}

float ComboRoute::GetTimeEfficiency() const {
    if (m_totalFrames <= 0) {
        return 0.0f;
    }
    return m_totalDamage / static_cast<float>(m_totalFrames);
}

bool ComboRoute::IsOptimalRoute() const {
    // Check if route is efficient
    float damageEff = GetDamageEfficiency();
    float timeEff = GetTimeEfficiency();
    
    // Thresholds for optimal route
    const float MIN_DAMAGE_EFFICIENCY = 10.0f;  // 10 damage per mana
    const float MIN_TIME_EFFICIENCY = 2.0f;     // 2 damage per frame
    
    return damageEff >= MIN_DAMAGE_EFFICIENCY && timeEff >= MIN_TIME_EFFICIENCY;
}

std::vector<AttackType> ComboRoute::GetSuggestedContinuation() const {
    std::vector<AttackType> suggestions;
    
    if (m_route.empty()) {
        // Start with light attack for easy confirms
        suggestions.push_back(AttackType::Light);
        return suggestions;
    }
    
    // Get last attack type
    AttackType lastAttack = m_route.back().attack;
    
    // Suggest natural progressions
    switch (lastAttack) {
        case AttackType::Light:
            suggestions.push_back(AttackType::Medium);
            suggestions.push_back(AttackType::Light);  // Light chain
            break;
        case AttackType::Medium:
            suggestions.push_back(AttackType::Heavy);
            suggestions.push_back(AttackType::Special);
            break;
        case AttackType::Heavy:
            suggestions.push_back(AttackType::Special);
            suggestions.push_back(AttackType::Ultimate);
            break;
        case AttackType::Special:
            suggestions.push_back(AttackType::Light);  // Reset pressure
            suggestions.push_back(AttackType::Ultimate);
            break;
        case AttackType::Ultimate:
            // Usually ends combo
            break;
    }
    
    return suggestions;
}

} // namespace ArenaFighter