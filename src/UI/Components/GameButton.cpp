#include "GameButton.h"
#include <imgui_internal.h>
#include <algorithm>
#include <cmath>

namespace ArenaFighter {

// Static button cache for QuickButton
static std::unordered_map<std::string, std::unique_ptr<GameButton>> s_buttonCache;

GameButton::GameButton(const std::string& label, ImVec2 size)
    : m_label(label),
      m_size(size),
      m_currentSize(size),
      m_cornerRadius(UIStyle::CORNER_RADIUS),
      m_style(ButtonStyle::Primary),
      m_normalColor(UIStyle::BUTTON_RED),
      m_hoverColor(UIStyle::BUTTON_RED_HOVER),
      m_activeColor(UIStyle::BUTTON_RED_ACTIVE),
      m_textColor(UIStyle::TEXT_WHITE),
      m_borderColor(UIStyle::BORDER_BLUE),
      m_borderWidth(0.0f),
      m_font(nullptr),
      m_iconTexture(nullptr),
      m_iconSize(24.0f, 24.0f),
      m_iconOnLeft(true),
      m_hasIcon(false),
      m_state(ButtonState::Normal),
      m_previousState(ButtonState::Normal),
      m_enabled(true),
      m_selected(false),
      m_glowEnabled(false),
      m_glowColor(UIStyle::BORDER_GOLD),
      m_glowIntensity(0.0f),
      m_hoverScale(UIStyle::HOVER_SCALE),
      m_pressAnimationEnabled(true),
      m_animationTimer(0.0f),
      m_scaleAnimation(1.0f),
      m_glowAnimation(0.0f),
      m_pressAnimation(0.0f) {
}

GameButton::~GameButton() = default;

bool GameButton::Render(ImVec2 position) {
    if (!m_enabled) {
        m_state = ButtonState::Disabled;
    } else {
        UpdateState(position);
    }
    
    // Apply style colors
    ApplyStyle();
    
    // Render background
    RenderBackground(position);
    
    // Render glow effect
    if (m_glowEnabled && m_glowIntensity > 0.0f) {
        RenderGlow(position);
    }
    
    // Calculate content area
    ImVec2 contentPos = position;
    ImVec2 contentSize = m_currentSize;
    
    // Render icon if present
    if (m_hasIcon && m_iconTexture) {
        RenderIcon(contentPos);
    }
    
    // Render label
    RenderLabel(contentPos);
    
    // Handle tooltip
    if (!m_tooltip.empty() && m_state == ButtonState::Hovered) {
        HandleTooltip();
    }
    
    // Handle click
    bool clicked = false;
    if (m_state == ButtonState::Pressed && m_previousState == ButtonState::Hovered) {
        clicked = true;
        if (m_onClick) {
            m_onClick();
        }
    }
    
    // Handle hover callback
    if (m_state == ButtonState::Hovered && m_previousState == ButtonState::Normal) {
        if (m_onHover) {
            m_onHover();
        }
    }
    
    m_previousState = m_state;
    
    return clicked;
}

void GameButton::SetStyle(ButtonStyle style) {
    m_style = style;
    
    switch (style) {
        case ButtonStyle::Primary:
            m_normalColor = UIStyle::BUTTON_RED;
            m_hoverColor = UIStyle::BUTTON_RED_HOVER;
            m_activeColor = UIStyle::BUTTON_RED_ACTIVE;
            break;
            
        case ButtonStyle::Secondary:
            m_normalColor = UIStyle::BUTTON_GOLD;
            m_hoverColor = UIStyle::BUTTON_GOLD_HOVER;
            m_activeColor = UIStyle::BUTTON_GOLD_ACTIVE;
            break;
            
        case ButtonStyle::Danger:
            m_normalColor = {139, 0, 0, 255};
            m_hoverColor = {178, 34, 34, 255};
            m_activeColor = {100, 0, 0, 255};
            break;
            
        case ButtonStyle::Success:
            m_normalColor = {34, 139, 34, 255};
            m_hoverColor = {50, 205, 50, 255};
            m_activeColor = {0, 100, 0, 255};
            break;
            
        case ButtonStyle::Info:
            m_normalColor = UIStyle::BORDER_BLUE;
            m_hoverColor = {70, 170, 240, 255};
            m_activeColor = {30, 130, 200, 255};
            break;
            
        case ButtonStyle::Ghost:
            m_normalColor = {0, 0, 0, 0};
            m_hoverColor = {255, 255, 255, 20};
            m_activeColor = {255, 255, 255, 40};
            m_borderWidth = UIStyle::BORDER_WIDTH;
            break;
    }
}

void GameButton::SetCustomColors(const UIStyle::Color& normal, const UIStyle::Color& hover, const UIStyle::Color& active) {
    m_normalColor = normal;
    m_hoverColor = hover;
    m_activeColor = active;
}

void GameButton::SetIcon(const std::string& iconPath) {
    m_iconPath = iconPath;
    // Load texture (implementation depends on graphics system)
    // m_iconTexture = LoadTexture(iconPath);
    m_hasIcon = (m_iconTexture != nullptr);
}

void GameButton::Update(float deltaTime) {
    UpdateAnimations(deltaTime);
}

void GameButton::UpdateState(ImVec2 position) {
    ImVec2 mousePos = ImGui::GetMousePos();
    
    bool hovered = mousePos.x >= position.x && 
                   mousePos.x <= position.x + m_currentSize.x &&
                   mousePos.y >= position.y && 
                   mousePos.y <= position.y + m_currentSize.y;
    
    if (!m_enabled) {
        m_state = ButtonState::Disabled;
    } else if (m_selected) {
        m_state = ButtonState::Selected;
    } else if (hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        m_state = ButtonState::Pressed;
    } else if (hovered) {
        m_state = ButtonState::Hovered;
    } else {
        m_state = ButtonState::Normal;
    }
}

void GameButton::UpdateAnimations(float deltaTime) {
    m_animationTimer += deltaTime;
    
    // Scale animation
    float targetScale = 1.0f;
    if (m_state == ButtonState::Hovered) {
        targetScale = m_hoverScale;
    } else if (m_state == ButtonState::Pressed) {
        targetScale = 0.95f;
    }
    
    m_scaleAnimation += (targetScale - m_scaleAnimation) * deltaTime * UIStyle::HOVER_SPEED;
    m_currentSize = ImVec2(m_size.x * m_scaleAnimation, m_size.y * m_scaleAnimation);
    
    // Glow animation
    if (m_glowEnabled) {
        float targetGlow = (m_state == ButtonState::Hovered || m_state == ButtonState::Selected) ? 1.0f : 0.0f;
        m_glowAnimation += (targetGlow - m_glowAnimation) * deltaTime * UIStyle::HOVER_SPEED;
        m_glowIntensity = m_glowAnimation;
    }
    
    // Press animation
    if (m_pressAnimationEnabled && m_state == ButtonState::Pressed) {
        m_pressAnimation = 1.0f;
    } else {
        m_pressAnimation *= (1.0f - deltaTime * 10.0f);
    }
}

void GameButton::ApplyStyle() {
    ImVec4 color = GetCurrentColor();
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, m_cornerRadius);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, m_borderWidth);
    ImGui::PushStyleColor(ImGuiCol_Border, m_borderColor);
}

