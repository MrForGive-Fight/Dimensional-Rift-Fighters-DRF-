#include "CharacterUnlockScreen.h"
#include <imgui.h>

namespace ArenaFighter {

// CharacterCustomizationPanel Implementation
CharacterCustomizationPanel::CharacterCustomizationPanel()
    : UIPanel("Customization", ImVec2(0, 0), ImVec2(800, 600)),
      m_characterPreview(nullptr),
      m_selectedCategory(0) {
    
    SetBackgroundColor(UITheme::PANEL_BG_LOST);
    SetBorderColor(UITheme::BORDER_BLUE);
}

void CharacterCustomizationPanel::SetCharacter(Character3DPreview* preview) {
    m_characterPreview = preview;
}

void CharacterCustomizationPanel::Initialize() {
    CreatePreviewOptions();
    CreateDecorationGrid();
}

void CharacterCustomizationPanel::Render() {
    if (!IsVisible()) return;
    
    Begin();
    
    // Left panel - Preview options
    ImGui::BeginChild("PreviewOptions", ImVec2(250, -1), true);
    ImGui::Text("Preview Actions");
    ImGui::Separator();
    
    for (auto& option : m_previewOptions) {
        if (option.button->Render(ImGui::GetCursorScreenPos())) {
            OnPreviewSelected(option.animationName);
        }
        ImGui::Dummy(ImVec2(0, 5));
    }
    ImGui::EndChild();
    
    ImGui::SameLine();
    
    // Center - Character preview
    ImGui::BeginChild("CharacterView", ImVec2(300, -1), true);
    if (m_characterPreview) {
        ImVec2 previewPos = ImGui::GetCursorScreenPos();
        m_characterPreview->Render(previewPos, ImVec2(280, 400));
    }
    ImGui::EndChild();
    
    ImGui::SameLine();
    
    // Right panel - Decoration items
    ImGui::BeginChild("Decorations", ImVec2(-1, -1), true);
    ImGui::Text("Decorations");
    ImGui::Separator();
    
    // Category tabs
    for (size_t i = 0; i < m_decorationCategories.size(); ++i) {
        if (i > 0) ImGui::SameLine();
        
        bool selected = (i == m_selectedCategory);
        if (selected) {
            ImGui::PushStyleColor(ImGuiCol_Button, UITheme::BORDER_ORANGE);
        }
        
        if (ImGui::Button(m_decorationCategories[i].name.c_str(), ImVec2(80, 30))) {
            m_selectedCategory = static_cast<int>(i);
        }
        
        if (selected) {
            ImGui::PopStyleColor();
        }
    }
    
    ImGui::Separator();
    
    // Decoration grid
    if (m_decorationGrid) {
        m_decorationGrid->Render();
    }
    
    // Random button
    ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 140, ImGui::GetWindowHeight() - 50));
    m_randomDecoButton->Render(ImGui::GetCursorScreenPos());
    
    ImGui::EndChild();
    
    End();
}

void CharacterCustomizationPanel::CreatePreviewOptions() {
    m_previewOptions = {
        {"Dwarf Preview", "dwarf_stance"},
        {"Action: Salute Preview", "salute"},
        {"Action: I'm perfect Preview", "perfect"},
        {"Action: Hero 3 Preview", "hero3"},
        {"Action: Dance 12 Preview", "dance12"}
    };
    
    for (auto& option : m_previewOptions) {
        option.button = std::make_unique<GameButton>(option.name, ImVec2(230, 35));
        option.button->SetStyle(ButtonStyle::Secondary);
    }
}

void CharacterCustomizationPanel::CreateDecorationGrid() {
    m_decorationCategories = {
        {"Hair", {}},
        {"Head", {}},
        {"Face", {}},
        {"Outfit", {}}
    };
    
    // TODO: Load actual decoration items
    
    m_decorationGrid = std::make_unique<GridLayout>(3, 6);
    m_decorationGrid->SetCellSize(60.0f);
    m_decorationGrid->SetSpacing(5.0f);
    
    m_randomDecoButton = std::make_unique<GameButton>("Random Deco", ImVec2(120, 35));
    m_randomDecoButton->SetStyle(ButtonStyle::Info);
    m_randomDecoButton->SetOnClick([this]() {
        // TODO: Randomize decorations
    });
}

