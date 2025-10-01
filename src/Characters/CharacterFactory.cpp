#include "CharacterFactory.h"
#include <algorithm>
#include <iostream>
#include "Murim/HyukWoonSung.h"

namespace ArenaFighter {

CharacterFactory& CharacterFactory::GetInstance() {
    static CharacterFactory instance;
    return instance;
}

void CharacterFactory::RegisterCharacter(int id, const std::string& name,
                                        CharacterCategory category,
                                        const std::string& description,
                                        CharacterCreator creator) {
    // Register creator function
    m_creators[id] = creator;
    
    // Store character info
    CharacterInfo info{id, name, category, description, true};
    m_characterInfo[id] = info;
    m_nameToId[name] = id;
    m_roster.push_back(info);
    
    // Sort roster by category then by name
    std::sort(m_roster.begin(), m_roster.end(),
              [](const CharacterInfo& a, const CharacterInfo& b) {
                  if (a.category != b.category) {
                      return static_cast<int>(a.category) < static_cast<int>(b.category);
                  }
                  return a.name < b.name;
              });
}

std::unique_ptr<CharacterBase> CharacterFactory::CreateCharacter(int id) const {
    auto it = m_creators.find(id);
    if (it != m_creators.end()) {
        return it->second();
    }
    
    std::cerr << "CharacterFactory: Character ID " << id << " not found!" << std::endl;
    return nullptr;
}

std::unique_ptr<CharacterBase> CharacterFactory::CreateCharacterByName(const std::string& name) const {
    auto it = m_nameToId.find(name);
    if (it != m_nameToId.end()) {
        return CreateCharacter(it->second);
    }
    
    std::cerr << "CharacterFactory: Character '" << name << "' not found!" << std::endl;
    return nullptr;
}

std::vector<CharacterFactory::CharacterInfo> 
CharacterFactory::GetCharactersByCategory(CharacterCategory category) const {
    std::vector<CharacterInfo> result;
    
    for (const auto& info : m_roster) {
        if (info.category == category) {
            result.push_back(info);
        }
    }
    
    return result;
}

const CharacterFactory::CharacterInfo* CharacterFactory::GetCharacterInfo(int id) const {
    auto it = m_characterInfo.find(id);
    if (it != m_characterInfo.end()) {
        return &it->second;
    }
    return nullptr;
}

const CharacterFactory::CharacterInfo* 
CharacterFactory::GetCharacterInfoByName(const std::string& name) const {
    auto it = m_nameToId.find(name);
    if (it != m_nameToId.end()) {
        return GetCharacterInfo(it->second);
    }
    return nullptr;
}

bool CharacterFactory::IsCharacterRegistered(int id) const {
    return m_creators.find(id) != m_creators.end();
}

void CharacterFactory::InitializeDefaultCharacters() {
    // Register all character categories
    RegisterSystemCharacters();
    RegisterGodsHeroesCharacters();
    RegisterMurimCharacters();
    RegisterCultivationCharacters();
    RegisterAnimalCharacters();
    RegisterMonstersCharacters();
    RegisterChaosCharacters();
}

void CharacterFactory::RegisterSystemCharacters() {
    // Example system characters - these would be implemented in separate files
    // For now, registering placeholders
    
    // Digital Samurai
    RegisterCharacter(101, "Digital Samurai", CharacterCategory::System,
        "A cyber-warrior merging traditional combat with digital enhancement",
        []() { return std::make_unique<CharacterBase>("Digital Samurai", 
                                                      CharacterCategory::System, 
                                                      StatMode::Attack); });
    
    // Tech Mage
    RegisterCharacter(102, "Tech Mage", CharacterCategory::System,
        "Master of digital sorcery and virtual reality manipulation",
        []() { return std::make_unique<CharacterBase>("Tech Mage", 
                                                      CharacterCategory::System, 
                                                      StatMode::Special); });
    
    // Cyber Ninja
    RegisterCharacter(103, "Cyber Ninja", CharacterCategory::System,
        "Stealth assassin with advanced cloaking and hacking abilities",
        []() { return std::make_unique<CharacterBase>("Cyber Ninja", 
                                                      CharacterCategory::System, 
                                                      StatMode::Hybrid); });
}

void CharacterFactory::RegisterGodsHeroesCharacters() {
    // Zeus
    RegisterCharacter(201, "Zeus", CharacterCategory::GodsHeroes,
        "King of Olympus wielding divine lightning",
        []() { return std::make_unique<CharacterBase>("Zeus", 
                                                      CharacterCategory::GodsHeroes, 
                                                      StatMode::Attack); });
    
    // Thor
    RegisterCharacter(202, "Thor", CharacterCategory::GodsHeroes,
        "Norse god of thunder with mighty Mjolnir",
        []() { return std::make_unique<CharacterBase>("Thor", 
                                                      CharacterCategory::GodsHeroes, 
                                                      StatMode::Hybrid); });
    
    // Athena
    RegisterCharacter(203, "Athena", CharacterCategory::GodsHeroes,
        "Goddess of wisdom and strategic warfare",
        []() { return std::make_unique<CharacterBase>("Athena", 
                                                      CharacterCategory::GodsHeroes, 
                                                      StatMode::Defense); });
}

void CharacterFactory::RegisterMurimCharacters() {
    // Hyuk Woon Sung - S-Tier Unique
    RegisterCharacter(300, "Hyuk Woon Sung", CharacterCategory::Murim,
        "S-Tier martial artist with dual stance system and Divine Arts",
        []() { return std::make_unique<HyukWoonSung>(); });
    
    // Heavenly Demon
    RegisterCharacter(301, "Heavenly Demon", CharacterCategory::Murim,
        "Master of demonic martial arts",
        []() { return std::make_unique<CharacterBase>("Heavenly Demon", 
                                                      CharacterCategory::Murim, 
                                                      StatMode::Attack); });
    
    // Sword Saint
    RegisterCharacter(302, "Sword Saint", CharacterCategory::Murim,
        "Peerless swordsman who achieved enlightenment",
        []() { return std::make_unique<CharacterBase>("Sword Saint", 
                                                      CharacterCategory::Murim, 
                                                      StatMode::Hybrid); });
    
    // Poison Phoenix
    RegisterCharacter(303, "Poison Phoenix", CharacterCategory::Murim,
        "Mistress of deadly toxins and phoenix techniques",
        []() { return std::make_unique<CharacterBase>("Poison Phoenix", 
                                                      CharacterCategory::Murim, 
                                                      StatMode::Special); });
}

void CharacterFactory::RegisterCultivationCharacters() {
    // Jade Emperor
    RegisterCharacter(401, "Jade Emperor", CharacterCategory::Cultivation,
        "Immortal ruler who transcended mortality",
        []() { return std::make_unique<CharacterBase>("Jade Emperor", 
                                                      CharacterCategory::Cultivation, 
                                                      StatMode::Special); });
    
    // Dao Seeker
    RegisterCharacter(402, "Dao Seeker", CharacterCategory::Cultivation,
        "Cultivator pursuing the ultimate truth",
        []() { return std::make_unique<CharacterBase>("Dao Seeker", 
                                                      CharacterCategory::Cultivation, 
                                                      StatMode::Hybrid); });
    
    // Spirit Alchemist
    RegisterCharacter(403, "Spirit Alchemist", CharacterCategory::Cultivation,
        "Master of pill refinement and spiritual flames",
        []() { return std::make_unique<CharacterBase>("Spirit Alchemist", 
                                                      CharacterCategory::Cultivation, 
                                                      StatMode::Special); });
}

void CharacterFactory::RegisterAnimalCharacters() {
    // Thunder Wolf
    RegisterCharacter(501, "Thunder Wolf", CharacterCategory::Animal,
        "Alpha predator channeling storm energy",
        []() { return std::make_unique<CharacterBase>("Thunder Wolf", 
                                                      CharacterCategory::Animal, 
                                                      StatMode::Attack); });
    
    // Ancient Turtle
    RegisterCharacter(502, "Ancient Turtle", CharacterCategory::Animal,
        "Wise guardian with impenetrable defense",
        []() { return std::make_unique<CharacterBase>("Ancient Turtle", 
                                                      CharacterCategory::Animal, 
                                                      StatMode::Defense); });
    
    // Phoenix
    RegisterCharacter(503, "Phoenix", CharacterCategory::Animal,
        "Immortal firebird of rebirth",
        []() { return std::make_unique<CharacterBase>("Phoenix", 
                                                      CharacterCategory::Animal, 
                                                      StatMode::Special); });
}

void CharacterFactory::RegisterMonstersCharacters() {
    // Shadow Demon
    RegisterCharacter(601, "Shadow Demon", CharacterCategory::Monsters,
        "Dark entity from the void between worlds",
        []() { return std::make_unique<CharacterBase>("Shadow Demon", 
                                                      CharacterCategory::Monsters, 
                                                      StatMode::Attack); });
    
    // Undead King
    RegisterCharacter(602, "Undead King", CharacterCategory::Monsters,
        "Lich lord commanding legions of undead",
        []() { return std::make_unique<CharacterBase>("Undead King", 
                                                      CharacterCategory::Monsters, 
                                                      StatMode::Defense); });
    
    // Eldritch Horror
    RegisterCharacter(603, "Eldritch Horror", CharacterCategory::Monsters,
        "Cosmic nightmare defying comprehension",
        []() { return std::make_unique<CharacterBase>("Eldritch Horror", 
                                                      CharacterCategory::Monsters, 
                                                      StatMode::Custom); });
}

void CharacterFactory::RegisterChaosCharacters() {
    // Void Walker
    RegisterCharacter(701, "Void Walker", CharacterCategory::Chaos,
        "Entity that exists between dimensions",
        []() { return std::make_unique<CharacterBase>("Void Walker", 
                                                      CharacterCategory::Chaos, 
                                                      StatMode::Hybrid); });
    
    // Chaos Mage
    RegisterCharacter(702, "Chaos Mage", CharacterCategory::Chaos,
        "Sorcerer wielding unpredictable reality-warping magic",
        []() { return std::make_unique<CharacterBase>("Chaos Mage", 
                                                      CharacterCategory::Chaos, 
                                                      StatMode::Special); });
    
    // Entropy Knight
    RegisterCharacter(703, "Entropy Knight", CharacterCategory::Chaos,
        "Warrior accelerating the heat death of the universe",
        []() { return std::make_unique<CharacterBase>("Entropy Knight", 
                                                      CharacterCategory::Chaos, 
                                                      StatMode::Attack); });
}

} // namespace ArenaFighter