ImVec4 GameButton::GetCurrentColor() const {
    switch (m_state) {
        case ButtonState::Hovered:
            return m_hoverColor;
        case ButtonState::Pressed:
            return m_activeColor;
        case ButtonState::Selected:
            return m_activeColor;
        case ButtonState::Disabled:
            return ImVec4(m_normalColor.r/255.0f * 0.5f, 
                         m_normalColor.g/255.0f * 0.5f, 
                         m_normalColor.b/255.0f * 0.5f, 
                         m_normalColor.a/255.0f * 0.5f);
        default:
            return m_normalColor;
    }
}

void GameButton::RenderBackground(ImVec2 position) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    // Calculate actual position based on scale
    ImVec2 scaledPos = position;
    if (m_scaleAnimation != 1.0f) {
        float offsetX = (m_currentSize.x - m_size.x) * 0.5f;
        float offsetY = (m_currentSize.y - m_size.y) * 0.5f;
        scaledPos = ImVec2(position.x - offsetX, position.y - offsetY);
    }
    
    ImVec2 pMax = ImVec2(scaledPos.x + m_currentSize.x, scaledPos.y + m_currentSize.y);
    
    // Background
    ImU32 bgColor = ImGui::ColorConvertFloat4ToU32(GetCurrentColor());
    drawList->AddRectFilled(scaledPos, pMax, bgColor, m_cornerRadius);
    
    // Border
    if (m_borderWidth > 0.0f) {
        drawList->AddRect(scaledPos, pMax, m_borderColor.ToU32(), m_cornerRadius, 0, m_borderWidth);
    }
    
    // Press effect
    if (m_pressAnimation > 0.0f && m_pressAnimationEnabled) {
        float pressScale = 1.0f + m_pressAnimation * 0.1f;
        ImVec2 pressMin = ImVec2(scaledPos.x - 5 * m_pressAnimation, 
                                scaledPos.y - 5 * m_pressAnimation);
        ImVec2 pressMax = ImVec2(pMax.x + 5 * m_pressAnimation, 
                                pMax.y + 5 * m_pressAnimation);
        
        ImU32 pressColor = IM_COL32(255, 255, 255, 50 * m_pressAnimation);
        drawList->AddRect(pressMin, pressMax, pressColor, m_cornerRadius + 5, 0, 2.0f);
    }
    
    ImGui::PopStyleColor(4); // Pop the colors and border we pushed
    ImGui::PopStyleVar(2); // Pop the style vars
}

