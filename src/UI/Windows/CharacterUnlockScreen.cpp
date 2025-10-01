#include "CharacterUnlockScreen.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <cmath>
#include <algorithm>

namespace ArenaFighter {

CharacterUnlockScreen::CharacterUnlockScreen()
    : UIPanel("CharacterUnlock", ImVec2(0, 0), ImGui::GetIO().DisplaySize),
      m_currentState(INITIAL_GLOW),
      m_stateTimer(0.0f),
      m_transitionDuration(1.0f),
      m_unlockType(PERMANENT_HERO),
      m_showHelm(true),
      m_showArmor(true) {
    
    // Set fullscreen dark background
    SetBackgroundColor({0, 0, 0, 200});
    SetFlags(PanelFlags::NoBackground | PanelFlags::NoBorder);
    
    // Create components
    m_characterPreview = std::make_unique<Character3DPreview>();
    m_particleEffects = std::make_unique<ParticleSystem>();
    m_unlockAnimation = std::make_unique<UnlockAnimation>();
    
    // Create UI panels
    m_infoPanel = std::make_unique<UIPanel>("InfoPanel", 
        ImVec2(m_size.x * 0.5f - 300, m_size.y * 0.7f), ImVec2(600, 150));
    m_infoPanel->SetBackgroundColor(UITheme::PANEL_BG_LOST);
    
    m_customizationPanel = std::make_unique<CharacterCustomizationPanel>();
    m_customizationPanel->SetPosition(ImVec2(m_size.x * 0.1f, m_size.y * 0.2f));
    m_customizationPanel->SetSize(ImVec2(m_size.x * 0.8f, m_size.y * 0.6f));
    
    m_rewardPanel = std::make_unique<RewardDisplayPanel>();
    m_rewardPanel->SetPosition(ImVec2(m_size.x * 0.5f - 400, m_size.y * 0.5f - 300));
    m_rewardPanel->SetSize(ImVec2(800, 600));
    
    // Create buttons
    m_skipButton = std::make_unique<GameButton>("Skip", ImVec2(100, 40));
    m_skipButton->SetStyle(ButtonStyle::Ghost);
    m_skipButton->SetOnClick([this]() { SkipToEnd(); });
    
    m_nextButton = std::make_unique<GameButton>("Next", ImVec2(150, 50));
    m_nextButton->SetStyle(ButtonStyle::Primary);
    m_nextButton->SetOnClick([this]() { 
        if (m_currentState == CUSTOMIZATION) {
            TransitionToState(REWARD_DISPLAY);
        } else if (m_currentState == REWARD_DISPLAY) {
            TransitionToState(COMPLETE);
        }
    });
    
    m_helmToggle = std::make_unique<GameButton>("Helm Off", ImVec2(120, 40));
    m_helmToggle->SetStyle(ButtonStyle::Secondary);
    m_helmToggle->SetOnClick([this]() {
        m_showHelm = !m_showHelm;
        m_helmToggle->SetLabel(m_showHelm ? "Helm Off" : "Helm On");
        m_characterPreview->ShowHelmet(m_showHelm);
    });
    
    m_armorToggle = std::make_unique<GameButton>("Armor Off", ImVec2(120, 40));
    m_armorToggle->SetStyle(ButtonStyle::Secondary);
    m_armorToggle->SetOnClick([this]() {
        m_showArmor = !m_showArmor;
        m_armorToggle->SetLabel(m_showArmor ? "Armor Off" : "Armor On");
        m_characterPreview->ShowArmor(m_showArmor);
    });
}

CharacterUnlockScreen::~CharacterUnlockScreen() = default;

void CharacterUnlockScreen::SetUnlockData(const std::string& characterName, UnlockType type) {
    m_characterName = characterName;
    m_unlockType = type;
    
    switch (type) {
        case PERMANENT_HERO:
            m_characterTitle = "Permanent Hero";
            break;
        case TEMPORARY_HERO:
            m_characterTitle = "Temporary Hero (7 Days)";
            break;
        case SPECIAL_UNLOCK:
            m_characterTitle = "Special Unlock";
            break;
        case QUEST_REWARD:
            m_characterTitle = "Quest Reward";
            break;
    }
    
    m_characterPreview->LoadCharacter(characterName);
    m_customizationPanel->SetCharacter(m_characterPreview.get());
}

void CharacterUnlockScreen::SetRewards(int exp, int gold, const std::vector<std::string>& items) {
    m_rewards.experience = exp;
    m_rewards.gold = gold;
    m_rewards.items = items;
    
    // Set up reward panel
    m_rewardPanel->SetHeroReward(m_characterName, m_unlockType == PERMANENT_HERO);
    m_rewardPanel->SetExperienceReward(exp);
    m_rewardPanel->SetGoldReward(gold);
    
    for (const auto& item : items) {
        m_rewardPanel->AddItemReward(item, nullptr); // TODO: Load item icons
    }
}

void CharacterUnlockScreen::StartUnlockSequence() {
    m_currentState = INITIAL_GLOW;
    m_stateTimer = 0.0f;
    m_unlockAnimation->StartGlowAnimation();
    
    // Emit initial particles
    ImVec2 center = ImVec2(m_size.x * 0.5f, m_size.y * 0.5f);
    m_particleEffects->EmitLightOrbs(center, 20);
}

void CharacterUnlockScreen::SkipToEnd() {
    TransitionToState(REWARD_DISPLAY);
}

void CharacterUnlockScreen::Update(float deltaTime) {
    UIPanel::Update(deltaTime);
    
    m_stateTimer += deltaTime;
    m_unlockAnimation->Update(deltaTime);
    m_particleEffects->Update(deltaTime);
    m_characterPreview->Update(deltaTime);
    
    switch (m_currentState) {
        case INITIAL_GLOW:
            UpdateInitialGlow(deltaTime);
            break;
        case REVEAL_CHARACTER:
            UpdateRevealCharacter(deltaTime);
            break;
        case DISPLAY_INFO:
            UpdateDisplayInfo(deltaTime);
            break;
        case CUSTOMIZATION:
            UpdateCustomization(deltaTime);
            break;
        case REWARD_DISPLAY:
            UpdateRewardDisplay(deltaTime);
            break;
        case COMPLETE:
            if (m_onComplete) {
                m_onComplete();
            }
            break;
    }
}

void CharacterUnlockScreen::Render() {
    if (!IsVisible()) return;
    
    // Darken background
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    drawList->AddRectFilled(ImVec2(0, 0), m_size, IM_COL32(0, 0, 0, 200));
    
    // Render based on state
    switch (m_currentState) {
        case INITIAL_GLOW:
            RenderGlowEffect();
            break;
        case REVEAL_CHARACTER:
            RenderCharacterReveal();
            break;
        case DISPLAY_INFO:
            RenderCharacterReveal();
            RenderInfoPanel();
            break;
        case CUSTOMIZATION:
            RenderCustomizationPanel();
            break;
        case REWARD_DISPLAY:
            RenderRewardPanel();
            break;
    }
    
    // Always render particles on top
    RenderParticles();
    
    // Render skip button in early states
    if (m_currentState < CUSTOMIZATION) {
        m_skipButton->Render(ImVec2(m_size.x - 120, 20));
    }
}

void CharacterUnlockScreen::UpdateInitialGlow(float deltaTime) {
    if (m_stateTimer >= 2.0f) {
        TransitionToState(REVEAL_CHARACTER);
        m_unlockAnimation->StartRevealAnimation();
    }
    
    // Emit more particles as glow intensifies
    if (fmod(m_stateTimer, 0.1f) < deltaTime) {
        ImVec2 center = ImVec2(m_size.x * 0.5f, m_size.y * 0.5f);
        m_particleEffects->EmitSparkles(center, 5);
    }
}

void CharacterUnlockScreen::UpdateRevealCharacter(float deltaTime) {
    if (m_stateTimer >= 3.0f) {
        TransitionToState(DISPLAY_INFO);
    }
    
    // Rotate character during reveal
    float rotation = m_stateTimer * 90.0f; // 90 degrees per second
    m_characterPreview->SetRotation(rotation);
}

void CharacterUnlockScreen::UpdateDisplayInfo(float deltaTime) {
    if (m_stateTimer >= 3.0f) {
        TransitionToState(CUSTOMIZATION);
        m_customizationPanel->Initialize();
    }
}

void CharacterUnlockScreen::UpdateCustomization(float deltaTime) {
    m_customizationPanel->Update(deltaTime);
}

void CharacterUnlockScreen::UpdateRewardDisplay(float deltaTime) {
    if (m_stateTimer == 0.0f) {
        m_rewardPanel->StartRevealAnimation();
        m_unlockAnimation->StartCelebrationAnimation();
        
        // Celebration particles
        ImVec2 center = ImVec2(m_size.x * 0.5f, m_size.y * 0.5f);
        m_particleEffects->EmitCelebration(center);
    }
}

void CharacterUnlockScreen::RenderGlowEffect() {
    ImVec2 center = ImVec2(m_size.x * 0.5f, m_size.y * 0.5f);
    m_unlockAnimation->Render(center);
}

void CharacterUnlockScreen::RenderCharacterReveal() {
    ImVec2 center = ImVec2(m_size.x * 0.5f, m_size.y * 0.4f);
    ImVec2 previewSize = ImVec2(400, 500);
    ImVec2 previewPos = ImVec2(center.x - previewSize.x * 0.5f, center.y - previewSize.y * 0.5f);
    
    m_characterPreview->Render(previewPos, previewSize);
    m_unlockAnimation->Render(center);
}

void CharacterUnlockScreen::RenderInfoPanel() {
    m_infoPanel->Begin();
    
    // Title
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]); // Large font
    ImGui::TextColored(UITheme::BORDER_GOLD, "%s", m_characterTitle.c_str());
    ImGui::PopFont();
    
    // Character name
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]); // Extra large font
    ImGui::SetCursorPosX((m_infoPanel->GetSize().x - ImGui::CalcTextSize(m_characterName.c_str()).x) * 0.5f);
    ImGui::TextColored(UITheme::TEXT_WHITE, "%s", m_characterName.c_str());
    ImGui::PopFont();
    
    // Toggle buttons
    ImGui::SetCursorPos(ImVec2(m_infoPanel->GetSize().x * 0.5f - 130, m_infoPanel->GetSize().y - 60));
    m_helmToggle->Render(ImGui::GetCursorScreenPos());
    ImGui::SetCursorPos(ImVec2(m_infoPanel->GetSize().x * 0.5f + 10, m_infoPanel->GetSize().y - 60));
    m_armorToggle->Render(ImGui::GetCursorScreenPos());
    
    m_infoPanel->End();
}

