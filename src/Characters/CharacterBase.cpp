#include "CharacterBase.h"
#include "CharacterCategory.h"
#include "../Combat/CombatSystem.h"
#include <random>
#include <algorithm>

#include <iostream>
#include "../Animation/CharacterAnimator.h"
namespace ArenaFighter {

// Static member initialization
int CharacterBase::s_nextId = 1;

CharacterBase::CharacterBase(const std::string& name, CharacterCategory category, StatMode statMode)
    : m_id(s_nextId++)
    , m_name(name)
    , m_category(category)
    , m_statMode(statMode) {
    
    // Apply category and stat mode modifiers
    ApplyStatModifiers();
    
    // Initialize gear skills with empty placeholders
    for (int i = 0; i < 8; ++i) {
        m_gearSkills[i] = GearSkill();
        m_gearSkills[i].name = "Skill " + std::to_string(i + 1);
        m_gearSkills[i].manaCost = 10.0f + (i * 5.0f); // Progressive mana costs
        m_gearSkills[i].cooldown = 2.0f + (i * 0.5f);  // Progressive cooldowns
        m_gearSkillCooldowns[i] = 0.0f;  // No cooldown initially
    }
}

CharacterBase::~CharacterBase() = default;

bool CharacterBase::IsInCounterState() const {
    // Counter state is when player is in startup frames of an attack
    // This is a simplified check - real implementation would check frame data
    return m_currentState == CharacterState::Normal && m_stateTimer < 0.2f;
}

bool CharacterBase::RollCritical() const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 1.0);
    
    return dis(gen) < m_criticalChance;
}

bool CharacterBase::CanAffordSkill(float manaCost) const {
    return m_currentMana >= manaCost;
}

void CharacterBase::ConsumeMana(float amount) {
    m_currentMana = std::max(0.0f, m_currentMana - amount);
}

void CharacterBase::RegenerateMana(float deltaTime) {
    // CLAUDE.md: 5 mana per second
    m_manaRegenTimer += deltaTime;
    
    // Get mana regen modifier from category
    const auto& traits = CharacterCategoryManager::GetInstance().GetCategoryTraits(m_category);
    float regenModifier = traits.manaRegenModifier;
    
    // Special mode gets additional mana regen
    if (m_statMode == StatMode::Special) {
        regenModifier *= 1.1f; // +10% mana regen for special mode
    }
    
    // Regenerate mana every 0.1 seconds for smoother regen
    const float regenInterval = 0.1f;
    while (m_manaRegenTimer >= regenInterval) {
        float regenAmount = CombatSystem::MANA_REGEN * regenInterval * regenModifier;
        m_currentMana = std::min(m_maxMana, m_currentMana + regenAmount);
        m_manaRegenTimer -= regenInterval;
    }
}

void CharacterBase::TakeDamage(float damage) {
    m_currentHealth = std::max(0.0f, m_currentHealth - damage);
}

void CharacterBase::Heal(float amount) {
    m_currentHealth = std::min(m_maxHealth, m_currentHealth + amount);
}

void CharacterBase::SetGearSkill(int index, const GearSkill& skill) {
    if (index >= 0 && index < 8) {
        m_gearSkills[index] = skill;
    }
}

void CharacterBase::SwitchGear(int gearIndex) {
    if (gearIndex >= 0 && gearIndex <= 3 && gearIndex != m_currentGear) {
        int oldGear = m_currentGear;
        m_currentGear = gearIndex;
        OnGearSwitch(oldGear, m_currentGear);
    }
}

void CharacterBase::Initialize() {
    // Reset to full health and mana
    m_currentHealth = m_maxHealth;
    m_currentMana = m_maxMana;
    m_currentState = CharacterState::Normal;
    m_stateTimer = 0.0f;
    m_manaRegenTimer = 0.0f;
}