void CharacterCustomizationPanel::OnPreviewSelected(const std::string& animationName) {
    if (m_characterPreview) {
        m_characterPreview->PlayAnimation(animationName);
    }
}

void CharacterCustomizationPanel::OnDecorationSelected(int index) {
    // TODO: Apply decoration to character
}

// RewardDisplayPanel Implementation
RewardDisplayPanel::RewardDisplayPanel()
    : UIPanel("Rewards", ImVec2(0, 0), ImVec2(800, 600)),
      m_animationTimer(0.0f),
      m_isAnimating(false) {
    
    SetBackgroundColor({0, 0, 0, 0}); // Transparent for custom rendering
    SetFlags(PanelFlags::NoBackground);
}

void RewardDisplayPanel::SetHeroReward(const std::string& heroName, bool isPermanent) {
    Reward reward;
    reward.type = Reward::HERO;
    reward.name = heroName + (isPermanent ? " (Permanent)" : " (7 Days)");
    reward.value = 0;
    reward.icon = nullptr;
    reward.revealDelay = 0.0f;
    reward.revealProgress = 0.0f;
    m_rewards.push_back(reward);
}

void RewardDisplayPanel::SetExperienceReward(int exp) {
    if (exp > 0) {
        Reward reward;
        reward.type = Reward::EXPERIENCE;
        reward.name = "Experience";
        reward.value = exp;
        reward.icon = nullptr;
        reward.revealDelay = 0.3f;
        reward.revealProgress = 0.0f;
        m_rewards.push_back(reward);
    }
}

void RewardDisplayPanel::SetGoldReward(int gold) {
    if (gold > 0) {
        Reward reward;
        reward.type = Reward::GOLD;
        reward.name = "Pesos";
        reward.value = gold;
        reward.icon = nullptr;
        reward.revealDelay = 0.6f;
        reward.revealProgress = 0.0f;
        m_rewards.push_back(reward);
    }
}

void RewardDisplayPanel::AddItemReward(const std::string& itemName, void* icon) {
    Reward reward;
    reward.type = Reward::ITEM;
    reward.name = itemName;
    reward.value = 0;
    reward.icon = icon;
    reward.revealDelay = 0.9f + m_rewards.size() * 0.2f;
    reward.revealProgress = 0.0f;
    m_rewards.push_back(reward);
}

void RewardDisplayPanel::StartRevealAnimation() {
    m_isAnimating = true;
    m_animationTimer = 0.0f;
}

void RewardDisplayPanel::Render() {
    if (!IsVisible()) return;
    
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    // Update animation
    if (m_isAnimating) {
        m_animationTimer += ImGui::GetIO().DeltaTime;
        
        for (auto& reward : m_rewards) {
            if (m_animationTimer >= reward.revealDelay) {
                reward.revealProgress = std::min(1.0f, 
                    (m_animationTimer - reward.revealDelay) / 0.5f);
            }
        }
    }
    
    // Render shield frame
    RenderShieldFrame();
    
    // Render rewards
    ImVec2 rewardPos = ImVec2(m_position.x + m_size.x * 0.5f - 200, 
                             m_position.y + 150);
    
    for (const auto& reward : m_rewards) {
        if (reward.revealProgress > 0.0f) {
            RenderReward(reward, rewardPos);
            rewardPos.y += 80;
        }
    }
    
    // Render "Receive Reward" button
    if (m_animationTimer > 2.0f) {
        GameButton receiveBtn("Receive Reward", ImVec2(200, 60));
        receiveBtn.SetStyle(ButtonStyle::Primary);
        receiveBtn.EnableGlow(true);
        receiveBtn.Render(ImVec2(m_position.x + m_size.x * 0.5f - 100, 
                                m_position.y + m_size.y - 120));
    }
}

