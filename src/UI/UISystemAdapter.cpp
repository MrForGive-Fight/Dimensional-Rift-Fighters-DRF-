#include "UISystemAdapter.h"
#include "../Shop/ItemManager.h"
#include "../Characters/CategoryManager.h"
#include <algorithm>

namespace ArenaFighter {

// DFRMainMenuUI Implementation

void DFRMainMenuUI::HandleMouseMove(int x, int y) {
    for (auto& button : m_buttons) {
        button.isHovered = (x >= button.x && x <= button.x + button.width &&
                          y >= button.y && y <= button.y + button.height);
    }
}

void DFRMainMenuUI::HandleMouseClick(int x, int y) {
    for (auto& button : m_buttons) {
        if (button.isHovered && button.isEnabled && button.onClick) {
            button.onClick();
        }
    }
}

// DFRModeSelectionUI Implementation

void DFRModeSelectionUI::SetCurrentWeek(int week) {
    m_currentWeek = week;
    InitializeButtons(); // Refresh buttons to update text
}

void DFRModeSelectionUI::HandleMouseMove(int x, int y) {
    for (auto& button : m_modeButtons) {
        button.isHovered = (x >= button.x && x <= button.x + button.width &&
                          y >= button.y && y <= button.y + button.height);
    }
}

void DFRModeSelectionUI::HandleMouseClick(int x, int y) {
    for (auto& button : m_modeButtons) {
        if (button.isHovered && button.isEnabled && button.onClick) {
            button.onClick();
        }
    }
}

// DFRCharacterSelectionUI Implementation

DFRCharacterSelectionUI::DFRCharacterSelectionUI(int slots) 
    : m_maxSlots(slots)
    , m_selectedCategory(CharacterCategory::Murim) {
    
    InitializeCategoryButtons();
    UpdateCharacterList();
    InitializeActionButtons();
}

void DFRCharacterSelectionUI::InitializeCategoryButtons() {
    m_categoryButtons.clear();
    
    float startX = 100.0f;
    float y = 50.0f;
    float spacing = 150.0f;
    
    // All DFR categories
    std::vector<std::pair<CharacterCategory, std::string>> categories = {
        {CharacterCategory::System, "System"},
        {CharacterCategory::GodsHeroes, "Gods/Heroes"},
        {CharacterCategory::Murim, "Murim"},
        {CharacterCategory::Cultivation, "Cultivation"},
        {CharacterCategory::Animal, "Animal"},
        {CharacterCategory::Monsters, "Monsters"},
        {CharacterCategory::Chaos, "Chaos"}
    };
    
    for (const auto& [category, name] : categories) {
        m_categoryButtons.push_back({
            name, CharacterCategoryManager::GetInstance().GetCategoryTraits(category).description,
            startX, y, 140, 40,
            false, true, category == m_selectedCategory,
            [this, category]() { 
                m_selectedCategory = category;
                // Update selection state
                for (auto& btn : m_categoryButtons) {
                    btn.isSelected = false;
                }
                UpdateCharacterList();
            }
        });
        startX += spacing;
    }
}

void DFRCharacterSelectionUI::UpdateCharacterList() {
    m_characterButtons.clear();
    
    // Get all available characters for the selected category
    auto& factory = CharacterFactory::GetInstance();
    auto allCharacters = factory.GetAvailableCharacters();
    
    // Filter by category
    m_availableCharacters.clear();
    for (const auto& charName : allCharacters) {
        auto character = factory.CreateCharacterByName(charName);
        if (character && character->GetCategory() == m_selectedCategory) {
            m_availableCharacters.push_back(std::move(character));
        }
    }
    
    // Create buttons
    float startX = 100.0f;
    float startY = 150.0f;
    float spacing = 180.0f;
    int col = 0;
    
    for (const auto& character : m_availableCharacters) {
        float x = startX + (col % 5) * spacing;
        float y = startY + (col / 5) * 220.0f;
        
        std::string tooltip = "Tier: " + std::string(1, character->GetTier()) + "\n";
        tooltip += "HP: " + std::to_string(static_cast<int>(character->GetMaxHealth())) + "\n";
        tooltip += "Mana: " + std::to_string(static_cast<int>(character->GetMaxMana()));
        
        bool isSelected = std::find(m_selectedCharacterNames.begin(), 
                                  m_selectedCharacterNames.end(),
                                  character->GetName()) != m_selectedCharacterNames.end();
        
        m_characterButtons.push_back({
            character->GetName(), tooltip,
            x, y, 160, 200,
            false, true, isSelected,
            [this, name = character->GetName()]() { 
                SelectCharacter(name);
            }
        });
        
        col++;
    }
}

void DFRCharacterSelectionUI::InitializeActionButtons() {
    m_actionButtons.clear();
    
    // Ready Button
    m_actionButtons.push_back({
        "READY", "Confirm character selection",
        1620, 900, 200, 60,
        false, false, false,
        [this]() { 
            if (m_onConfirm && m_selectedCharacterNames.size() == m_maxSlots) {
                m_onConfirm(m_selectedCharacterNames);
            }
        }
    });
    
    // Back Button
    m_actionButtons.push_back({
        "BACK", "Return to mode selection",
        100, 900, 150, 50,
        false, true, false,
        [this]() { 
            if (m_onCancel) m_onCancel();
        }
    });
}

void DFRCharacterSelectionUI::SelectCharacter(const std::string& name) {
    auto it = std::find(m_selectedCharacterNames.begin(), 
                       m_selectedCharacterNames.end(), name);
    
    if (it != m_selectedCharacterNames.end()) {
        // Deselect
        m_selectedCharacterNames.erase(it);
    } else {
        // Select if not at max
        if (m_selectedCharacterNames.size() < m_maxSlots) {
            m_selectedCharacterNames.push_back(name);
        }
    }
    
    // Update button states
    for (auto& btn : m_characterButtons) {
        if (btn.text == name) {
            btn.isSelected = (it == m_selectedCharacterNames.end() && 
                            m_selectedCharacterNames.size() <= m_maxSlots);
        }
    }
    
    // Update Ready button
    m_actionButtons[0].isEnabled = (m_selectedCharacterNames.size() == m_maxSlots);
}

void DFRCharacterSelectionUI::SetMaxSlots(int slots) {
    m_maxSlots = slots;
    m_selectedCharacterNames.clear();
    m_actionButtons[0].isEnabled = false;
    UpdateCharacterList(); // Refresh selection states
}

void DFRCharacterSelectionUI::HandleMouseMove(int x, int y) {
    for (auto& button : m_categoryButtons) {
        button.isHovered = (x >= button.x && x <= button.x + button.width &&
                          y >= button.y && y <= button.y + button.height);
    }
    for (auto& button : m_characterButtons) {
        button.isHovered = (x >= button.x && x <= button.x + button.width &&
                          y >= button.y && y <= button.y + button.height);
    }
    for (auto& button : m_actionButtons) {
        button.isHovered = (x >= button.x && x <= button.x + button.width &&
                          y >= button.y && y <= button.y + button.height);
    }
}

void DFRCharacterSelectionUI::HandleMouseClick(int x, int y) {
    for (auto& button : m_categoryButtons) {
        if (button.isHovered && button.onClick) {
            button.onClick();
        }
    }
    for (auto& button : m_characterButtons) {
        if (button.isHovered && button.onClick) {
            button.onClick();
        }
    }
    for (auto& button : m_actionButtons) {
        if (button.isHovered && button.isEnabled && button.onClick) {
            button.onClick();
        }
    }
}

std::string DFRCharacterSelectionUI::GetSelectionStatus() const {
    return std::to_string(m_selectedCharacterNames.size()) + " / " + 
           std::to_string(m_maxSlots) + " Selected";
}

// DFRLoadoutSetupUI Implementation

DFRLoadoutSetupUI::DFRLoadoutSetupUI(CharacterBase* character) 
    : m_character(character) {
    
    InitializeStatButtons();
    InitializeGearButtons();
    InitializeInfoButtons();
}

void DFRLoadoutSetupUI::InitializeStatButtons() {
    m_statButtons.clear();
    
    float centerX = 960.0f;
    float startY = 300.0f;
    float spacing = 100.0f;
    
    // Attack Mode
    m_statButtons.push_back({
        "ATTACK MODE", UISystemAdapter::StatModeDescriptionFixer::GetAttackModeDesc(),
        centerX - 250, startY, 220, 100,
        false, true, false,
        [this]() { 
            if (m_onStatModeSelected) m_onStatModeSelected(StatMode::Attack);
        }
    });
    
    // Defense Mode
    m_statButtons.push_back({
        "DEFENSE MODE", UISystemAdapter::StatModeDescriptionFixer::GetDefenseModeDesc(),
        centerX + 30, startY, 220, 100,
        false, true, false,
        [this]() { 
            if (m_onStatModeSelected) m_onStatModeSelected(StatMode::Defense);
        }
    });
    
    // Special Mode (with corrected description)
    m_statButtons.push_back({
        "SPECIAL MODE", UISystemAdapter::StatModeDescriptionFixer::GetSpecialModeDesc(),
        centerX - 250, startY + spacing, 220, 100,
        false, true, false,
        [this]() { 
            if (m_onStatModeSelected) m_onStatModeSelected(StatMode::Special);
        }
    });
    
    // Hybrid Mode (with corrected description)
    m_statButtons.push_back({
        "HYBRID MODE ★", UISystemAdapter::StatModeDescriptionFixer::GetHybridModeDesc(),
        centerX + 30, startY + spacing, 220, 100,
        false, true, true, // Selected by default
        [this]() { 
            if (m_onStatModeSelected) m_onStatModeSelected(StatMode::Hybrid);
        }
    });
    
    // Custom Mode
    m_statButtons.push_back({
        "CUSTOM MODE", UISystemAdapter::StatModeDescriptionFixer::GetCustomModeDesc(),
        centerX - 110, startY + spacing * 2, 220, 100,
        false, false, false, // Disabled
        nullptr
    });
}

void DFRLoadoutSetupUI::InitializeGearButtons() {
    m_gearButtons.clear();
    
    if (!m_character) return;
    
    float startX = 100.0f;
    float startY = 600.0f;
    
    // Show gear skills with clear indication of cooldowns
    for (int i = 0; i < 4; ++i) {
        int skill1 = i * 2;
        int skill2 = i * 2 + 1;
        
        const auto& gearSkill1 = m_character->GetGearSkills()[skill1];
        const auto& gearSkill2 = m_character->GetGearSkills()[skill2];
        
        std::string gearName = "Gear " + std::to_string(i + 1);
        std::string tooltip = gearSkill1.name + " (Mana: " + 
                            std::to_string(static_cast<int>(gearSkill1.manaCost)) +
                            ", CD: " + std::to_string(static_cast<int>(gearSkill1.cooldown)) + "s)\n" +
                            gearSkill2.name + " (Mana: " + 
                            std::to_string(static_cast<int>(gearSkill2.manaCost)) +
                            ", CD: " + std::to_string(static_cast<int>(gearSkill2.cooldown)) + "s)";
        
        m_gearButtons.push_back({
            gearName, tooltip,
            startX + i * 200, startY, 180, 80,
            false, true, i == m_character->GetCurrentGear(),
            [this, i]() {
                if (m_character) {
                    m_character->SwitchGear(i);
                    // Update selection state
                    for (int j = 0; j < m_gearButtons.size(); ++j) {
                        m_gearButtons[j].isSelected = (j == i);
                    }
                }
            }
        });
    }
}

void DFRLoadoutSetupUI::InitializeInfoButtons() {
    m_infoButtons.clear();
    
    // Info panel showing skill system
    m_infoButtons.push_back({
        "SKILL INFO", UISystemAdapter::GetSkillSystemTooltip(),
        1400, 200, 400, 300,
        false, false, false,
        nullptr
    });
    
    // Confirm button
    m_infoButtons.push_back({
        "READY", "Confirm loadout and start match",
        1620, 900, 200, 60,
        false, true, false,
        [this]() {
            if (m_onConfirm) m_onConfirm();
        }
    });
}

void DFRLoadoutSetupUI::HandleMouseMove(int x, int y) {
    for (auto& button : m_statButtons) {
        button.isHovered = (x >= button.x && x <= button.x + button.width &&
                          y >= button.y && y <= button.y + button.height);
    }
    for (auto& button : m_gearButtons) {
        button.isHovered = (x >= button.x && x <= button.x + button.width &&
                          y >= button.y && y <= button.y + button.height);
    }
    for (auto& button : m_infoButtons) {
        button.isHovered = (x >= button.x && x <= button.x + button.width &&
                          y >= button.y && y <= button.y + button.height);
    }
}

void DFRLoadoutSetupUI::HandleMouseClick(int x, int y) {
    // Stat mode selection
    for (auto& button : m_statButtons) {
        if (button.isHovered && button.isEnabled && button.onClick) {
            // Clear other selections
            for (auto& btn : m_statButtons) {
                btn.isSelected = false;
            }
            button.isSelected = true;
            button.onClick();
        }
    }
    
    // Gear selection
    for (auto& button : m_gearButtons) {
        if (button.isHovered && button.isEnabled && button.onClick) {
            button.onClick();
        }
    }
    
    // Info buttons
    for (auto& button : m_infoButtons) {
        if (button.isHovered && button.isEnabled && button.onClick) {
            button.onClick();
        }
    }
}

// DFRCombatHUD Implementation

DFRCombatHUD::DFRCombatHUD() {
    // Initialize special move indicators (S+Direction)
    m_specialMoveIndicators.resize(4);
    m_specialMoveIndicators[0] = {"S+↑", "Special Up (Mana only)", 100, 800, 60, 60};
    m_specialMoveIndicators[1] = {"S+↓", "Special Down (Mana only)", 100, 870, 60, 60};
    m_specialMoveIndicators[2] = {"S+←", "Special Left (Mana only)", 30, 835, 60, 60};
    m_specialMoveIndicators[3] = {"S+→", "Special Right (Mana only)", 170, 835, 60, 60};
    
    // Initialize gear skill indicators
    m_gearSkillIndicators.resize(4);
    m_gearSkillIndicators[0] = {"AS", "Gear Skill 1 (Mana + CD)", 300, 800, 80, 60};
    m_gearSkillIndicators[1] = {"AD", "Gear Skill 2 (Mana + CD)", 390, 800, 80, 60};
    m_gearSkillIndicators[2] = {"SD", "Gear Skill 3 (Mana + CD)", 480, 800, 80, 60};
    m_gearSkillIndicators[3] = {"ASD", "Gear Skill 4 (Mana + CD)", 570, 800, 80, 60};
}

void DFRCombatHUD::Update(CharacterBase* player, CharacterBase* enemy, float deltaTime) {
    if (!player || !enemy) return;
    
    // Update player display
    m_player.name = player->GetName();
    m_player.healthPercent = player->GetCurrentHealth() / player->GetMaxHealth();
    m_player.manaPercent = player->GetCurrentMana() / player->GetMaxMana();
    m_player.currentStance = player->GetCurrentStance();
    m_player.isBlocking = player->IsBlocking();
    m_player.blockHoldTime = player->GetBlockDuration();
    
    // Update gear cooldowns
    m_player.gearCooldowns.clear();
    for (int i = 0; i < 8; ++i) {
        m_player.gearCooldowns.push_back(player->GetGearSkillCooldownRemaining(i));
    }
    
    // Update enemy display
    m_enemy.name = enemy->GetName();
    m_enemy.healthPercent = enemy->GetCurrentHealth() / enemy->GetMaxHealth();
    m_enemy.manaPercent = enemy->GetCurrentMana() / enemy->GetMaxMana();
    m_enemy.currentStance = enemy->GetCurrentStance();
    m_enemy.isBlocking = enemy->IsBlocking();
}

void DFRCombatHUD::ShowSpecialMoveAvailable(InputDirection direction, bool available) {
    int index = static_cast<int>(direction);
    if (index >= 0 && index < 4) {
        m_specialMoveIndicators[index].isEnabled = available;
    }
}

void DFRCombatHUD::ShowGearSkillStatus(int skillIndex, float cooldownRemaining, bool canAfford) {
    int buttonIndex = skillIndex / 2; // Map skill to button
    if (buttonIndex >= 0 && buttonIndex < 4) {
        auto& button = m_gearSkillIndicators[buttonIndex];
        button.isEnabled = (cooldownRemaining <= 0.0f && canAfford);
        
        if (cooldownRemaining > 0.0f) {
            button.text = button.text.substr(0, 3) + " (" + 
                         std::to_string(static_cast<int>(cooldownRemaining)) + "s)";
        }
    }
}

} // namespace ArenaFighter