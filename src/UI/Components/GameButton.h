#pragma once
#include <imgui.h>
#include "../Core/UIStyle.h"
#include <string>
#include <functional>

namespace ArenaFighter {

enum class ButtonState {
    Normal,
    Hovered,
    Pressed,
    Selected,
    Disabled
};

enum class ButtonStyle {
    Primary,    // Red style for main actions
    Secondary,  // Gold style for secondary actions
    Danger,     // Dark red for dangerous actions
    Success,    // Green for confirmations
    Info,       // Blue for information
    Ghost       // Transparent with border
};

class GameButton {
public:
    GameButton(const std::string& label, ImVec2 size = ImVec2(UIStyle::BUTTON_WIDTH, UIStyle::BUTTON_HEIGHT));
    ~GameButton();
    
    // Main render function - returns true if clicked
    bool Render(ImVec2 position);
    
    // Style configuration
    void SetStyle(ButtonStyle style);
    void SetCustomColors(const UIStyle::Color& normal, const UIStyle::Color& hover, const UIStyle::Color& active);
    void SetTextColor(const UIStyle::Color& color) { m_textColor = color; }
    void SetBorderColor(const UIStyle::Color& color) { m_borderColor = color; }
    void SetBorderWidth(float width) { m_borderWidth = width; }
    
    // Icon support
    void SetIcon(const std::string& iconPath);
    void SetIconPosition(bool leftSide) { m_iconOnLeft = leftSide; }
    void SetIconSize(ImVec2 size) { m_iconSize = size; }
    
    // Label and tooltip
    void SetLabel(const std::string& label) { m_label = label; }
    void SetTooltip(const std::string& tooltip) { m_tooltip = tooltip; }
    void SetFont(ImFont* font) { m_font = font; }
    
    // Size and shape
    void SetSize(ImVec2 size) { m_size = size; }
    void SetCornerRadius(float radius) { m_cornerRadius = radius; }
    ImVec2 GetSize() const { return m_currentSize; }
    
    // State management
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    void SetSelected(bool selected) { m_selected = selected; }
    bool IsEnabled() const { return m_enabled; }
    bool IsSelected() const { return m_selected; }
    ButtonState GetState() const { return m_state; }
    
    // Effects
    void EnableGlow(bool enable) { m_glowEnabled = enable; }
    void SetGlowColor(const UIStyle::Color& color) { m_glowColor = color; }
    void SetHoverScale(float scale) { m_hoverScale = scale; }
    void EnablePressAnimation(bool enable) { m_pressAnimationEnabled = enable; }
    
    // Callbacks
    void SetOnClick(std::function<void()> callback) { m_onClick = callback; }
    void SetOnHover(std::function<void()> callback) { m_onHover = callback; }
    
    // Update
    void Update(float deltaTime);
    
private:
    std::string m_label;
    std::string m_tooltip;
    ImVec2 m_size;
    ImVec2 m_currentSize;
    float m_cornerRadius;
    
    // Style
    ButtonStyle m_style;
    UIStyle::Color m_normalColor;
    UIStyle::Color m_hoverColor;
    UIStyle::Color m_activeColor;
    UIStyle::Color m_textColor;
    UIStyle::Color m_borderColor;
    float m_borderWidth;
    ImFont* m_font;
    
    // Icon
    void* m_iconTexture;
    std::string m_iconPath;
    ImVec2 m_iconSize;
    bool m_iconOnLeft;
    bool m_hasIcon;
    
    // State
    ButtonState m_state;
    ButtonState m_previousState;
    bool m_enabled;
    bool m_selected;
    
    // Effects
    bool m_glowEnabled;
    UIStyle::Color m_glowColor;
    float m_glowIntensity;
    float m_hoverScale;
    bool m_pressAnimationEnabled;
    
    // Animation
    float m_animationTimer;
    float m_scaleAnimation;
    float m_glowAnimation;
    float m_pressAnimation;
    
    // Callbacks
    std::function<void()> m_onClick;
    std::function<void()> m_onHover;
    
    // Internal methods
    void UpdateState(ImVec2 position);
    void UpdateAnimations(float deltaTime);
    void ApplyStyle();
    ImVec4 GetCurrentColor() const;
    void RenderBackground(ImVec2 position);
    void RenderIcon(ImVec2 position);
    void RenderLabel(ImVec2 position);
    void RenderGlow(ImVec2 position);
    void HandleTooltip();
};

// Helper function for quick button creation
inline bool QuickButton(const std::string& label, ButtonStyle style = ButtonStyle::Primary, 
                       ImVec2 size = ImVec2(UIStyle::BUTTON_WIDTH, UIStyle::BUTTON_HEIGHT)) {
    static std::unordered_map<std::string, std::unique_ptr<GameButton>> buttonCache;
    
    if (buttonCache.find(label) == buttonCache.end()) {
        buttonCache[label] = std::make_unique<GameButton>(label, size);
        buttonCache[label]->SetStyle(style);
    }
    
    return buttonCache[label]->Render(ImGui::GetCursorScreenPos());
}

} // namespace ArenaFighter