void CharacterUnlockScreen::RenderCustomizationPanel() {
    m_customizationPanel->Render();
    
    // Next button
    m_nextButton->Render(ImVec2(m_size.x - 170, m_size.y - 70));
}

void CharacterUnlockScreen::RenderRewardPanel() {
    m_rewardPanel->Render();
    
    // Exit button with countdown
    int countdown = std::max(0, 25 - static_cast<int>(m_stateTimer));
    char exitLabel[64];
    snprintf(exitLabel, sizeof(exitLabel), "Exit...%d (ESC)", countdown);
    
    GameButton exitButton(exitLabel, ImVec2(150, 50));
    exitButton.SetStyle(ButtonStyle::Secondary);
    exitButton.SetEnabled(countdown == 0);
    exitButton.Render(ImVec2(m_size.x * 0.5f - 75, m_size.y - 100));
    
    if (countdown == 0 && ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        TransitionToState(COMPLETE);
    }
}

void CharacterUnlockScreen::RenderParticles() {
    m_particleEffects->Render();
}

void CharacterUnlockScreen::TransitionToState(UnlockState newState) {
    m_currentState = newState;
    m_stateTimer = 0.0f;
}

// UnlockAnimation Implementation
UnlockAnimation::UnlockAnimation()
    : m_currentAnimation(GLOW_BURST),
      m_progress(0.0f),
      m_duration(2.0f),
      m_isComplete(false),
      m_glowRadius(50.0f),
      m_glowIntensity(0.0f),
      m_glowColor(IM_COL32(255, 220, 100, 255)),
      m_revealHeight(0.0f),
      m_fadeInAlpha(0.0f) {
    
    // Initialize light rays
    for (int i = 0; i < 12; ++i) {
        m_lightRays.push_back(rand() % 360);
    }
}

