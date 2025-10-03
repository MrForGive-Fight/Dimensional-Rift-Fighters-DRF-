#pragma once

#include <string>
#include <array>
#include <memory>
#include <unordered_map>
#include "../Combat/CombatEnums.h"
#include "CharacterCategory.h"

namespace ArenaFighter {

// Forward declarations
class CombatSystem;
class CharacterAnimator;
struct FrameData;
enum class ElementType {
    Neutral,
    Fire,
    Ice,
    Lightning,
    Earth,
    Wind,
    Light,
    Dark,
    Void
};

enum class CharacterState {
    Normal,
    Defending,
    Blocking,     // New state for blocking
    HitStun,
    KnockedDown,
    GettingUp,
    Airborne,
    Crouching,
    ExecutingSpecial  // During special move execution
};

enum class InputDirection {
    Up,
    Down,
    Left,
    Right
};

enum class CharacterCategory {
    System,
    GodsHeroes,
    Murim,
    Cultivation,
    Animal,
    Monsters,
    Chaos
};

/**
 * @brief Gear skill definition - each character has 4 gears with 2 skills each
 * These skills have COOLDOWNS as per CLAUDE.md (AS, AD, ASD, SD inputs)
 */
struct GearSkill {
    std::string name;
    std::string animation;
    float manaCost;
    float baseDamage;
    float cooldown;        // Cooldown in seconds
    AttackType attackType;

    // Frame data will be registered with CombatSystem
    int startupFrames;
    int activeFrames;
    int recoveryFrames;

    // Special properties
    bool isProjectile = false;
    bool canCombo = true;
    ElementType element = ElementType::Neutral;
};

/**
 * @brief Special move definition - S+Direction inputs (MANA ONLY, NO COOLDOWN)
 */
struct SpecialMove {
    std::string name;
    std::string animation;
    float manaCost;        // MANA cost only, no cooldown
    float baseDamage;
    AttackType attackType;

    // Frame data
    int startupFrames;
    int activeFrames;
    int recoveryFrames;

    // Special properties
    bool isProjectile = false;
    bool canCombo = true;
    bool blockable = true;
    ElementType element = ElementType::Neutral;

    // Optional stance requirement (for stance characters)
    int requiredStance = -1;  // -1 means any stance
};

/**
 * @brief Base class for all characters in DFR
 *
 * Implements core stats and mana-based skill system (no cooldowns)
 * Following CLAUDE.md specifications
 */
class CharacterBase {
public:
    CharacterBase(const std::string& name, CharacterCategory category, StatMode statMode = StatMode::Hybrid);
    virtual ~CharacterBase();

    // Identity
    int GetId() const { return m_id; }
    const std::string& GetName() const { return m_name; }
    CharacterCategory GetCategory() const { return m_category; }
    StatMode GetStatMode() const { return m_statMode; }

    // Stats - CLAUDE.md base values
    float GetMaxHealth() const { return m_maxHealth; }
    float GetCurrentHealth() const { return m_currentHealth; }
    float GetMaxMana() const { return m_maxMana; }
    float GetCurrentMana() const { return m_currentMana; }
    float GetDefense() const { return m_defense; }
    float GetSpeed() const { return m_speed; }
    float GetWeight() const { return m_weight; }

    // Combat properties
    float GetPowerModifier() const { return m_powerModifier; }
    ElementType GetElement() const { return m_element; }
    CharacterState GetCurrentState() const { return m_currentState; }
    bool IsInCounterState() const;
    bool RollCritical() const;

    // Mana management
    bool CanAffordSkill(float manaCost) const;
    void ConsumeMana(float amount);
    void RegenerateMana(float deltaTime);

    // Health management
    void TakeDamage(float damage);
    void Heal(float amount);
    bool IsAlive() const { return m_currentHealth > 0; }

    // State management
    void SetState(CharacterState state) { m_currentState = state; }
    void UpdateState(float deltaTime);

    // Blocking state
    bool IsBlocking() const { return m_currentState == CharacterState::Blocking; }
    void StartBlocking();
    void StopBlocking();

    // Gear system - 4 gears x 2 skills = 8 total skills (WITH COOLDOWNS)
    const std::array<GearSkill, 8>& GetGearSkills() const { return m_gearSkills; }
    void SetGearSkill(int index, const GearSkill& skill);
    int GetCurrentGear() const { return m_currentGear; }
    void SwitchGear(int gearIndex); // 0-3, instant switch, no mana cost