void RewardDisplayPanel::RenderShieldFrame() {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    ImVec2 center = ImVec2(m_position.x + m_size.x * 0.5f, 
                          m_position.y + m_size.y * 0.5f);
    
    // Shield shape background
    ImVec2 shieldSize = ImVec2(600, 500);
    ImVec2 topLeft = ImVec2(center.x - shieldSize.x * 0.5f, 
                           center.y - shieldSize.y * 0.5f);
    
    // Draw shield-shaped background
    std::vector<ImVec2> shieldPoints = {
        ImVec2(center.x, topLeft.y),
        ImVec2(topLeft.x + 100, topLeft.y + 50),
        ImVec2(topLeft.x, topLeft.y + 150),
        ImVec2(topLeft.x, center.y + 100),
        ImVec2(center.x, topLeft.y + shieldSize.y),
        ImVec2(topLeft.x + shieldSize.x, center.y + 100),
        ImVec2(topLeft.x + shieldSize.x, topLeft.y + 150),
        ImVec2(topLeft.x + shieldSize.x - 100, topLeft.y + 50)
    };
    
    // Background fill
    drawList->AddConvexPolyFilled(shieldPoints.data(), 
                                 static_cast<int>(shieldPoints.size()), 
                                 IM_COL32(20, 30, 50, 230));
    
    // Border
    drawList->AddPolyline(shieldPoints.data(), 
                         static_cast<int>(shieldPoints.size()), 
                         UITheme::BORDER_GOLD.ToU32(), 
                         ImDrawFlags_Closed, 3.0f);
    
    // Battle axes decoration
    RenderBattleAxes(topLeft, shieldSize);
}

void RewardDisplayPanel::RenderReward(const Reward& reward, ImVec2 position) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    float scale = reward.revealProgress;
    float alpha = reward.revealProgress;
    
    // Icon or placeholder
    ImVec2 iconPos = position;
    ImVec2 iconSize = ImVec2(64 * scale, 64 * scale);
    
    ImU32 iconBg = IM_COL32(60, 80, 120, static_cast<int>(200 * alpha));
    drawList->AddRectFilled(iconPos, ImVec2(iconPos.x + iconSize.x, iconPos.y + iconSize.y), 
                           iconBg, 5.0f);
    
    // Text
    ImVec2 textPos = ImVec2(position.x + 80, position.y + 10);
    
    ImGui::SetCursorScreenPos(textPos);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
    
    switch (reward.type) {
        case Reward::HERO:
            ImGui::TextColored(UITheme::BORDER_GOLD, "Hero: %s", reward.name.c_str());
            break;
        case Reward::EXPERIENCE:
            ImGui::TextColored(UITheme::TEXT_WHITE, "+%d %s", reward.value, reward.name.c_str());
            break;
        case Reward::GOLD:
            ImGui::TextColored(UITheme::BUTTON_GOLD, "+%d %s", reward.value, reward.name.c_str());
            break;
        case Reward::ITEM:
            ImGui::TextColored(UITheme::TEXT_WHITE, "%s", reward.name.c_str());
            break;
    }
    
    ImGui::PopStyleVar();
}

void RewardDisplayPanel::RenderBattleAxes(ImVec2 position, ImVec2 size) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    // Left axe
    ImVec2 leftAxePos = ImVec2(position.x - 50, position.y + 100);
    drawList->AddLine(leftAxePos, ImVec2(leftAxePos.x + 100, leftAxePos.y + 100), 
                     UITheme::BORDER_GOLD.ToU32(), 5.0f);
    
    // Right axe
    ImVec2 rightAxePos = ImVec2(position.x + size.x - 50, position.y + 100);
    drawList->AddLine(rightAxePos, ImVec2(rightAxePos.x - 100, rightAxePos.y + 100), 
                     UITheme::BORDER_GOLD.ToU32(), 5.0f);
}

} // namespace ArenaFighter