void UnlockAnimation::StartGlowAnimation() {
    m_currentAnimation = GLOW_BURST;
    m_progress = 0.0f;
    m_duration = 2.0f;
    m_isComplete = false;
}

void UnlockAnimation::StartRevealAnimation() {
    m_currentAnimation = CHARACTER_REVEAL;
    m_progress = 0.0f;
    m_duration = 3.0f;
    m_isComplete = false;
}

void UnlockAnimation::StartCelebrationAnimation() {
    m_currentAnimation = CELEBRATION;
    m_progress = 0.0f;
    m_duration = 5.0f;
    m_isComplete = false;
}

void UnlockAnimation::Update(float deltaTime) {
    if (m_isComplete) return;
    
    m_progress += deltaTime / m_duration;
    if (m_progress >= 1.0f) {
        m_progress = 1.0f;
        m_isComplete = true;
    }
    
    switch (m_currentAnimation) {
        case GLOW_BURST:
            m_glowIntensity = m_progress;
            m_glowRadius = 50.0f + m_progress * 200.0f;
            break;
            
        case CHARACTER_REVEAL:
            m_fadeInAlpha = m_progress;
            m_revealHeight = m_progress * 500.0f;
            break;
            
        case CELEBRATION:
            // Pulsing effect
            m_glowIntensity = 0.5f + 0.5f * sin(m_progress * 10.0f);
            break;
    }
}

