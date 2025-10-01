#include "UIPanel.h"
#include <imgui_internal.h>
#include <algorithm>

namespace ArenaFighter {

UIPanel::UIPanel(const std::string& id, ImVec2 position, ImVec2 size)
    : m_id(id),
      m_position(position),
      m_size(size),
      m_padding(UIStyle::PANEL_PADDING, UIStyle::PANEL_PADDING),
      m_backgroundColor(UIStyle::PANEL_BG),
      m_borderColor(UIStyle::BORDER_BLUE),
      m_borderWidth(UIStyle::BORDER_WIDTH),
      m_cornerRadius(UIStyle::CORNER_RADIUS),
      m_flags(PanelFlags::None),
      m_glowColor(UIStyle::BORDER_GOLD),
      m_glowIntensity(0.0f),
      m_useGradient(false),
      m_alpha(1.0f),
      m_animatedPosition(position),
      m_targetPosition(position),
      m_animationTimer(0.0f),
      m_animationDuration(0.0f),
      m_isAnimating(false),
      m_visible(true),
      m_isHovered(false),
      m_isFocused(false),
      m_isOpen(false) {
}

UIPanel::~UIPanel() = default;

void UIPanel::Begin() {
    if (!m_visible || m_alpha <= 0.01f) return;
    
    // Update hover state
    ImVec2 mousePos = ImGui::GetMousePos();
    m_isHovered = mousePos.x >= m_animatedPosition.x && 
                  mousePos.x <= m_animatedPosition.x + m_size.x &&
                  mousePos.y >= m_animatedPosition.y && 
                  mousePos.y <= m_animatedPosition.y + m_size.y;
    
    // Push alpha
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_alpha);
    
    // Set window flags
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | 
                                   ImGuiWindowFlags_NoScrollbar |
                                   ImGuiWindowFlags_NoCollapse;
    
    if (!(m_flags & PanelFlags::Draggable)) {
        windowFlags |= ImGuiWindowFlags_NoMove;
    }
    
    if (!(m_flags & PanelFlags::Resizable)) {
        windowFlags |= ImGuiWindowFlags_NoResize;
    }
    
    if (m_flags & PanelFlags::AutoResize) {
        windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
    }
    
    // Set position and size
    ImGui::SetNextWindowPos(m_animatedPosition);
    ImGui::SetNextWindowSize(m_size);
    
    // Custom rendering for background and effects
    if (!(m_flags & PanelFlags::NoBackground)) {
        RenderBackground();
    }
    
    if (m_flags & PanelFlags::GlowEffect && m_glowIntensity > 0.0f) {
        RenderGlow();
    }
    
    // Begin window
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_padding);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 
                       (m_flags & PanelFlags::NoBorder) ? 0.0f : m_borderWidth);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 
                       (m_flags & PanelFlags::NoRounding) ? 0.0f : m_cornerRadius);
    
    ImGui::PushStyleColor(ImGuiCol_WindowBg, m_backgroundColor);
    ImGui::PushStyleColor(ImGuiCol_Border, m_borderColor);
    
    m_isOpen = ImGui::Begin(m_id.c_str(), nullptr, windowFlags);
}

void UIPanel::End() {
    if (!m_visible || m_alpha <= 0.01f) return;
    
    if (m_isOpen) {
        // Update focus state
        m_isFocused = ImGui::IsWindowFocused();
        
        // Custom border rendering if needed
        if (!(m_flags & PanelFlags::NoBorder)) {
            RenderBorder();
        }
    }
    
    ImGui::End();
    
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(3);
    ImGui::PopStyleVar(); // Alpha
}

void UIPanel::Update(float deltaTime) {
    // Update animations
    if (m_isAnimating) {
        UpdateAnimation(deltaTime);
    }
    
    // Update glow effect
    if (m_flags & PanelFlags::GlowEffect) {
        if (m_isHovered) {
            m_glowIntensity = std::min(m_glowIntensity + deltaTime * 3.0f, 1.0f);
        } else {
            m_glowIntensity = std::max(m_glowIntensity - deltaTime * 2.0f, 0.0f);
        }
    }
}

void UIPanel::SetGradientColors(const UIStyle::Color& top, const UIStyle::Color& bottom) {
    m_gradientTop = top;
    m_gradientBottom = bottom;
    m_useGradient = true;
    m_flags = m_flags | PanelFlags::GradientBG;
}

