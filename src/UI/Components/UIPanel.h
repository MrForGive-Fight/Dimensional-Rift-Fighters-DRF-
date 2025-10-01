#pragma once
#include <imgui.h>
#include "../Core/UIStyle.h"
#include <string>
#include <functional>

namespace ArenaFighter {

enum class PanelFlags {
    None = 0,
    NoBackground = 1 << 0,
    NoBorder = 1 << 1,
    NoRounding = 1 << 2,
    Draggable = 1 << 3,
    Resizable = 1 << 4,
    AutoResize = 1 << 5,
    GlowEffect = 1 << 6,
    GradientBG = 1 << 7
};

inline PanelFlags operator|(PanelFlags a, PanelFlags b) {
    return static_cast<PanelFlags>(static_cast<int>(a) | static_cast<int>(b));
}

inline bool operator&(PanelFlags a, PanelFlags b) {
    return static_cast<int>(a) & static_cast<int>(b);
}

class UIPanel {
public:
    UIPanel(const std::string& id, ImVec2 position, ImVec2 size);
    ~UIPanel();
    
    // Main render function
    void Begin();
    void End();
    
    // Style setters
    void SetBackgroundColor(const UIStyle::Color& color) { m_backgroundColor = color; }
    void SetBorderColor(const UIStyle::Color& color) { m_borderColor = color; }
    void SetBorderWidth(float width) { m_borderWidth = width; }
    void SetCornerRadius(float radius) { m_cornerRadius = radius; }
    void SetFlags(PanelFlags flags) { m_flags = flags; }
    void SetPadding(float padding) { m_padding = ImVec2(padding, padding); }
    void SetPadding(ImVec2 padding) { m_padding = padding; }
    
    // Position and size
    void SetPosition(ImVec2 pos) { m_position = pos; }
    void SetSize(ImVec2 size) { m_size = size; }
    ImVec2 GetPosition() const { return m_position; }
    ImVec2 GetSize() const { return m_size; }
    
    // Effects
    void SetGlowColor(const UIStyle::Color& color) { m_glowColor = color; }
    void SetGlowIntensity(float intensity) { m_glowIntensity = intensity; }
    void SetGradientColors(const UIStyle::Color& top, const UIStyle::Color& bottom);
    
    // Animation
    void FadeIn(float duration = 0.3f);
    void FadeOut(float duration = 0.3f);
    void SlideIn(ImVec2 from, float duration = 0.5f);
    void SlideOut(ImVec2 to, float duration = 0.5f);
    
    // Update
    void Update(float deltaTime);
    
    // State
    bool IsVisible() const { return m_visible && m_alpha > 0.01f; }
    void SetVisible(bool visible) { m_visible = visible; }
    bool IsHovered() const { return m_isHovered; }
    bool IsFocused() const { return m_isFocused; }
    
private:
    std::string m_id;
    ImVec2 m_position;
    ImVec2 m_size;
    ImVec2 m_padding;
    
    // Style
    UIStyle::Color m_backgroundColor;
    UIStyle::Color m_borderColor;
    float m_borderWidth;
    float m_cornerRadius;
    PanelFlags m_flags;
    
    // Effects
    UIStyle::Color m_glowColor;
    float m_glowIntensity;
    UIStyle::Color m_gradientTop;
    UIStyle::Color m_gradientBottom;
    bool m_useGradient;
    
    // Animation state
    float m_alpha;
    ImVec2 m_animatedPosition;
    ImVec2 m_targetPosition;
    float m_animationTimer;
    float m_animationDuration;
    bool m_isAnimating;
    
    // State
    bool m_visible;
    bool m_isHovered;
    bool m_isFocused;
    bool m_isOpen;
    
    // Internal rendering
    void RenderBackground();
    void RenderBorder();
    void RenderGlow();
    void UpdateAnimation(float deltaTime);
};

} // namespace ArenaFighter