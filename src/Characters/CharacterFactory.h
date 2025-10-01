#pragma once

#include <memory>
#include <unordered_map>
#include <functional>
#include <vector>
#include <string>
#include "CharacterBase.h"

namespace ArenaFighter {

/**
 * @brief Factory class for creating and managing character instances
 * 
 * Singleton pattern following CLAUDE.md specifications
 * Manages character registration and creation by ID/category
 */
class CharacterFactory {
public:
    // Character creation function type
    using CharacterCreator = std::function<std::unique_ptr<CharacterBase>()>;
    
    // Character info for roster management
    struct CharacterInfo {
        int id;
        std::string name;
        CharacterCategory category;
        std::string description;
        bool isUnlocked = true; // For future unlock system
    };
    
    // Get singleton instance
    static CharacterFactory& GetInstance();
    
    // Register a character type
    void RegisterCharacter(int id, const std::string& name, 
                          CharacterCategory category,
                          const std::string& description,
                          CharacterCreator creator);
    
    // Create character by ID
    std::unique_ptr<CharacterBase> CreateCharacter(int id) const;
    
    // Create character by name
    std::unique_ptr<CharacterBase> CreateCharacterByName(const std::string& name) const;
    
    // Get all characters in a category
    std::vector<CharacterInfo> GetCharactersByCategory(CharacterCategory category) const;
    
    // Get full character roster
    const std::vector<CharacterInfo>& GetCharacterRoster() const { return m_roster; }
    
    // Get character info
    const CharacterInfo* GetCharacterInfo(int id) const;
    const CharacterInfo* GetCharacterInfoByName(const std::string& name) const;
    
    // Check if character is registered
    bool IsCharacterRegistered(int id) const;
    
    // Initialize default characters (called once at startup)
    void InitializeDefaultCharacters();
    
private:
    CharacterFactory() = default;
    ~CharacterFactory() = default;
    
    // Delete copy/move constructors
    CharacterFactory(const CharacterFactory&) = delete;
    CharacterFactory& operator=(const CharacterFactory&) = delete;
    
    // Character registry
    std::unordered_map<int, CharacterCreator> m_creators;
    std::unordered_map<int, CharacterInfo> m_characterInfo;
    std::unordered_map<std::string, int> m_nameToId;
    std::vector<CharacterInfo> m_roster;
    
    // Next available ID for auto-registration
    int m_nextAutoId = 1000;
    
    // Helper to register built-in characters
    void RegisterSystemCharacters();
    void RegisterGodsHeroesCharacters();
    void RegisterMurimCharacters();
    void RegisterCultivationCharacters();
    void RegisterAnimalCharacters();
    void RegisterMonstersCharacters();
    void RegisterChaosCharacters();
};

/**
 * @brief Helper macro for character registration
 * 
 * Use in character implementation files to auto-register
 */
#define REGISTER_CHARACTER(id, name, category, description, className) \
    namespace { \
        struct className##Registrar { \
            className##Registrar() { \
                CharacterFactory::GetInstance().RegisterCharacter( \
                    id, name, category, description, \
                    []() { return std::make_unique<className>(); } \
                ); \
            } \
        }; \
        static className##Registrar s_##className##Registrar; \
    }

} // namespace ArenaFighter