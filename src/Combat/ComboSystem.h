#pragma once

#include <vector>
#include <chrono>
#include "CombatEnums.h"

namespace ArenaFighter {

/**
 * @brief Individual hit information in a combo
 */
struct ComboHit {
    AttackType attackType;
    float damage;
    int targetId;
    std::chrono::steady_clock::time_point timestamp;
    int hitNumber;
};

/**
 * @brief Manages combo tracking, scaling, and validation following LSFDC patterns
 * 
 * Features:
 * - Maximum 15 hit combos
 * - 0.9x damage scaling per hit
 * - Hitstun decay of 0.95x per hit
 * - Maximum 60% health damage limit
 */
class ComboSystem {
public:
    ComboSystem();
    ~ComboSystem();
    
    // Combo management
    void RegisterHit(AttackType type, float damage, int targetId);
    void Reset();
    void Update(float deltaTime);
    
    // Combo queries
    int GetHitCount() const { return static_cast<int>(m_comboHits.size()); }
    float GetTotalDamage() const { return m_totalDamage; }
    float GetCurrentScaling() const;
    float GetHitstunScaling() const;
    bool IsActive() const { return !m_comboHits.empty() && m_comboTimer > 0.0f; }
    bool CanExtendCombo() const;
    
    // Combo analysis
    float GetDamagePerSecond() const;
    float GetAverageHitDamage() const;
    AttackType GetLastHitType() const;
    bool IsRepetitive() const;  // Check if using same move too much
    
    // Constants from CLAUDE.md
    static constexpr float COMBO_TIMEOUT = 1.5f;  // 1.5 seconds to continue combo
    static constexpr int MAX_COMBO_LENGTH = 15;
    static constexpr float DAMAGE_SCALING = 0.9f;
    static constexpr float HITSTUN_DECAY = 0.95f;
    static constexpr float MAX_DAMAGE_PERCENT = 0.6f;  // 60% max health
    
private:
    std::vector<ComboHit> m_comboHits;
    float m_totalDamage;
    float m_comboTimer;
    std::chrono::steady_clock::time_point m_comboStartTime;
    std::chrono::steady_clock::time_point m_lastHitTime;
    
    // Repetition tracking
    struct MoveUsage {
        AttackType type;
        int count;
    };
    std::vector<MoveUsage> m_moveUsage;
    
    void UpdateMoveUsage(AttackType type);
    float CalculateRepetitionPenalty() const;
};

/**
 * @brief Manages proration (damage scaling) for extended combos
 */
class ProrationSystem {
public:
    ProrationSystem();
    
    // Calculate final damage after all scaling factors
    float CalculateProratedDamage(float baseDamage, int comboHit, 
                                 bool isStarter, bool isRepetitive);
    
    // Individual scaling factors
    float GetComboScaling(int hitCount) const;
    float GetStarterScaling(bool isStarter, int hitCount) const;
    float GetRepetitionScaling(int sameMovesCount) const;
    
    // LSFDC scaling constants
    static constexpr float STARTER_SCALING = 1.0f;      // First hit always 100%
    static constexpr float STARTER_BONUS = 1.1f;        // Starter moves get bonus
    static constexpr float REPETITION_PENALTY = 0.8f;   // Using same move repeatedly
    static constexpr float MINIMUM_SCALING = 0.1f;      // Minimum 10% damage
    
private:
    float ClampScaling(float scaling) const;
};

/**
 * @brief Tracks combo routes and provides optimization hints
 */
class ComboRoute {
public:
    struct RouteNode {
        AttackType attack;
        float damage;
        int frames;
        float manaUsed;
    };
    
    ComboRoute();
    
    void AddNode(AttackType attack, float damage, int frames, float manaUsed);
    void Clear();
    
    // Route analysis
    float GetTotalDamage() const;
    int GetTotalFrames() const;
    float GetTotalManaUsed() const;
    float GetDamageEfficiency() const;  // Damage per mana
    float GetTimeEfficiency() const;    // Damage per frame
    
    // Route optimization
    bool IsOptimalRoute() const;
    std::vector<AttackType> GetSuggestedContinuation() const;
    
private:
    std::vector<RouteNode> m_route;
    float m_totalDamage;
    int m_totalFrames;
    float m_totalMana;
};

} // namespace ArenaFighter