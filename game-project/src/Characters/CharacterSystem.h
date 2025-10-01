#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

namespace ArenaFighter {

enum class CharacterCategory {
    System,
    GodsHeroes,
    Murim,
    Cultivation,
    Animal,
    Monsters,
    Chaos
};

enum class StatMode {
    Attack,
    Defense,
    Special,
    Hybrid,
    Custom
};

struct CharacterStats {
    float health = 1000.0f;
    float maxHealth = 1000.0f;
    float mana = 100.0f;
    float maxMana = 100.0f;
    
    float attackPower = 100.0f;
    float defensePower = 100.0f;
    float specialPower = 100.0f;
    float speed = 100.0f;
    
    StatMode currentMode = StatMode::Attack;
};

struct Skill {
    std::string name;
    float manaCost;
    float damage;
    float cooldown = 0.0f; // Only for ultimate skills
    std::string animation;
};

class Character {
public:
    Character(const std::string& name, CharacterCategory category);
    
    // Basic Info
    const std::string& GetName() const { return m_name; }
    CharacterCategory GetCategory() const { return m_category; }
    
    // Stats Management
    CharacterStats& GetStats() { return m_stats; }
    void SetStatMode(StatMode mode);
    void TakeDamage(float damage);
    void UseMana(float amount);
    void Heal(float amount);
    void RestoreMana(float amount);
    
    // Skills (4 gears × 2 skills = 8 total)
    void SetSkill(int gearIndex, int skillSlot, const Skill& skill);
    const Skill* GetSkill(int gearIndex, int skillSlot) const;
    bool CanUseSkill(int gearIndex, int skillSlot) const;
    
    // Character State
    bool IsAlive() const { return m_stats.health > 0; }
    
private:
    std::string m_name;
    CharacterCategory m_category;
    CharacterStats m_stats;
    
    // 4 gears, 2 skills each
    Skill m_skills[4][2];
    int m_currentGear = 0;
};

class CharacterSystem {
public:
    CharacterSystem() = default;
    ~CharacterSystem() = default;

    bool Initialize();
    void Shutdown();
    void Update(float deltaTime);
    void Render(class RenderingSystem* renderer);

    // Character Management
    std::shared_ptr<Character> CreateCharacter(const std::string& name, CharacterCategory category);
    std::shared_ptr<Character> LoadCharacterPreset(const std::string& presetName);
    
    // Character Switching (Lost Saga style)
    void QueueCharacterSwitch(int playerID, const std::string& characterName);
    void ProcessCharacterSwitch(int playerID);
    bool CanSwitchCharacter(int playerID) const;
    
    // Active Characters
    Character* GetPlayerCharacter(int playerID);
    const std::vector<Character*> GetAllActiveCharacters() const;

private:
    std::unordered_map<std::string, std::shared_ptr<Character>> m_characterDatabase;
    std::unordered_map<int, std::shared_ptr<Character>> m_activeCharacters;
    std::unordered_map<int, std::string> m_pendingSwitches;
    
    // Character switch cooldown
    std::unordered_map<int, float> m_switchCooldowns;
    static constexpr float SWITCH_COOLDOWN = 3.0f;
};

} // namespace ArenaFighter