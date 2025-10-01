#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include "../Characters/CharacterFactory.h"
#include "../GameModes/GameModeManager.h"
#include "../Characters/CharacterBase.h"

namespace ArenaFighter {

/**
 * @brief UI Panel types aligned with DFR
 */
enum class DFRPanelType {
    MainMenu,
    ModeSelection,
    CharacterSelection,
    LoadoutSetup,      // Select gears and stat distribution
    Rankings,
    Shop,
    InGameHUD,
    PostMatch
};

/**
 * @brief Button structure for UI
 */
struct DFRUIButton {
    std::string text;
    std::string tooltip;
    float x, y, width, height;
    bool isHovered;
    bool isEnabled;
    bool isSelected;
    std::function<void()> onClick;
};

/**
 * @brief Main Menu UI adapted for DFR
 */
class DFRMainMenuUI {
private:
    std::vector<DFRUIButton> m_buttons;
    std::function<void(const std::string&)> m_onMenuAction;

public:
    DFRMainMenuUI() {
        InitializeButtons();
    }

    void InitializeButtons() {
        m_buttons.clear();
        
        // Play Button
        m_buttons.push_back({
            "PLAY", "Enter combat modes",
            860, 400, 200, 60,
            false, true, false,
            [this]() { 
                if (m_onMenuAction) m_onMenuAction("ModeSelection"); 
            }
        });
        
        // Shop Button
        m_buttons.push_back({
            "SHOP", "Buy equipment and enhancements",
            860, 480, 200, 60,
            false, true, false,
            [this]() { 
                if (m_onMenuAction) m_onMenuAction("Shop"); 
            }
        });
        
        // Rankings Button
        m_buttons.push_back({
            "RANKINGS", "View weekly character rankings",
            860, 560, 200, 60,
            false, true, false,
            [this]() { 
                if (m_onMenuAction) m_onMenuAction("Rankings"); 
            }
        });
        
        // Training Button
        m_buttons.push_back({
            "TRAINING", "Practice combos and skills",
            860, 640, 200, 60,
            false, true, false,
            [this]() { 
                if (m_onMenuAction) m_onMenuAction("Training"); 
            }
        });
        
        // Exit Button
        m_buttons.push_back({
            "EXIT", "Exit game",
            860, 720, 200, 60,
            false, true, false,
            [this]() { 
                if (m_onMenuAction) m_onMenuAction("Exit"); 
            }
        });
    }
    
    void HandleMouseMove(int x, int y);
    void HandleMouseClick(int x, int y);
    void SetActionCallback(std::function<void(const std::string&)> callback) {
        m_onMenuAction = callback;
    }
    const std::vector<DFRUIButton>& GetButtons() const { return m_buttons; }
};

/**
 * @brief Mode Selection UI for DFR
 */
class DFRModeSelectionUI {
private:
    std::vector<DFRUIButton> m_modeButtons;
    std::function<void(GameModeType)> m_onModeSelected;
    int m_currentWeek;
    
public:
    DFRModeSelectionUI() : m_currentWeek(1) {
        InitializeButtons();
    }
    