void CharacterBase::Update(float deltaTime) {
    // Update state timer
    UpdateState(deltaTime);
    
    // Update cooldowns
    UpdateCooldowns(deltaTime);
    
    // Regenerate mana (not during special move execution)
    if (m_currentState != CharacterState::ExecutingSpecial) {
        RegenerateMana(deltaTime);
    }
    
    // Update block duration
    if (m_currentState == CharacterState::Blocking) {
        m_blockDuration += deltaTime;
    }
    
    // Update character-specific systems if present
    if (m_stanceSystem) {
        // m_stanceSystem->Update(deltaTime);
    }
    if (m_evolutionSystem) {
        // m_evolutionSystem->Update(deltaTime);
    }
    if (m_transformationSystem) {
        // m_transformationSystem->Update(deltaTime);
    }
}

void CharacterBase::UpdateState(float deltaTime) {
    // Update animation system
    if (m_animator) {
        m_animator->Update(deltaTime);
    }
    m_stateTimer += deltaTime;
    
    // Auto-recover from certain states after time
    switch (m_currentState) {
        case CharacterState::KnockedDown:
            if (m_stateTimer > 1.0f) { // 1 second knockdown
                m_currentState = CharacterState::GettingUp;
                m_stateTimer = 0.0f;
            }
            break;
            
        case CharacterState::GettingUp:
            if (m_stateTimer > 0.5f) { // 0.5 second wakeup
                m_currentState = CharacterState::Normal;
                m_stateTimer = 0.0f;
            }
            break;
            
        case CharacterState::ExecutingSpecial:
            // Special move completion is handled by combat system
            break;
            
        default:
            // Other states are managed by combat system
            break;
    }
}

void CharacterBase::ApplyStatModifiers() {
    // Get category manager
    auto& categoryMgr = CharacterCategoryManager::GetInstance();
    
    // Apply category modifiers
    categoryMgr.ApplyCategoryModifiers(m_category, 
        m_maxHealth, m_maxMana, m_defense, m_speed, m_powerModifier);
    
    // Apply stat mode modifiers
    categoryMgr.ApplyStatModeModifiers(m_statMode,
        m_maxHealth, m_maxMana, m_defense, m_speed, m_powerModifier);
    
    // Apply category-specific bonuses
    const auto& traits = categoryMgr.GetCategoryTraits(m_category);
    m_criticalChance += traits.criticalChanceBonus;
    
    // Set current values to max
    m_currentHealth = m_maxHealth;
    m_currentMana = m_maxMana;
}

bool CharacterBase::HasStanceSystem() const {
    const auto& traits = CharacterCategoryManager::GetInstance().GetCategoryTraits(m_category);
    return traits.hasStanceSystem;
}

std::string CharacterBase::GetVisualTheme() const {
    const auto& traits = CharacterCategoryManager::GetInstance().GetCategoryTraits(m_category);
    return traits.themeDescription;
}

std::string CharacterBase::GetAuraType() const {
    const auto& traits = CharacterCategoryManager::GetInstance().GetCategoryTraits(m_category);
    return traits.auraType;
}

void CharacterBase::StartBlocking() {
    if (m_currentState != CharacterState::Blocking && 
        m_currentState != CharacterState::HitStun &&
        m_currentState != CharacterState::KnockedDown &&
        m_currentState != CharacterState::ExecutingSpecial) {
        SetState(CharacterState::Blocking);
        m_blockDuration = 0.0f;
        m_stateTimer = 0.0f;
        OnBlockStart();
    }
}

void CharacterBase::StopBlocking() {
    if (m_currentState == CharacterState::Blocking) {
        SetState(CharacterState::Normal);
        m_stateTimer = 0.0f;
        OnBlockEnd();
    }
}

bool CharacterBase::IsGearSkillOnCooldown(int skillIndex) const {
    if (skillIndex >= 0 && skillIndex < 8) {
        return m_gearSkillCooldowns[skillIndex] > 0.0f;
    }
    return false;
}

