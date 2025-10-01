#pragma once

#include <string>
#include <vector>
#include <d3d11.h>

namespace ArenaFighter {

constexpr float BASE_HEALTH = 1000.0f;
constexpr float BASE_MANA = 100.0f;
constexpr float MANA_REGEN = 5.0f;  // Mana regeneration per second
constexpr int BASE_DEFENSE = 100;
constexpr int BASE_SPEED = 100;

struct SkillData {
    std::string m_id;
    std::string m_name;
    float m_manaCost;
    float m_baseDamage;
    float m_range;
    int m_startupFrames;
    int m_activeFrames;
    int m_recoveryFrames;
};

struct GearData {
    std::string m_id;
    std::string m_name;
    std::vector<SkillData> m_skills; // 2 skills per gear
};

struct CharacterData {
    std::string m_id;
    std::string m_name;
    std::string m_category; // System, GodsHeroes, Murim, Cultivation, Animal, Monsters, Chaos
    int m_level;
    bool m_owned;
    ID3D11ShaderResourceView* m_portraitTexture;
    ID3D11ShaderResourceView* m_iconTexture;
    
    // Stats
    float m_health;
    float m_mana;
    int m_defense;
    int m_speed;
    
    // Display stats (simplified)
    int m_attack;
    int m_special;
    
    // Gear system (4 gears per character)
    std::vector<GearData> m_gears;
    
    CharacterData();
    void initializeDefaultGears();
};

} // namespace ArenaFighter