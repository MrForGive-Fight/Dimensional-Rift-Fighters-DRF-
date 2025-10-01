#pragma once

#include "../../../game-project/src/Characters/CharacterBase.h"
#include "../../../game-project/src/Combat/CombatEnums.h"
#include <algorithm>

namespace ArenaFighter {

enum class RouEvolutionForm {
    GOBLIN = 0,      // 0-24% gauge
    HOBGOBLIN = 1,   // 25-49% gauge
    OGRE = 2,        // 50-74% gauge
    APOSTLE_LORD = 3,// 75-99% gauge
    VAJRAYAKSA = 4   // 100% gauge
};

enum class Direction {
    UP,
    LEFT,
    RIGHT,
    DOWN
};

struct BuffInfo {
    enum Type {
        DAMAGE_REDUCTION,
        DAMAGE_BOOST,
        SPEED_BOOST,
        DEFENSE_BOOST
    };
    Type type;
    float value;
    float duration;
};

class Rou : public CharacterBase {
public:
    Rou();
    ~Rou();
    
    // Core Systems
    void Update(float deltaTime) override;
    virtual void Render();
    
    // Evolution System
    void UpdateEvolutionGauge(float amount);
    void CheckEvolution();
    void Evolve();
    void ApplyEmergencyProtocol();
    
    // Combat System
    virtual void ExecuteBasicCombo();
    virtual void ExecuteSpecialMove(int skillId);
    
    // S+Direction Moves (15 total)
    void ExecuteDirectionalSpecial(Direction dir);
    
    // Form-Specific Combos
    void GoblinCombo();      // DDD (21-26 damage total)
    void HobgoblinCombo();   // DDDD (35-40 damage total)
    void OgreCombo();        // DDDDD (62-70 damage total)
    void ApostleLordCombo(); // DDDDDD (83-95 damage total)
    void VajrayaksaCombo();  // DDDDDDDD (130-150 damage total)
    
    // Gauge Building Events
    void OnHit(float damage);
    void OnTakeDamage(float damage);
    void OnKill();
    void OnEquipmentPickup();
    void OnDeath();
    
    // Getters
    RouEvolutionForm GetCurrentForm() const { return m_currentForm; }
    float GetEvolutionGauge() const { return m_evolutionGauge; }
    float GetHPPercent() const { return (m_currentHP / m_maxHP) * 100.0f; }
    float GetMaxHP() const { return m_maxHP; }
    
    // Combat helpers
    void Heal(float amount) { m_currentHP = std::min(m_currentHP + amount, m_maxHP); }
    void AddBuff(BuffInfo::Type type, float value, float duration);
    float GetDamageReduction() const;
    
    // Get current form stats
    FormStats GetCurrentFormStats() const { return FORM_STATS[static_cast<int>(m_currentForm)]; }
    
private:
    // Evolution State
    RouEvolutionForm m_currentForm;
    float m_evolutionGauge;
    bool m_emergencyProtocolUsed;
    float m_emergencyProtocolTimer;
    
    // Health system
    float m_currentHP;
    float m_maxHP;
    
    // Buffs
    std::vector<BuffInfo> m_activeBuffs;
    
    // Form Stats
    struct FormStats {
        float damageMultiplier;
        float speedMultiplier;
        float defenseMultiplier;
        float sizeScale;
        int maxComboHits;
        float baseHP;
        int startupFrames;
        int activeFrames;
        int recoveryFrames;
    };
    
    static constexpr FormStats FORM_STATS[5] = {
        // Goblin: High speed, low damage, small
        {0.85f, 1.3f, 1.15f, 0.8f, 3, 180.0f, 3, 2, 5},
        // Hobgoblin: Balanced
        {1.0f, 1.1f, 1.0f, 1.0f, 4, 200.0f, 5, 3, 8},
        // Ogre: High damage, low defense, large
        {1.25f, 0.9f, 0.85f, 2.5f, 5, 220.0f, 6, 4, 10},
        // Apostle Lord: High damage, medium defense
        {1.4f, 1.0f, 0.7f, 2.0f, 6, 210.0f, 8, 5, 12},
        // Vajrayaksa: Highest damage, lowest defense
        {1.6f, 1.1f, 0.5f, 2.5f, 8, 200.0f, 10, 8, 15}
    };
    
    // Special Move Management
    struct SpecialMove {
        std::string name;
        float cooldown;
        float currentCooldown;
        std::function<void()> execute;
    };
    
    std::vector<SpecialMove> m_specialMoves;
    void InitializeSpecialMoves();
    
    // Goblin Specials
    void PanicJump();        // S+Up - Quick vertical escape
    void SurvivalBite();     // S+Left - Counter stance with heal
    void GoblinRush();       // S+Right - Fast roll through enemies
    
    // Hobgoblin Specials
    void ShadowUpper();      // S+Up - Rising uppercut
    void DarkCounter();      // S+Left - Counter with explosion
    void PhantomStrike();    // S+Right - Teleport backstab
    
    // Ogre Specials
    void OgreSlam();         // S+Up - Jump slam with shockwave
    void GroundQuake();      // S+Left - Earth spike attack
    void BrutalCharge();     // S+Right - Armored rush grab
    
    // Apostle Lord Specials
    void DemonAscension();   // S+Up - Fly and rain orbs
    void LordsTerritory();   // S+Left - Buff field creation
    void OrbBarrage();       // S+Right - Homing orb attack
    
    // Vajrayaksa Specials
    void HeavenSplitter();   // S+Up - 4-arm energy pillar
    void OverlordsDecree();  // S+Left - AOE fear and reset
    void ThousandArmsRush(); // S+Right - Teleport barrage
    
    // Helper functions
    void UpdateBuffs(float deltaTime);
    void UpdateCooldowns(float deltaTime);
    FormStats GetCurrentFormStats() const;
    void PlayEvolutionVFX();
    void ApplyFormChanges();
};

// Constants
constexpr float BASE_HEALTH = 1000.0f;
constexpr float BASE_MANA = 100.0f;
constexpr float MANA_REGEN = 5.0f;
constexpr float COMBO_SCALING = 0.9f;
constexpr int MAX_COMBO_HITS = 15;
constexpr float EMERGENCY_PROTOCOL_HP_THRESHOLD = 0.3f;
constexpr float EMERGENCY_PROTOCOL_HEAL = 0.15f;
constexpr float EMERGENCY_PROTOCOL_DR_DURATION = 3.0f;
constexpr float EVOLUTION_GAUGE_ON_HIT = 1.5f;
constexpr float EVOLUTION_GAUGE_PER_10HP_LOST = 0.5f;
constexpr float EVOLUTION_GAUGE_ON_KILL = 15.0f;
constexpr float EVOLUTION_GAUGE_ON_PICKUP = 7.0f;
constexpr float EVOLUTION_GAUGE_DEATH_PENALTY = -0.35f;

} // namespace ArenaFighter