void UnlockAnimation::Render(ImVec2 centerPos) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    switch (m_currentAnimation) {
        case GLOW_BURST:
            RenderGlowBurst(centerPos);
            RenderLightRays(centerPos);
            break;
            
        case CHARACTER_REVEAL:
            RenderRevealEffect(centerPos);
            break;
            
        case CELEBRATION:
            RenderGlowBurst(centerPos);
            break;
    }
}

void UnlockAnimation::RenderGlowBurst(ImVec2 center) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    // Multiple layers of glow
    for (int i = 3; i >= 0; --i) {
        float radius = m_glowRadius * (1.0f + i * 0.3f);
        float alpha = m_glowIntensity * (1.0f - i * 0.25f) * 0.3f;
        ImU32 color = IM_COL32(255, 220, 100, static_cast<int>(255 * alpha));
        
        drawList->AddCircleFilled(center, radius, color, 64);
    }
    
    // Core bright center
    float coreRadius = m_glowRadius * 0.3f;
    drawList->AddCircleFilled(center, coreRadius, 
        IM_COL32(255, 255, 255, static_cast<int>(255 * m_glowIntensity)));
}

void UnlockAnimation::RenderLightRays(ImVec2 center) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    for (size_t i = 0; i < m_lightRays.size(); ++i) {
        float angle = m_lightRays[i] + m_progress * 30.0f;
        float radian = angle * 3.14159f / 180.0f;
        
        float length = 200.0f + sin(m_progress * 5.0f + i) * 50.0f;
        float width = 2.0f + sin(m_progress * 3.0f + i) * 1.0f;
        
        ImVec2 start = center;
        ImVec2 end = ImVec2(
            center.x + cos(radian) * length,
            center.y + sin(radian) * length
        );
        
        ImU32 color = IM_COL32(255, 220, 100, static_cast<int>(100 * m_glowIntensity));
        drawList->AddLine(start, end, color, width);
    }
}

