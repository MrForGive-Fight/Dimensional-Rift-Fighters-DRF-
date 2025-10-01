#include "CharacterCategory.h"
#include "CharacterBase.h"

namespace ArenaFighter {

CharacterCategoryManager& CharacterCategoryManager::GetInstance() {
    static CharacterCategoryManager instance;
    return instance;
}

CharacterCategoryManager::CharacterCategoryManager() {
    InitializeCategoryTraits();
}

void CharacterCategoryManager::InitializeCategoryTraits() {
    // System Category - Digital/Tech themed
    m_categoryTraits[CharacterCategory::System] = {
        "Digital technology and cyber warfare",
        "#00FF00", // Neon green
        "#0080FF", // Electric blue
        "digital_particles",
        "tech_grid",
        1.0f,   // health
        1.2f,   // mana (+20%)
        0.9f,   // defense (-10%)
        1.1f,   // speed (+10%)
        1.0f,   // power
        0.05f,  // +5% crit
        1.2f,   // +20% mana regen
        false,  // no stance
        false,  // no evolution
        true    // has transformation (digital forms)
    };
    
    // Gods & Heroes Category - Divine/Mythological themed
    m_categoryTraits[CharacterCategory::GodsHeroes] = {
        "Divine powers and legendary heroics",
        "#FFD700", // Gold
        "#FFFFFF", // White
        "divine_light",
        "holy_aura",
        1.2f,   // health (+20%)
        1.1f,   // mana (+10%)
        1.1f,   // defense (+10%)
        0.9f,   // speed (-10%)
        1.2f,   // power (+20%)
        0.0f,   // no crit bonus
        1.0f,   // normal mana regen
        true,   // has stance (divine forms)
        false,  // no evolution
        true    // has transformation
    };
    
    // Murim Category - Martial Arts themed
    m_categoryTraits[CharacterCategory::Murim] = {
        "Eastern martial arts and inner energy",
        "#8B4513", // Saddle brown
        "#FF6347", // Tomato red
        "chi_flow",
        "martial_spirit",
        1.1f,   // health (+10%)
        1.0f,   // mana
        1.0f,   // defense
        1.2f,   // speed (+20%)
        1.1f,   // power (+10%)
        0.1f,   // +10% crit
        1.1f,   // +10% mana regen
        true,   // has stance
        false,  // no evolution
        false   // no transformation
    };
    
    // Cultivation Category - Spiritual Energy themed
    m_categoryTraits[CharacterCategory::Cultivation] = {
        "Spiritual cultivation and immortal techniques",
        "#9370DB", // Medium purple
        "#00CED1", // Dark turquoise
        "spirit_essence",
        "cultivation_aura",
        1.0f,   // health
        1.5f,   // mana (+50%)
        0.8f,   // defense (-20%)
        1.0f,   // speed
        1.3f,   // power (+30%)
        0.0f,   // no crit bonus
        1.5f,   // +50% mana regen
        true,   // has stance (cultivation stages)
        true,   // has evolution (breakthrough)
        false   // no transformation
    };
    
    // Animal Category - Beast/Nature themed
    m_categoryTraits[CharacterCategory::Animal] = {
        "Primal instincts and natural ferocity",
        "#228B22", // Forest green
        "#8B4513", // Saddle brown
        "nature_particles",
        "beast_aura",
        1.3f,   // health (+30%)
        0.8f,   // mana (-20%)
        1.1f,   // defense (+10%)
        1.1f,   // speed (+10%)
        1.0f,   // power
        0.15f,  // +15% crit (predator instincts)
        0.8f,   // -20% mana regen
        false,  // no stance
        true,   // has evolution
        false   // no transformation
    };
    
    // Monsters Category - Dark/Monstrous themed
    m_categoryTraits[CharacterCategory::Monsters] = {
        "Monstrous power and dark evolution",
        "#4B0082", // Indigo
        "#8B0000", // Dark red
        "dark_essence",
        "monster_aura",
        1.5f,   // health (+50%)
        0.9f,   // mana (-10%)
        1.2f,   // defense (+20%)
        0.8f,   // speed (-20%)
        1.3f,   // power (+30%)
        0.0f,   // no crit bonus
        0.9f,   // -10% mana regen
        false,  // no stance
        true,   // has evolution
        true    // has transformation
    };
    
    // Chaos Category - Unpredictable/Reality-warping themed
    m_categoryTraits[CharacterCategory::Chaos] = {
        "Reality manipulation and chaotic energy",
        "#FF1493", // Deep pink
        "#4B0082", // Indigo
        "chaos_rift",
        "unstable_aura",
        1.0f,   // health (varies)
        1.3f,   // mana (+30%)
        0.9f,   // defense (-10%)
        1.0f,   // speed (varies)
        1.0f,   // power (varies)
        0.2f,   // +20% crit (chaos factor)
        1.3f,   // +30% mana regen
        true,   // has stance (chaos forms)
        false,  // no evolution
        true    // has transformation (chaos shifts)
    };
}

const CategoryTraits& CharacterCategoryManager::GetCategoryTraits(CharacterCategory category) const {
    auto it = m_categoryTraits.find(category);
    if (it != m_categoryTraits.end()) {
        return it->second;
    }
    
    // Return default traits if not found
    static CategoryTraits defaultTraits;
    return defaultTraits;
}

std::string CharacterCategoryManager::GetCategoryTheme(CharacterCategory category) const {
    const auto& traits = GetCategoryTraits(category);
    return traits.themeDescription;
}

std::string CharacterCategoryManager::GetCategoryDescription(CharacterCategory category) const {
    switch (category) {
        case CharacterCategory::System:
            return "Digital warriors harnessing technology and cyber warfare";
        case CharacterCategory::GodsHeroes:
            return "Divine beings and legendary heroes with mythological powers";
        case CharacterCategory::Murim:
            return "Martial artists mastering inner energy and combat techniques";
        case CharacterCategory::Cultivation:
            return "Immortal cultivators pursuing spiritual enlightenment";
        case CharacterCategory::Animal:
            return "Primal beasts and nature's champions";
        case CharacterCategory::Monsters:
            return "Dark creatures with monstrous strength and evolution";
        case CharacterCategory::Chaos:
            return "Unpredictable entities that warp reality itself";
        default:
            return "Unknown category";
    }
}

void CharacterCategoryManager::ApplyCategoryModifiers(CharacterCategory category,
                                                      float& health, float& mana,
                                                      float& defense, float& speed,
                                                      float& power) const {
    const auto& traits = GetCategoryTraits(category);
    
    health *= traits.healthModifier;
    mana *= traits.manaModifier;
    defense *= traits.defenseModifier;
    speed *= traits.speedModifier;
    power *= traits.powerModifier;
}

void CharacterCategoryManager::ApplyStatModeModifiers(StatMode mode,
                                                      float& health, float& mana,
                                                      float& defense, float& speed,
                                                      float& power) const {
    switch (mode) {
        case StatMode::Attack:
            power *= 1.2f;      // +20% power
            defense *= 0.9f;    // -10% defense
            break;
            
        case StatMode::Defense:
            defense *= 1.2f;    // +20% defense
            speed *= 0.9f;      // -10% speed
            break;
            
        case StatMode::Special:
            mana *= 1.2f;       // +20% mana
            // Mana regen bonus applied separately
            break;
            
        case StatMode::Hybrid:
            // Balanced - no modifications
            break;
            
        case StatMode::Custom:
            // Character-specific, handled elsewhere
            break;
    }
}

} // namespace ArenaFighter