    // Get skills for current gear (2 skills per gear)
    const GearSkill& GetGearSkill1() const { return m_gearSkills[m_currentGear * 2]; }
    const GearSkill& GetGearSkill2() const { return m_gearSkills[m_currentGear * 2 + 1]; }

    // Check if gear skill is on cooldown
    bool IsGearSkillOnCooldown(int skillIndex) const;
    float GetGearSkillCooldownRemaining(int skillIndex) const;
    void StartGearSkillCooldown(int skillIndex);

    // Special move system - S+Direction inputs (MANA ONLY, NO COOLDOWN)
    void RegisterSpecialMove(InputDirection direction, const SpecialMove& move);
    const SpecialMove* GetSpecialMove(InputDirection direction) const;
    bool HasSpecialMove(InputDirection direction) const;
    const std::unordered_map<InputDirection, SpecialMove>& GetAllSpecialMoves() const { return m_specialMoves; }

    // Execute special move
    bool CanExecuteSpecialMove(InputDirection direction) const;
    void ExecuteSpecialMove(InputDirection direction);

    // Helper for stance-based special moves
    bool CanExecuteSpecialMoveInStance(InputDirection direction, int currentStance) const;

    // Animation system
    CharacterAnimator* GetAnimator() { return m_animator.get(); }
    const CharacterAnimator* GetAnimator() const { return m_animator.get(); }
    bool InitializeAnimator(const std::string& skeletonPath);
    bool LoadAnimation(const std::string& name, const std::string& filepath);
    void PlayAnimation(const std::string& stateName, bool forceRestart = false);

    // Virtual methods for character-specific behavior
    virtual void Initialize();
    virtual void Update(float deltaTime);
    virtual void OnGearSwitch(int oldGear, int newGear) {}
    virtual void OnSkillUse(int skillIndex) {}
    virtual void OnSpecialMoveExecute(InputDirection direction) {}
    virtual void OnBlockStart() {}
    virtual void OnBlockEnd() {}

    // Stance system support (for applicable characters)
    virtual bool HasStanceSystem() const;
    virtual void SwitchStance(int stanceIndex) {}
    virtual int GetCurrentStance() const { return 0; }

    // Category-specific visual info
    std::string GetVisualTheme() const;
    std::string GetAuraType() const;

protected:
    // Core properties
    int m_id;
    std::string m_name;
    CharacterCategory m_category;
    StatMode m_statMode;

    // Stats (from CLAUDE.md)
    float m_maxHealth = 1000.0f;
    float m_currentHealth = 1000.0f;
    float m_maxMana = 100.0f;
    float m_currentMana = 100.0f;
    float m_defense = 100.0f;
    float m_speed = 100.0f;
    float m_weight = 100.0f;

    // Combat properties
    float m_powerModifier = 1.0f;
    float m_criticalChance = 0.05f; // 5% base crit
    ElementType m_element = ElementType::Neutral;
    CharacterState m_currentState = CharacterState::Normal;

    // Gear system (with cooldowns)
    std::array<GearSkill, 8> m_gearSkills;
    std::array<float, 8> m_gearSkillCooldowns;  // Current cooldown timers
    int m_currentGear = 0; // 0-3

    // Special move system (mana only)
    std::unordered_map<InputDirection, SpecialMove> m_specialMoves;
    InputDirection m_lastSpecialDirection = InputDirection::Up;

    // Character-specific systems (optional)
    std::unique_ptr<class StanceSystem> m_stanceSystem;
    std::unique_ptr<class EvolutionSystem> m_evolutionSystem;
    std::unique_ptr<class TransformationSystem> m_transformationSystem;

    // Animation system
    std::unique_ptr<CharacterAnimator> m_animator;

    // Apply category and stat mode modifiers
    void ApplyStatModifiers();

private:
    static int s_nextId;

    // Internal state tracking
    float m_stateTimer = 0.0f;
    float m_manaRegenTimer = 0.0f;
    float m_blockDuration = 0.0f;  // Time spent blocking

    // Update cooldowns
    void UpdateCooldowns(float deltaTime);
};

} // namespace ArenaFighter

// Hash function for InputDirection to use with unordered_map
namespace std {
    template<>
    struct hash<ArenaFighter::InputDirection> {
        size_t operator()(const ArenaFighter::InputDirection& dir) const {
            return hash<int>()(static_cast<int>(dir));
        }
    };
}
