#pragma once

#include <string>
#include <unordered_map>

namespace ArenaFighter {

// Forward declaration from CharacterBase.h
enum class CharacterCategory;

/**
 * @brief Category-specific traits and bonuses for character types
 * 
 * Each category has unique visual themes and stat modifiers
 * Following CLAUDE.md specifications
 */
struct CategoryTraits {
    // Visual theme properties
    std::string themeDescription;
    std::string primaryColor;
    std::string secondaryColor;
    std::string effectStyle; // Particle effect style
    std::string auraType;
    
    // Category-specific stat modifiers (multiplicative)
    float healthModifier = 1.0f;
    float manaModifier = 1.0f;
    float defenseModifier = 1.0f;
    float speedModifier = 1.0f;
    float powerModifier = 1.0f;
    
    // Special properties
    float criticalChanceBonus = 0.0f; // Additive bonus
    float manaRegenModifier = 1.0f;
    bool hasStanceSystem = false;
    bool hasEvolutionSystem = false;
    bool hasTransformationSystem = false;
};

/**
 * @brief Stat distribution modes for characters
 * 
 * Defines how a character's stats are distributed
 */
enum class StatMode {
    Attack,    // +20% power, -10% defense
    Defense,   // +20% defense, -10% speed
    Special,   // +20% mana, +10% mana regen
    Hybrid,    // Balanced stats
    Custom     // Character-specific distribution
};

/**
 * @brief Manager for character categories and their properties
 */
class CharacterCategoryManager {
public:
    static CharacterCategoryManager& GetInstance();
    
    // Get category traits
    const CategoryTraits& GetCategoryTraits(CharacterCategory category) const;
    
    // Get visual theme information
    std::string GetCategoryTheme(CharacterCategory category) const;
    std::string GetCategoryDescription(CharacterCategory category) const;
    
    // Apply category bonuses to base stats
    void ApplyCategoryModifiers(CharacterCategory category, 
                                float& health, float& mana, 
                                float& defense, float& speed, 
                                float& power) const;
    
    // Get stat mode modifiers
    void ApplyStatModeModifiers(StatMode mode,
                                float& health, float& mana,
                                float& defense, float& speed,
                                float& power) const;
    
private:
    CharacterCategoryManager();
    ~CharacterCategoryManager() = default;
    
    // Delete copy/move constructors
    CharacterCategoryManager(const CharacterCategoryManager&) = delete;
    CharacterCategoryManager& operator=(const CharacterCategoryManager&) = delete;
    
    // Category trait definitions
    std::unordered_map<CharacterCategory, CategoryTraits> m_categoryTraits;
    
    // Initialize category traits
    void InitializeCategoryTraits();
};

} // namespace ArenaFighter