void GameButton::RenderIcon(ImVec2 position) {
    if (!m_iconTexture) return;
    
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    // Calculate icon position
    float iconX = position.x + 10.0f;
    float iconY = position.y + (m_currentSize.y - m_iconSize.y) * 0.5f;
    
    if (!m_iconOnLeft) {
        iconX = position.x + m_currentSize.x - m_iconSize.x - 10.0f;
    }
    
    ImVec2 iconPos(iconX, iconY);
    ImVec2 iconMax(iconX + m_iconSize.x, iconY + m_iconSize.y);
    
    // Render icon with tint based on state
    ImU32 tint = IM_COL32(255, 255, 255, m_state == ButtonState::Disabled ? 128 : 255);
    drawList->AddImage(m_iconTexture, iconPos, iconMax, ImVec2(0, 0), ImVec2(1, 1), tint);
}

void GameButton::RenderLabel(ImVec2 position) {
    if (m_font) {
        ImGui::PushFont(m_font);
    }
    
    // Calculate text position (centered)
    ImVec2 textSize = ImGui::CalcTextSize(m_label.c_str());
    float textX = position.x + (m_currentSize.x - textSize.x) * 0.5f;
    float textY = position.y + (m_currentSize.y - textSize.y) * 0.5f;
    
    // Adjust for icon
    if (m_hasIcon && m_iconTexture) {
        if (m_iconOnLeft) {
            textX += (m_iconSize.x + 5.0f) * 0.5f;
        } else {
            textX -= (m_iconSize.x + 5.0f) * 0.5f;
        }
    }
    
    // Render text
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImU32 textColor = m_textColor.ToU32();
    
    if (m_state == ButtonState::Disabled) {
        textColor = IM_COL32(m_textColor.r * 0.5f, m_textColor.g * 0.5f, 
                            m_textColor.b * 0.5f, m_textColor.a * 0.5f);
    }
    
    drawList->AddText(ImVec2(textX, textY), textColor, m_label.c_str());
    
    if (m_font) {
        ImGui::PopFont();
    }
}

void GameButton::RenderGlow(ImVec2 position) {
    if (m_glowIntensity <= 0.0f) return;
    
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    
    // Calculate glow bounds
    float glowSize = 10.0f * m_glowIntensity;
    ImVec2 glowMin = ImVec2(position.x - glowSize, position.y - glowSize);
    ImVec2 glowMax = ImVec2(position.x + m_currentSize.x + glowSize, 
                           position.y + m_currentSize.y + glowSize);
    
    // Animated glow
    float time = ImGui::GetTime();
    float pulse = 0.8f + 0.2f * sin(time * 2.0f);
    
    UIStyle::Color glow = m_glowColor;
    glow.a = static_cast<float>(glow.a) * m_glowIntensity * pulse * 0.5f;
    
    // Multi-layer glow
    for (int i = 0; i < 2; ++i) {
        float layerAlpha = glow.a * (1.0f - i * 0.5f);
        UIStyle::Color layerGlow = glow;
        layerGlow.a = static_cast<float>(layerGlow.a) * layerAlpha;
        
        float layerSize = glowSize * (1.0f - i * 0.3f);
        ImVec2 layerMin = ImVec2(position.x - layerSize, position.y - layerSize);
        ImVec2 layerMax = ImVec2(position.x + m_currentSize.x + layerSize, 
                                position.y + m_currentSize.y + layerSize);
        
        drawList->AddRect(layerMin, layerMax, layerGlow.ToU32(), 
                         m_cornerRadius + layerSize, 0, 2.0f);
    }
}

void GameButton::HandleTooltip() {
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("%s", m_tooltip.c_str());
        ImGui::EndTooltip();
    }
}

} // namespace ArenaFighter