    void InitializeButtons() {
        m_modeButtons.clear();
        
        // Ranked 1v1 (with weekly slot variation)
        m_modeButtons.push_back({
            GetRankedModeText(), GetRankedTooltip(),
            200, 200, 300, 80,
            false, true, false,
            [this]() { 
                if (m_onModeSelected) m_onModeSelected(GameModeType::Versus); 
            }
        });
        
        // Death Match
        m_modeButtons.push_back({
            "DEATH MATCH", "8 Player FFA - Last one standing",
            550, 200, 300, 80,
            false, true, false,
            [this]() { 
                if (m_onModeSelected) m_onModeSelected(GameModeType::DeathMatch); 
            }
        });
        
        // Beast Mode
        m_modeButtons.push_back({
            "BEAST MODE", "1 Giant player vs 7 regular players",
            900, 200, 300, 80,
            false, true, false,
            [this]() { 
                if (m_onModeSelected) m_onModeSelected(GameModeType::BeastMode); 
            }
        });
        
        // For Glory
        m_modeButtons.push_back({
            "FOR GLORY", "3v3 Team Battle",
            200, 320, 300, 80,
            false, true, false,
            [this]() { 
                if (m_onModeSelected) m_onModeSelected(GameModeType::ForGlory); 
            }
        });
        
        // Dimensional Rift
        m_modeButtons.push_back({
            "DIMENSIONAL RIFT", "PvE Dungeon - Fight corrupted heroes",
            550, 320, 300, 80,
            false, true, false,
            [this]() { 
                if (m_onModeSelected) m_onModeSelected(GameModeType::DimensionalRift); 
            }
        });
        
        // Tournament
        m_modeButtons.push_back({
            "TOURNAMENT", "32-player bracket tournament",
            900, 320, 300, 80,
            false, true, false,
            [this]() { 
                if (m_onModeSelected) m_onModeSelected(GameModeType::Tournament); 
            }
        });
        
        // Training Mode
        m_modeButtons.push_back({
            "TRAINING", "Practice mode with infinite resources",
            550, 440, 300, 80,
            false, true, false,
            [this]() { 
                if (m_onModeSelected) m_onModeSelected(GameModeType::Training); 
            }
        });
        
        // Back Button
        m_modeButtons.push_back({
            "BACK", "",
            100, 900, 150, 50,
            false, true, false,
            [this]() { 
                if (m_onModeSelected) m_onModeSelected(GameModeType::None); 
            }
        });
    }
    
    std::string GetRankedModeText() const {
        int slots = (m_currentWeek <= 2) ? 1 : 3;
        return "RANKED 1v1\nWeek " + std::to_string(m_currentWeek) + " - " + 
               std::to_string(slots) + " Hero" + (slots > 1 ? "es" : "");
    }
    
    std::string GetRankedTooltip() const {
        return (m_currentWeek <= 2) ? 
            "Single character duel" : 
            "3 character rotation battle";
    }
    
    void SetCurrentWeek(int week);
    void HandleMouseMove(int x, int y);
    void HandleMouseClick(int x, int y);
    void SetModeSelectedCallback(std::function<void(GameModeType)> callback) {
        m_onModeSelected = callback;
    }
    const std::vector<DFRUIButton>& GetButtons() const { return m_modeButtons; }
};

/**
 * @brief Character Selection UI for DFR
 */
class DFRCharacterSelectionUI {
private:
    std::vector<std::unique_ptr<CharacterBase>> m_availableCharacters;
    std::vector<std::string> m_selectedCharacterNames;
    int m_maxSlots;
    CharacterCategory m_selectedCategory;
    
    std::vector<DFRUIButton> m_categoryButtons;
    std::vector<DFRUIButton> m_characterButtons;
    std::vector<DFRUIButton> m_actionButtons;
    
    std::function<void(const std::vector<std::string>&)> m_onConfirm;
    std::function<void()> m_onCancel;
    
public:
    DFRCharacterSelectionUI(int slots);
    
    void InitializeCategoryButtons();
    void UpdateCharacterList();
    void InitializeActionButtons();
    void SelectCharacter(const std::string& name);
    void SetMaxSlots(int slots);
    
    void HandleMouseMove(int x, int y);
    void HandleMouseClick(int x, int y);
    
    void SetConfirmCallback(std::function<void(const std::vector<std::string>&)> callback) {
        m_onConfirm = callback;
    }
    void SetCancelCallback(std::function<void()> callback) {
        m_onCancel = callback;
    }
    
