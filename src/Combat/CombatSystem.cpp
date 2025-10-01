#include "CombatSystem.h"
#include "DamageCalculator.h"
#include "HitDetection.h"
#include "ComboSystem.h"
#include "FrameData.h"
#include "../Characters/CharacterBase.h"
#include <algorithm>
#include <chrono>

namespace ArenaFighter {

struct CombatSystem::CombatSystemImpl {
    std::unique_ptr<DamageCalculator> damageCalculator;
    std::unique_ptr<HitDetection> hitDetection;
    std::unordered_map<int, std::unique_ptr<ComboSystem>> comboSystems;
    std::unordered_map<std::string, FrameData> frameDataRegistry;
    std::unordered_map<int, CombatState> combatStates;
    std::unordered_map<int, SpecialMoveSystem*> specialMoveSystems;
    
    // Timing tracking
    std::chrono::steady_clock::time_point lastUpdateTime;
};

CombatSystem::CombatSystem() : m_impl(std::make_unique<CombatSystemImpl>()) {
    m_impl->lastUpdateTime = std::chrono::steady_clock::now();
}

CombatSystem::~CombatSystem() = default;

bool CombatSystem::Initialize() {
    // Initialize subsystems
    m_impl->damageCalculator = std::make_unique<DamageCalculator>();
    m_impl->hitDetection = std::make_unique<HitDetection>();
    
    // Initialize with LSFDC patterns
    m_impl->damageCalculator->Initialize();
    m_impl->hitDetection->Initialize();
    
    return true;
}

void CombatSystem::Shutdown() {
    m_impl->comboSystems.clear();
    m_impl->frameDataRegistry.clear();
    m_impl->combatStates.clear();
    m_impl->specialMoveSystems.clear();
}

void CombatSystem::Update(float deltaTime) {
    UpdateCombatStates(deltaTime);
    UpdateManaRegeneration(deltaTime);
    ProcessActiveHitboxes(deltaTime);
    CleanExpiredCombos(deltaTime);
    
    // Update special move systems
    for (auto& [playerId, system] : m_impl->specialMoveSystems) {
        if (system) {
            system->update(deltaTime);
        }
    }
}

float CombatSystem::ProcessDamage(Character* attacker, Character* defender,
                                 float baseDamage, DamageType damageType,
                                 AttackType attackType, int comboCount) {
    if (!attacker || !defender || !m_impl->damageCalculator) {
        return 0.0f;
    }
    
    // Get combo count if not provided
    if (comboCount == 0 && m_impl->comboSystems.find(attacker->GetId()) != m_impl->comboSystems.end()) {
        comboCount = m_impl->comboSystems[attacker->GetId()]->GetHitCount();
    }
    
    // Calculate damage using LSFDC formula
    DamageCalculator::DamageParams params;
    params.baseDamage = baseDamage;
    params.attackerPower = attacker->GetPowerModifier();
    params.defenderDefense = defender->GetDefense();
    params.damageType = damageType;
    params.attackType = attackType;
    params.comboCount = comboCount;
    params.isCounter = defender->IsInCounterState();
    params.isCritical = attacker->RollCritical();
    params.attackerElement = attacker->GetElement();
    params.defenderElement = defender->GetElement();
    params.defenderState = defender->GetCurrentState();
    
    float finalDamage = m_impl->damageCalculator->CalculateDamage(params);
    
    // Apply blocking damage reduction
    if (IsBlocking(defender->GetId())) {
        float reduction = GetBlockDamageReduction(defender->GetId());
        finalDamage *= (1.0f - reduction);
        
        // Apply chip damage for blocked attacks
        if (attackType != AttackType::Throw) { // Throws bypass block
            finalDamage *= CHIP_DAMAGE_MULTIPLIER;
        }
    }
    
    // Apply combo damage limit (60% max health)
    if (m_impl->comboSystems.find(attacker->GetId()) != m_impl->comboSystems.end()) {
        auto& comboSystem = m_impl->comboSystems[attacker->GetId()];
        float totalComboDamage = comboSystem->GetTotalDamage() + finalDamage;
        float maxAllowedDamage = defender->GetMaxHealth() * MAX_COMBO_DAMAGE_PERCENT;
        
        if (totalComboDamage > maxAllowedDamage) {
            finalDamage = std::max(0.0f, maxAllowedDamage - comboSystem->GetTotalDamage());
        }
    }
    
    return finalDamage;
}

bool CombatSystem::CheckHit(const HitBox& attackBox, const HurtBox& defenseBox,
                           float activeFrames, float currentFrame) {
    if (!m_impl->hitDetection) {
        return false;
    }
    
    return m_impl->hitDetection->CheckCollision(attackBox, defenseBox, activeFrames, currentFrame);
}

void CombatSystem::RegisterHit(int attackerId, int defenderId, AttackType type, float damage) {
    // Get or create combo system for attacker
    if (m_impl->comboSystems.find(attackerId) == m_impl->comboSystems.end()) {
        m_impl->comboSystems[attackerId] = std::make_unique<ComboSystem>();
    }
    
    m_impl->comboSystems[attackerId]->RegisterHit(type, damage, defenderId);
    
    // Apply block stun if defender was blocking
    if (IsBlocking(defenderId)) {
        auto stateIt = m_impl->combatStates.find(defenderId);
        if (stateIt != m_impl->combatStates.end()) {
            // Block stun scales with attack type
            int blockStun = 0;
            switch (type) {
                case AttackType::Light:
                    blockStun = 8;
                    break;
                case AttackType::Medium:
                    blockStun = 12;
                    break;
                case AttackType::Heavy:
                    blockStun = 16;
                    break;
                case AttackType::Special:
                    blockStun = 20;
                    break;
                default:
                    blockStun = 10;
                    break;
            }
            stateIt->second.blockstunFrames = std::max(stateIt->second.blockstunFrames, blockStun);
            
            // Also apply block stun to special move system
            auto specialIt = m_impl->specialMoveSystems.find(defenderId);
            if (specialIt != m_impl->specialMoveSystems.end() && specialIt->second) {
                specialIt->second->applyBlockStun(static_cast<float>(blockStun));
            }
        }
    }
}

void CombatSystem::ResetCombo(int attackerId) {
    if (m_impl->comboSystems.find(attackerId) != m_impl->comboSystems.end()) {
        m_impl->comboSystems[attackerId]->Reset();
    }
}

int CombatSystem::GetComboCount(int attackerId) const {
    auto it = m_impl->comboSystems.find(attackerId);
    if (it != m_impl->comboSystems.end()) {
        return it->second->GetHitCount();
    }
    return 0;
}

float CombatSystem::GetComboScaling(int attackerId) const {
    auto it = m_impl->comboSystems.find(attackerId);
    if (it != m_impl->comboSystems.end()) {
        return it->second->GetCurrentScaling();
    }
    return 1.0f;
}

bool CombatSystem::IsValidCombo(int attackerId, float timeSinceLastHit) const {
    auto it = m_impl->comboSystems.find(attackerId);
    if (it != m_impl->comboSystems.end()) {
        return it->second->IsActive() && timeSinceLastHit < ComboSystem::COMBO_TIMEOUT;
    }
    return false;
}

void CombatSystem::RegisterFrameData(const std::string& characterName,
                                   const std::string& skillName,
                                   const FrameData& frameData) {
    std::string key = characterName + "_" + skillName;
    m_impl->frameDataRegistry[key] = frameData;
}

const FrameData* CombatSystem::GetFrameData(const std::string& characterName,
                                           const std::string& skillName) const {
    std::string key = characterName + "_" + skillName;
    auto it = m_impl->frameDataRegistry.find(key);
    if (it != m_impl->frameDataRegistry.end()) {
        return &it->second;
    }
    return nullptr;
}

bool CombatSystem::CanAffordSkill(Character* character, float manaCost) const {
    if (!character) return false;
    return character->GetCurrentMana() >= manaCost;
}

void CombatSystem::ConsumeMana(Character* character, float manaCost) {
    if (!character) return;
    character->ConsumeMana(manaCost);
}

bool CombatSystem::IsInHitstun(Character* character) const {
    if (!character) return false;
    auto it = m_impl->combatStates.find(character->GetId());
    if (it != m_impl->combatStates.end()) {
        return it->second.hitstunFrames > 0;
    }
    return false;
}

bool CombatSystem::IsInBlockstun(Character* character) const {
    if (!character) return false;
    auto it = m_impl->combatStates.find(character->GetId());
    if (it != m_impl->combatStates.end()) {
        return it->second.blockstunFrames > 0;
    }
    return false;
}

bool CombatSystem::CanAct(Character* character) const {
    return !IsInHitstun(character) && !IsInBlockstun(character);
}

int CombatSystem::GetRemainingHitstun(Character* character) const {
    if (!character) return 0;
    auto it = m_impl->combatStates.find(character->GetId());
    if (it != m_impl->combatStates.end()) {
        return it->second.hitstunFrames;
    }
    return 0;
}

void CombatSystem::UpdateCombatStates(float deltaTime) {
    int framesToUpdate = static_cast<int>(deltaTime * 60.0f); // 60 FPS
    
    for (auto& [characterId, state] : m_impl->combatStates) {
        if (state.hitstunFrames > 0) {
            state.hitstunFrames = std::max(0, state.hitstunFrames - framesToUpdate);
        }
        if (state.blockstunFrames > 0) {
            state.blockstunFrames = std::max(0, state.blockstunFrames - framesToUpdate);
        }
        
        // Update blocking states
        ProcessBlockingState(characterId, deltaTime);
    }
}

void CombatSystem::UpdateManaRegeneration(float deltaTime) {
    // Mana regeneration is handled by Character class
    // This is here for any global mana effects
}

void CombatSystem::ProcessActiveHitboxes(float deltaTime) {
    // Process active hitboxes - implementation depends on hitbox management system
    if (m_impl->hitDetection) {
        m_impl->hitDetection->UpdateActiveHitboxes(deltaTime);
    }
}

void CombatSystem::CleanExpiredCombos(float deltaTime) {
    for (auto& [attackerId, comboSystem] : m_impl->comboSystems) {
        comboSystem->Update(deltaTime);
        if (!comboSystem->IsActive()) {
            comboSystem->Reset();
        }
    }
}

void CombatSystem::HandleSpecialInput(int playerId, InputDirection direction, bool sPressed) {
    // Get the special move system for this player
    auto it = m_impl->specialMoveSystems.find(playerId);
    if (it == m_impl->specialMoveSystems.end() || !it->second) {
        return;
    }
    
    SpecialMoveSystem* specialSystem = it->second;
    
    // Handle S button state
    if (sPressed) {
        specialSystem->handleSButtonPress();
    } else {
        specialSystem->handleSButtonRelease();
    }
    
    // Handle directional input
    if (direction != InputDirection::Neutral) {
        specialSystem->handleDirectionalInput(direction);
    }
    
    // Try to execute special move if conditions are met
    if (sPressed && direction != InputDirection::Neutral) {
        // Check if character can act
        auto stateIt = m_impl->combatStates.find(playerId);
        if (stateIt != m_impl->combatStates.end()) {
            if (stateIt->second.hitstunFrames > 0 || stateIt->second.blockstunFrames > 0) {
                return; // Cannot execute specials while in stun
            }
        }
        
        // Try to execute the special move
        if (specialSystem->tryExecuteSpecialMove(direction)) {
            // Special move was executed successfully
            // Combat system doesn't need to do anything else here
        }
    }
}

void CombatSystem::ProcessBlockingState(int playerId, float deltaTime) {
    // Get combat state
    auto stateIt = m_impl->combatStates.find(playerId);
    if (stateIt == m_impl->combatStates.end()) {
        // Create new combat state if it doesn't exist
        m_impl->combatStates[playerId] = CombatState();
        stateIt = m_impl->combatStates.find(playerId);
    }
    
    // Get special move system to check block state
    auto specialIt = m_impl->specialMoveSystems.find(playerId);
    if (specialIt != m_impl->specialMoveSystems.end() && specialIt->second) {
        SpecialMoveSystem* specialSystem = specialIt->second;
        
        // Update blocking state
        bool wasBlocking = stateIt->second.isBlocking;
        stateIt->second.isBlocking = specialSystem->isBlocking();
        
        // Set block damage reduction based on block duration
        if (stateIt->second.isBlocking) {
            float blockTime = specialSystem->getBlockHeldTime();
            
            // Scale damage reduction based on how long block has been held
            // Start at 30% reduction, scale up to 50% at full duration
            float minReduction = 0.3f;
            float maxReduction = BLOCK_DAMAGE_REDUCTION;
            float t = std::min(blockTime / BlockState::BLOCK_ACTIVATION_TIME, 1.0f);
            
            stateIt->second.blockDamageReduction = minReduction + (maxReduction - minReduction) * t;
        } else {
            stateIt->second.blockDamageReduction = 0.0f;
        }
        
        // Handle block release
        if (wasBlocking && !stateIt->second.isBlocking) {
            // Add a small recovery when releasing block
            stateIt->second.blockstunFrames = 5; // 5 frames of recovery
        }
    }
}

bool CombatSystem::IsBlocking(int playerId) const {
    auto it = m_impl->combatStates.find(playerId);
    if (it != m_impl->combatStates.end()) {
        return it->second.isBlocking;
    }
    return false;
}

float CombatSystem::GetBlockDamageReduction(int playerId) const {
    auto it = m_impl->combatStates.find(playerId);
    if (it != m_impl->combatStates.end()) {
        return it->second.blockDamageReduction;
    }
    return 0.0f;
}

void CombatSystem::RegisterSpecialMoveSystem(int playerId, SpecialMoveSystem* system) {
    if (system) {
        m_impl->specialMoveSystems[playerId] = system;
    }
}

SpecialMoveSystem* CombatSystem::GetSpecialMoveSystem(int playerId) const {
    auto it = m_impl->specialMoveSystems.find(playerId);
    if (it != m_impl->specialMoveSystems.end()) {
        return it->second;
    }
    return nullptr;
}

} // namespace ArenaFighter