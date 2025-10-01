#pragma once

#include "../GameModes/GameMode.h"
#include "../GameModes/GameModeManager.h"
#include "../Characters/CharacterBase.h"
#include "../Characters/CharacterFactory.h"
#include "../Ranking/RankingSystem.h"
#include <DirectXMath.h>

namespace ArenaFighter {

/**
 * @brief Adapter to make the provided GameplayManager work with DFR's correct system
 * 
 * Maps between the incorrect Character.h API and our proper DFR implementation
 */
class GameplayManagerAdapter {
public:
    // Game mode mapping
    static GameModeType ConvertGameMode(int providedMode) {
        switch (providedMode) {
            case 0: return GameModeType::Versus;        // Ranked1v1
            case 1: return GameModeType::DeathMatch;    // DeathMatch
            case 2: return GameModeType::DeathMatch;    // TeamDeathMatch
            case 3: return GameModeType::BeastMode;     // BeastMode
            case 4: return GameModeType::Versus;        // ForGlory3v3
            case 5: return GameModeType::ForGlory;      // ForGlory1v1Dual
            case 6: return GameModeType::DimensionalRift; // DimensionalRift
            case 7: return GameModeType::Tournament;    // Tournament
            case 8: return GameModeType::Training;      // Training
            default: return GameModeType::Training;
        }
    }
    
    // Character wrapper to adapt the API
    class CharacterAdapter {
    private:
        std::unique_ptr<CharacterBase> m_dfrCharacter;
        DirectX::XMFLOAT3 m_position;
        DirectX::XMFLOAT3 m_velocity;
        bool m_isBlocking;
        float m_blockHoldTime;
        
    public:
        CharacterAdapter(const std::string& name, int categoryId) 
            : m_position(0, 0, 0)
            , m_velocity(0, 0, 0)
            , m_isBlocking(false)
            , m_blockHoldTime(0.0f) {
            
            // Create DFR character
            m_dfrCharacter = CharacterFactory::GetInstance().CreateCharacterByName(name);
            if (!m_dfrCharacter) {
                // Create a default character if not found
                m_dfrCharacter = std::make_unique<CharacterBase>(name, 
                    static_cast<CharacterCategory>(categoryId));
            }
        }
        
        // Adapt the incorrect API to DFR's correct system
        bool ExecuteSpecialSkill(const std::string& direction) {
            InputDirection dir = InputDirection::Up;
            if (direction == "Up") dir = InputDirection::Up;
            else if (direction == "Down") dir = InputDirection::Down;
            else if (direction == "Left") dir = InputDirection::Left;
            else if (direction == "Right") dir = InputDirection::Right;
            
            // Special moves use MANA ONLY (no cooldowns in DFR)
            if (m_dfrCharacter->CanExecuteSpecialMove(dir)) {
                m_dfrCharacter->ExecuteSpecialMove(dir);
                return true;
            }
            return false;
        }
        
        bool ExecuteGearSkill(int slot) {
            // Gear skills have BOTH mana AND cooldowns
            int skillIndex = slot * 2; // Each gear has 2 skills
            
            const auto& skill = m_dfrCharacter->GetGearSkills()[skillIndex];
            if (!m_dfrCharacter->IsGearSkillOnCooldown(skillIndex) &&
                m_dfrCharacter->CanAffordSkill(skill.manaCost)) {
                m_dfrCharacter->ConsumeMana(skill.manaCost);
                m_dfrCharacter->StartGearSkillCooldown(skillIndex);
                return true;
            }
            return false;
        }
        
        void Update(float deltaTime) {
            m_dfrCharacter->Update(deltaTime);
            
            // Update position
            m_position.x += m_velocity.x * deltaTime;
            m_position.y += m_velocity.y * deltaTime;
            m_position.z += m_velocity.z * deltaTime;
            
            // Apply friction
            m_velocity.x *= 0.9f;
            m_velocity.z *= 0.9f;
            
            // Update blocking
            if (m_isBlocking) {
                m_blockHoldTime += deltaTime;
                if (m_blockHoldTime >= 1.0f && !m_dfrCharacter->IsBlocking()) {
                    m_dfrCharacter->StartBlocking();
                }
            }
        }
        
        void Move(float x, float z) {
            if (!m_dfrCharacter->IsBlocking()) {
                m_velocity.x = x * m_dfrCharacter->GetSpeed() / 20.0f;
                m_velocity.z = z * m_dfrCharacter->GetSpeed() / 20.0f;
            }
        }
        