    const std::vector<std::string>& GetSelectedCharacters() const {
        return m_selectedCharacterNames;
    }
    std::string GetSelectionStatus() const;
};

/**
 * @brief Loadout Setup UI for DFR
 * 
 * CRITICAL: This UI must clearly show that:
 * - Special moves (S+Direction) use MANA ONLY
 * - Gear skills (AS, AD, ASD, SD) have BOTH mana AND cooldowns
 */
class DFRLoadoutSetupUI {
private:
    CharacterBase* m_character;
    std::vector<DFRUIButton> m_statButtons;
    std::vector<DFRUIButton> m_gearButtons;
    std::vector<DFRUIButton> m_infoButtons;
    
    std::function<void(StatMode)> m_onStatModeSelected;
    std::function<void()> m_onConfirm;
    
public:
    DFRLoadoutSetupUI(CharacterBase* character);
    
    void InitializeStatButtons();
    void InitializeGearButtons();
    void InitializeInfoButtons();
    
    void HandleMouseMove(int x, int y);
    void HandleMouseClick(int x, int y);
    
    void SetStatModeCallback(std::function<void(StatMode)> callback) {
        m_onStatModeSelected = callback;
    }
    void SetConfirmCallback(std::function<void()> callback) {
        m_onConfirm = callback;
    }
};

/**
 * @brief Adapter to make provided UI work with DFR
 */
class UISystemAdapter {
public:
    /**
     * @brief Fix stat mode descriptions to clarify cooldown effects
     */
    class StatModeDescriptionFixer {
    public:
        static std::string GetAttackModeDesc() {
            return "ATTACK MODE\n+50% Attack Power\n-20% Defense/Skill";
        }
        
        static std::string GetDefenseModeDesc() {
            return "DEFENSE MODE\n+50% Defense\n-20% Attack/Skill";
        }
        
        static std::string GetSpecialModeDesc() {
            return "SPECIAL MODE\n+50% Skill Power\n+10% Mana Regen\n20% Faster GEAR Cooldowns";
            // Note: Emphasizes that cooldown reduction ONLY affects gear skills
        }
        
        static std::string GetHybridModeDesc() {
            return "HYBRID MODE ★\n+25% Attack & Skill\n+5% Mana Regen\n10% Faster GEAR Cooldowns";
            // Note: Clear that cooldown reduction is for gear skills only
        }
        
        static std::string GetCustomModeDesc() {
            return "CUSTOM MODE\n(Manual Allocation)\nComing Soon";
        }
    };
    
    /**
     * @brief Create tooltip explaining skill types
     */
    static std::string GetSkillSystemTooltip() {
        return "SKILL SYSTEM:\n"
               "• Special Moves (S+↑↓←→): Mana cost only, NO cooldowns\n"
               "• Gear Skills (AS/AD/SD/ASD): Mana cost AND cooldowns\n"
               "• Blocking (Hold S 1s): Prevents special moves";
    }
};

/**
 * @brief In-game HUD specific to DFR
 */
class DFRCombatHUD {
private:
    // Player info display
    struct CharacterDisplay {
        std::string name;
        float healthPercent;
        float manaPercent;
        int currentStance; // 0 = Light, 1 = Dark for Murim
        std::vector<float> gearCooldowns; // 8 skills (4 gears x 2)
        bool isBlocking;
        float blockHoldTime;
    };
    
    CharacterDisplay m_player;
    CharacterDisplay m_enemy;
    
    // Skill indicators
    std::vector<DFRUIButton> m_specialMoveIndicators;
    std::vector<DFRUIButton> m_gearSkillIndicators;
    
public:
    DFRCombatHUD();
    
    void Update(CharacterBase* player, CharacterBase* enemy, float deltaTime);
    void RenderHealthBars();
    void RenderManaBars();
    void RenderSkillIndicators();
    void RenderStanceDisplay();
    void RenderBlockIndicator();
    
    // Show clear distinction between skill types
    void ShowSpecialMoveAvailable(InputDirection direction, bool available);
    void ShowGearSkillStatus(int skillIndex, float cooldownRemaining, bool canAfford);
};

} // namespace ArenaFighter