void UnlockAnimation::RenderRevealEffect(ImVec2 center) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    // Vertical light beam
    float beamWidth = 100.0f * (1.0f - m_progress);
    float beamHeight = m_revealHeight;
    
    ImVec2 topLeft = ImVec2(center.x - beamWidth * 0.5f, center.y - beamHeight * 0.5f);
    ImVec2 bottomRight = ImVec2(center.x + beamWidth * 0.5f, center.y + beamHeight * 0.5f);
    
    // Gradient fill
    ImU32 topColor = IM_COL32(255, 255, 255, static_cast<int>(255 * m_fadeInAlpha));
    ImU32 bottomColor = IM_COL32(255, 220, 100, static_cast<int>(100 * m_fadeInAlpha));
    
    drawList->AddRectFilledMultiColor(topLeft, bottomRight, 
        topColor, topColor, bottomColor, bottomColor);
}

// Character3DPreview Implementation
Character3DPreview::Character3DPreview()
    : m_characterModel(nullptr),
      m_renderTexture(nullptr),
      m_rotation(0.0f),
      m_scale(1.0f),
      m_animationTime(0.0f),
      m_showHelmet(true),
      m_showArmor(true) {
}

Character3DPreview::~Character3DPreview() = default;

void Character3DPreview::LoadCharacter(const std::string& characterName) {
    // TODO: Load actual 3D model
    // For now, we'll simulate with placeholder
}

void Character3DPreview::SetPose(const std::string& poseName) {
    // TODO: Set character pose
}

void Character3DPreview::SetRotation(float angle) {
    m_rotation = angle;
}

void Character3DPreview::SetScale(float scale) {
    m_scale = scale;
}

void Character3DPreview::ShowHelmet(bool show) {
    m_showHelmet = show;
}

void Character3DPreview::ShowArmor(bool show) {
    m_showArmor = show;
}

void Character3DPreview::Update(float deltaTime) {
    m_animationTime += deltaTime;
}

void Character3DPreview::Render(ImVec2 position, ImVec2 size) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    // Render platform effect
    RenderPlatform(position, size);
    
    // TODO: Render actual 3D character
    // For now, render placeholder
    ImVec2 center = ImVec2(position.x + size.x * 0.5f, position.y + size.y * 0.5f);
    
    // Character silhouette placeholder
    ImU32 charColor = IM_COL32(100, 150, 200, 255);
    drawList->AddRectFilled(
        ImVec2(center.x - 50, center.y - 100),
        ImVec2(center.x + 50, center.y + 100),
        charColor, 5.0f
    );
    
    // Render energy rings
    RenderEnergyRings(ImVec2(center.x, position.y + size.y - 50), 150.0f);
}

void Character3DPreview::RenderPlatform(ImVec2 position, ImVec2 size) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    ImVec2 center = ImVec2(position.x + size.x * 0.5f, position.y + size.y - 50);
    
    // Circular platform
    float radius = 120.0f;
    ImU32 platformColor = IM_COL32(20, 40, 80, 200);
    drawList->AddCircleFilled(center, radius, platformColor, 32);
    
    // Platform border
    ImU32 borderColor = IM_COL32(100, 150, 255, 255);
    drawList->AddCircle(center, radius, borderColor, 32, 3.0f);
}

void Character3DPreview::RenderEnergyRings(ImVec2 center, float radius) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    // Animated energy rings
    for (int i = 0; i < 3; ++i) {
        float ringRadius = radius * (0.6f + i * 0.2f);
        float alpha = 0.5f + 0.5f * sin(m_animationTime * 2.0f + i);
        
        ImU32 ringColor = IM_COL32(100, 150, 255, static_cast<int>(100 * alpha));
        drawList->AddCircle(center, ringRadius, ringColor, 64, 2.0f);
    }
}

void Character3DPreview::PlayAnimation(const std::string& animationName) {
    m_currentAnimation = animationName;
    m_animationTime = 0.0f;
}

void Character3DPreview::StopAnimation() {
    m_currentAnimation = "";
}

} // namespace ArenaFighter