void UIPanel::FadeIn(float duration) {
    m_alpha = 0.0f;
    m_animationDuration = duration;
    m_animationTimer = 0.0f;
    m_isAnimating = true;
}

void UIPanel::FadeOut(float duration) {
    m_alpha = 1.0f;
    m_animationDuration = duration;
    m_animationTimer = 0.0f;
    m_isAnimating = true;
}

void UIPanel::SlideIn(ImVec2 from, float duration) {
    m_animatedPosition = from;
    m_targetPosition = m_position;
    m_animationDuration = duration;
    m_animationTimer = 0.0f;
    m_isAnimating = true;
}

void UIPanel::SlideOut(ImVec2 to, float duration) {
    m_animatedPosition = m_position;
    m_targetPosition = to;
    m_animationDuration = duration;
    m_animationTimer = 0.0f;
    m_isAnimating = true;
}

void UIPanel::RenderBackground() {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 p = m_animatedPosition;
    ImVec2 pMax = ImVec2(p.x + m_size.x, p.y + m_size.y);
    
    if (m_flags & PanelFlags::GradientBG && m_useGradient) {
        // Render gradient background
        ImU32 colTop = m_gradientTop.ToU32();
        ImU32 colBottom = m_gradientBottom.ToU32();
        
        drawList->AddRectFilledMultiColor(p, pMax, colTop, colTop, colBottom, colBottom);
    } else {
        // Solid background (handled by ImGui)
    }
}

void UIPanel::RenderBorder() {
    if (m_borderWidth <= 0.0f) return;
    
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 p = m_animatedPosition;
    ImVec2 pMax = ImVec2(p.x + m_size.x, p.y + m_size.y);
    
    // Custom border rendering for special effects
    if (m_isHovered && (m_flags & PanelFlags::GlowEffect)) {
        // Animated border on hover
        float time = ImGui::GetTime();
        float pulse = 0.8f + 0.2f * sin(time * 3.0f);
        
        UIStyle::Color animBorder = m_borderColor;
        animBorder.a = static_cast<float>(animBorder.a) * pulse;
        
        drawList->AddRect(p, pMax, animBorder.ToU32(), m_cornerRadius, 0, m_borderWidth);
    }
}

void UIPanel::RenderGlow() {
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    ImVec2 p = m_animatedPosition;
    ImVec2 pMax = ImVec2(p.x + m_size.x, p.y + m_size.y);
    
    float glowSize = 20.0f * m_glowIntensity;
    ImVec2 glowMin = ImVec2(p.x - glowSize, p.y - glowSize);
    ImVec2 glowMax = ImVec2(pMax.x + glowSize, pMax.y + glowSize);
    
    // Multi-layer glow
    for (int i = 0; i < 3; ++i) {
        float alpha = (1.0f - (i * 0.3f)) * m_glowIntensity * 0.3f;
        UIStyle::Color glow = m_glowColor;
        glow.a = static_cast<float>(glow.a) * alpha;
        
        float size = glowSize * (1.0f - i * 0.3f);
        ImVec2 min = ImVec2(p.x - size, p.y - size);
        ImVec2 max = ImVec2(pMax.x + size, pMax.y + size);
        
        drawList->AddRect(min, max, glow.ToU32(), m_cornerRadius + size, 0, 2.0f);
    }
}

void UIPanel::UpdateAnimation(float deltaTime) {
    m_animationTimer += deltaTime;
    
    if (m_animationTimer >= m_animationDuration) {
        m_animationTimer = m_animationDuration;
        m_isAnimating = false;
    }
    
    float t = m_animationTimer / m_animationDuration;
    // Smooth easing
    t = t * t * (3.0f - 2.0f * t);
    
    // Update position
    m_animatedPosition.x = m_position.x + (m_targetPosition.x - m_position.x) * t;
    m_animatedPosition.y = m_position.y + (m_targetPosition.y - m_position.y) * t;
    
    // Update alpha for fade animations
    if (m_targetPosition == m_position) {
        // This is a fade animation
        if (m_alpha < 0.5f) {
            // Fading in
            m_alpha = t;
        } else {
            // Fading out
            m_alpha = 1.0f - t;
        }
    }
}

} // namespace ArenaFighter