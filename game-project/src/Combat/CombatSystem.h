#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include "../Physics/PhysicsEngine.h"

namespace ArenaFighter {

enum class AttackType {
    Light,
    Medium,
    Heavy,
    Special,
    Ultimate
};

struct FrameData {
    int startup;
    int active;
    int recovery;
    int hitstun;
    int blockstun;
    float damage;
    float manaCost;
    DirectX::XMFLOAT3 knockback;
};

struct ComboHit {
    AttackType type;
    float damage;
    float scaling; // Damage scaling in combo
    int hitCount;
};

class ComboSystem {
public:
    void RegisterHit(const ComboHit& hit);
    void ResetCombo();
    float GetDamageScaling() const;
    int GetHitCount() const { return m_currentCombo.size(); }
    bool IsInCombo() const { return !m_currentCombo.empty() && m_comboTimer > 0; }
    void Update(float deltaTime);
    
private:
    std::vector<ComboHit> m_currentCombo;
    float m_comboTimer = 0.0f;
    static constexpr float COMBO_TIMEOUT = 1.5f;
    static constexpr int MAX_COMBO_LENGTH = 15;
};

enum class StatusEffect {
    None,
    Burn,      // Damage over time
    Freeze,    // Slow movement
    Shock,     // Interrupt actions
    Poison,    // Reduce healing
    Boost,     // Increase damage
    Shield,    // Damage reduction
    Haste      // Increase speed
};

struct StatusInstance {
    StatusEffect effect;
    float duration;
    float intensity;
    float tickTimer = 0.0f;
};

class CombatSystem {
public:
    CombatSystem() = default;
    ~CombatSystem() = default;

    bool Initialize();
    void Shutdown();
    void Update(float deltaTime);
    void Render(class RenderingSystem* renderer);

    // Attack Registration
    void RegisterAttack(int attackerID, AttackType type, const HitBox& hitbox);
    
    // Frame Data
    void LoadFrameData(const std::string& characterName, AttackType type, const FrameData& data);
    const FrameData* GetFrameData(const std::string& characterName, AttackType type) const;
    
    // Damage Calculation
    float CalculateDamage(float baseDamage, class Character* attacker, class Character* defender);
    
    // Combo System
    ComboSystem* GetComboSystem(int playerID);
    
    // Status Effects
    void ApplyStatusEffect(int targetID, StatusEffect effect, float duration, float intensity);
    void RemoveStatusEffect(int targetID, StatusEffect effect);
    bool HasStatusEffect(int targetID, StatusEffect effect) const;
    
    // Combat State
    bool IsInHitstun(int playerID) const;
    bool IsInBlockstun(int playerID) const;
    bool CanAct(int playerID) const;

private:
    // Frame data database
    std::unordered_map<std::string, FrameData> m_frameDataDB;
    
    // Active hitboxes
    struct ActiveHitbox {
        int attackerID;
        HitBox hitbox;
        int framesRemaining;
    };
    std::vector<ActiveHitbox> m_activeHitboxes;
    
    // Player combat states
    std::unordered_map<int, ComboSystem> m_comboSystems;
    std::unordered_map<int, std::vector<StatusInstance>> m_statusEffects;
    std::unordered_map<int, float> m_hitstunTimers;
    std::unordered_map<int, float> m_blockstunTimers;
    
    void ProcessHitboxCollisions();
    void UpdateStatusEffects(float deltaTime);
};

} // namespace ArenaFighter