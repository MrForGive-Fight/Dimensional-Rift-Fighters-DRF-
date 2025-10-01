#pragma once

#include "GameModesManager.h"
#include "../Characters/CharacterBase.h"
#include <DirectXMath.h>

namespace ArenaFighter {

enum class BeastType {
    Titan,          // Giant humanoid, high HP
    Dragon,         // Flying, breath attacks
    Behemoth,       // Tank, area attacks
    Phoenix,        // Resurrect ability
    Leviathan,      // Water-based attacks
    Demon,          // Balanced all-around
    Mecha,          // Technology-based
    Elemental       // Pure elemental form
};

struct BeastTransformation {
    BeastType type;
    std::string modelPath;
    float sizeMultiplier = 3.0f;
    
    // Stat multipliers
    float healthMultiplier = 10.0f;
    float attackMultiplier = 3.0f;
    float defenseMultiplier = 2.0f;
    float speedMultiplier = 0.7f;  // Usually slower
    
    // Special abilities
    std::vector<std::string> beastSkills;
    bool canFly = false;
    bool hasAreaDamage = false;
    bool hasRegen = false;
    float regenRate = 0.0f;
    
    // Weaknesses
    float headDamageMultiplier = 2.0f;  // Critical spots
    float backDamageMultiplier = 1.5f;
    std::vector<ElementType> weakElements;
};

class BeastModeSystem {
public:
    BeastModeSystem();
    
    // Transformation management
    bool TransformToBeast(int playerID, BeastType type);
    bool RevertToHuman(int playerID);
    bool IsTransformed(int playerID) const;
    BeastType GetBeastType(int playerID) const;
    
    // Beast meter system
    void AddBeastEnergy(int playerID, float amount);
    float GetBeastEnergy(int playerID) const;
    bool CanTransform(int playerID) const;
    
    // Transformation properties
    const BeastTransformation* GetTransformation(BeastType type) const;
    float GetRemainingDuration(int playerID) const;
    
    // Combat modifications
    float CalculateBeastDamage(float baseDamage, int attackerID, int targetID);
    bool CheckWeakspotHit(int beastID, const DirectX::XMFLOAT3& hitLocation);
    
    // Update
    void Update(float deltaTime);
    
private:
    std::unordered_map<BeastType, BeastTransformation> m_transformations;
    
    struct ActiveBeast {
        int playerID;
        BeastType type;
        float duration = 60.0f;  // 1 minute default
        float energy = 100.0f;
        bool isRaging = false;
    };
    std::unordered_map<int, ActiveBeast> m_activeBeasts;
    std::unordered_map<int, float> m_beastEnergy;  // Build-up meter
    
    static constexpr float MAX_BEAST_ENERGY = 100.0f;
    static constexpr float TRANSFORM_COST = 100.0f;
    static constexpr float RAGE_THRESHOLD = 25.0f;  // HP% for rage mode
    
    void InitializeTransformations();
    void ApplyRageMode(int beastID);
};

class BeastMode : public GameModeBase {
public:
    BeastMode();
    bool Initialize() override;
    void Start() override;
    void Update(float deltaTime) override;
    void End() override;
    bool IsMatchComplete() const override;
    int GetWinner() const override;
    
    // Game rules
    void SetBeastPlayer(int playerID);
    void SetHunterCount(int count) { m_hunterCount = count; }
    void EnableRotatingBeast(bool enable) { m_rotatingBeast = enable; }
    
    // Beast selection
    void SelectRandomBeast();
    void PromoteHunterToBeast(int hunterID);
    
    // Scoring
    void OnBeastKill(int victimID);
    void OnHunterKillBeast(int hunterID);
    void OnHunterDamageBeast(int hunterID, float damage);
    
    // Special mechanics
    void SpawnBeastPowerUp(const DirectX::XMFLOAT3& position);
    void ActivateBeastRage();
    
private:
    BeastModeSystem m_beastSystem;
    
    // Players
    int m_currentBeastID = -1;
    std::vector<int> m_hunterIDs;
    int m_hunterCount = 7;  // 1v7 default
    
    // Game state
    float m_beastSurvivalTime = 0.0f;
    int m_beastKills = 0;
    int m_hunterDeaths = 0;
    bool m_rotatingBeast = true;  // Pass beast role on death
    
    // Scoring
    struct Score {
        float survivalTime = 0.0f;
        int kills = 0;
        float damageDealt = 0.0f;
        int timesAsBeast = 0;
    };
    std::unordered_map<int, Score> m_playerScores;
    
    // Match settings
    float m_roundTime = 300.0f;  // 5 minutes per round
    int m_currentRound = 1;
    static constexpr int MAX_ROUNDS = 3;
    
    // Balance mechanics
    void BalanceBeastPower();
    float GetHunterDamageBonus() const;
    bool ShouldSpawnPowerUp() const;
};

// Beast Mode specific abilities
namespace BeastAbilities {
    // Titan abilities
    const std::vector<std::string> TitanSkills = {
        "Earth Shatter",    // Ground pound AOE
        "Titan's Grip",     // Grab and throw
        "Stone Skin",       // Temporary defense boost
        "Colossus Roar"    // Stun nearby enemies
    };
    
    // Dragon abilities  
    const std::vector<std::string> DragonSkills = {
        "Fire Breath",      // Cone fire attack
        "Wing Gust",        // Knockback AOE
        "Aerial Dive",      // Diving attack
        "Dragon's Fury"     // Ultimate fire storm
    };
    
    // Phoenix abilities
    const std::vector<std::string> PhoenixSkills = {
        "Flame Wings",      // Fire trail while flying
        "Rebirth",          // Resurrect once per match
        "Solar Flare",      // Blind enemies
        "Inferno Burst"     // Explode on death
    };
}

// Hunter equipment for Beast Mode
class BeastHunterKit {
public:
    struct HunterWeapon {
        std::string name;
        float damageVsBeast;
        float fireRate;
        int ammo;
        bool isPiercing;
    };
    
    static std::vector<HunterWeapon> GetHunterLoadout() {
        return {
            {"Beast Slayer Rifle", 150.0f, 0.5f, 30, true},
            {"Explosive Harpoon", 300.0f, 2.0f, 5, false},
            {"Chain Lightning Gun", 100.0f, 0.1f, 100, false},
            {"Gravity Hammer", 250.0f, 1.5f, -1, false}
        };
    }
    
    static std::vector<std::string> GetHunterGadgets() {
        return {
            "Grappling Hook",   // Mobility
            "Beast Trap",       // Slow beast
            "Smoke Grenade",    // Escape tool
            "Damage Amplifier"  // Team buff
        };
    }
};

} // namespace ArenaFighter