        void StartBlocking() {
            m_isBlocking = true;
            m_blockHoldTime = 0.0f;
        }
        
        void StopBlocking() {
            m_isBlocking = false;
            m_blockHoldTime = 0.0f;
            m_dfrCharacter->StopBlocking();
        }
        
        void SwitchStance() {
            if (m_dfrCharacter->HasStanceSystem()) {
                int currentStance = m_dfrCharacter->GetCurrentStance();
                m_dfrCharacter->SwitchStance(1 - currentStance); // Toggle between 0 and 1
            }
        }
        
        void DealDamageTo(CharacterAdapter* target, float baseDamage) {
            if (target) {
                float actualDamage = baseDamage * m_dfrCharacter->GetPowerModifier();
                target->TakeDamage(actualDamage);
            }
        }
        
        void TakeDamage(float damage) {
            m_dfrCharacter->TakeDamage(damage);
        }
        
        bool IsDefeated() const {
            return !m_dfrCharacter->IsAlive();
        }
        
        bool IsInRange(CharacterAdapter* target, float range) const {
            if (!target) return false;
            float dx = target->m_position.x - m_position.x;
            float dz = target->m_position.z - m_position.z;
            float distance = sqrtf(dx * dx + dz * dz);
            return distance <= range;
        }
        
        // Getters
        const std::string& GetName() const { return m_dfrCharacter->GetName(); }
        DirectX::XMFLOAT3 GetPosition() const { return m_position; }
        void SetPosition(const DirectX::XMFLOAT3& pos) { m_position = pos; }
        
        // Stats adapter
        struct StatsAdapter {
            const CharacterBase* character;
            
            int GetCurrentHealth() const { 
                return static_cast<int>(character->GetCurrentHealth()); 
            }
            int GetMaxHealth() const { 
                return static_cast<int>(character->GetMaxHealth()); 
            }
            int GetCurrentMana() const { 
                return static_cast<int>(character->GetCurrentMana()); 
            }
            int GetMaxMana() const { 
                return static_cast<int>(character->GetMaxMana()); 
            }
        };
        
        StatsAdapter GetStats() const {
            return StatsAdapter{m_dfrCharacter.get()};
        }
        
        CharacterBase* GetDFRCharacter() { return m_dfrCharacter.get(); }
    };
};

/**
 * @brief Corrected GameplayManager that uses DFR's proper skill system
 */
class DFRGameplayManager {
private:
    std::unique_ptr<GameModeManager> m_modeManager;
    std::unique_ptr<RankingSystem> m_rankingSystem;
    GameMode* m_currentMode;
    
public:
    DFRGameplayManager() {
        m_modeManager = std::make_unique<GameModeManager>();
        m_rankingSystem = std::make_unique<RankingSystem>();
        m_currentMode = nullptr;
    }
    
    void InitializeRanked1v1(std::vector<CharacterBase*> playerChars,
                            std::vector<CharacterBase*> enemyChars) {
        auto config = m_modeManager->GetModeConfig(GameModeType::Versus);
        
        // Configure for current week's slot count
        int week = m_rankingSystem->GetCurrentWeek();
        if (week <= 2) {
            config.maxCharactersPerPlayer = 1;
        } else {
            config.maxCharactersPerPlayer = 3;
        }
        
        m_currentMode = m_modeManager->CreateGameMode(GameModeType::Versus, config);
    }
    
    void Update(float deltaTime) {
        if (m_currentMode) {
            m_currentMode->Update(deltaTime);
        }
        
        // Update rankings if in ranked mode
        if (m_currentMode && m_currentMode->GetModeType() == GameModeType::Versus) {
            // Check for match completion and update rankings
            if (m_currentMode->GetMatchState() == MatchState::MatchEnd) {
                auto result = m_currentMode->GetMatchResult();
                UpdateRankings(result);
            }
        }
    }
    
private:
    void UpdateRankings(const MatchResult& result) {
        // Update character rankings based on match result
        for (const auto& playerResult : result.playerResults) {
            m_rankingSystem->recordMatchResult(
                playerResult.playerId,
                playerResult.opponentId,
                playerResult.characterId,
                playerResult.opponentCharacterId,
                playerResult.maxCombo,
                playerResult.damageDealt,
                result.matchDuration
            );
        }
    }
};

} // namespace ArenaFighter