float CharacterBase::GetGearSkillCooldownRemaining(int skillIndex) const {
    if (skillIndex >= 0 && skillIndex < 8) {
        return m_gearSkillCooldowns[skillIndex];
    }
    return 0.0f;
}

void CharacterBase::StartGearSkillCooldown(int skillIndex) {
    if (skillIndex >= 0 && skillIndex < 8) {
        m_gearSkillCooldowns[skillIndex] = m_gearSkills[skillIndex].cooldown;
    }
}

void CharacterBase::UpdateCooldowns(float deltaTime) {
    for (int i = 0; i < 8; ++i) {
        if (m_gearSkillCooldowns[i] > 0.0f) {
            m_gearSkillCooldowns[i] = std::max(0.0f, m_gearSkillCooldowns[i] - deltaTime);
        }
    }
}

void CharacterBase::RegisterSpecialMove(InputDirection direction, const SpecialMove& move) {
    m_specialMoves[direction] = move;
}

const SpecialMove* CharacterBase::GetSpecialMove(InputDirection direction) const {
    auto it = m_specialMoves.find(direction);
    if (it != m_specialMoves.end()) {
        return &it->second;
    }
    return nullptr;
}

bool CharacterBase::HasSpecialMove(InputDirection direction) const {
    return m_specialMoves.find(direction) != m_specialMoves.end();
}

bool CharacterBase::CanExecuteSpecialMove(InputDirection direction) const {
    // Cannot execute special moves while blocking
    if (IsBlocking()) {
        return false;
    }
    
    // Check if we have the special move
    const SpecialMove* move = GetSpecialMove(direction);
    if (!move) {
        return false;
    }
    
    // Check mana cost (no cooldown for special moves)
    if (!CanAffordSkill(move->manaCost)) {
        return false;
    }
    
    // Check state restrictions
    if (m_currentState != CharacterState::Normal && 
        m_currentState != CharacterState::Crouching &&
        m_currentState != CharacterState::Airborne) {
        return false;
    }
    
    // Check stance requirement if applicable
    if (HasStanceSystem() && move->requiredStance >= 0) {
        if (GetCurrentStance() != move->requiredStance) {
            return false;
        }
    }
    
    return true;
}

void CharacterBase::ExecuteSpecialMove(InputDirection direction) {
    if (!CanExecuteSpecialMove(direction)) {
        return;
    }
    
    const SpecialMove* move = GetSpecialMove(direction);
    if (move) {
        // Consume mana
        ConsumeMana(move->manaCost);
        
        // Set state
        SetState(CharacterState::ExecutingSpecial);
        m_stateTimer = 0.0f;
        m_lastSpecialDirection = direction;
        
        // Notify character-specific logic
        OnSpecialMoveExecute(direction);
    }
}

bool CharacterBase::CanExecuteSpecialMoveInStance(InputDirection direction, int currentStance) const {
    const SpecialMove* move = GetSpecialMove(direction);
    if (!move) {
        return false;
    }
    
    // Check stance requirement
    if (move->requiredStance >= 0 && move->requiredStance != currentStance) {
        return false;
    }
    
    // Check other requirements
    return CanExecuteSpecialMove(direction);
}

} // namespace ArenaFighter

// Animation system implementation
bool CharacterBase::InitializeAnimator(const std::string& skeletonPath) {
    if (!m_animator) {
        m_animator = std::make_unique<CharacterAnimator>();
    }
    
    return m_animator->Initialize(skeletonPath);
}

bool CharacterBase::LoadAnimation(const std::string& name, const std::string& filepath) {
    if (!m_animator) {
        std::cerr << "CharacterBase::LoadAnimation: Animator not initialized!" << std::endl;
        return false;
    }
    
    return m_animator->LoadAnimation(name, filepath);
}

void CharacterBase::PlayAnimation(const std::string& stateName, bool forceRestart) {
    if (!m_animator) {
        std::cerr << "CharacterBase::PlayAnimation: Animator not initialized!" << std::endl;
