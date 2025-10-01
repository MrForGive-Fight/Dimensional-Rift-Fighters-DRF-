#pragma once
#include "../Core/UITheme.h"
#include "UIElement.h"
#include <functional>

namespace ArenaFighter {

class Slider : public UIElement {
public:
    Slider(float minValue, float maxValue, float* value);
    virtual ~Slider() = default;
    
    // Configuration
    void SetRange(float min, float max) { m_minValue = min; m_maxValue = max; }
    void SetValue(float value);
    void SetStep(float step) { m_step = step; }
    void SetShowValue(bool show) { m_showValue = show; }
    void SetValueFormat(const std::string& format) { m_valueFormat = format; }
    
    // Style
    void SetTrackColor(const UIStyle::Color& color) { m_trackColor = color; }
    void SetFillColor(const UIStyle::Color& color) { m_fillColor = color; }
    void SetThumbColor(const UIStyle::Color& color) { m_thumbColor = color; }
    void SetThumbSize(float size) { m_thumbSize = size; }
    
    // Callbacks
    void SetOnChange(std::function<void(float)> callback) { m_onChange = callback; }
    
    // Core methods
    void Update(float deltaTime) override;
    void Render() override;
    bool HandleInput(const ImVec2& mousePos, bool clicked) override;
    
protected:
    float m_minValue;
    float m_maxValue;
    float* m_value;
    float m_step;
    bool m_showValue;
    std::string m_valueFormat;
    
    // Visual state
    bool m_isDragging;
    bool m_isHovered;
    float m_thumbPosition;
    float m_animatedThumbPos;
    
    // Style
    UIStyle::Color m_trackColor;
    UIStyle::Color m_fillColor;
    UIStyle::Color m_thumbColor;
    float m_thumbSize;
    float m_trackHeight;
    
    // Callback
    std::function<void(float)> m_onChange;
    
    // Internal methods
    float ValueToPosition(float value) const;
    float PositionToValue(float pos) const;
    void UpdateThumbPosition();
};

// Specialized volume slider with mute button
class VolumeSlider : public Slider {
public:
    VolumeSlider(float* value);
    
    void SetMuted(bool muted) { m_isMuted = muted; }
    bool IsMuted() const { return m_isMuted; }
    
    void Render() override;
    
private:
    bool m_isMuted;
    float m_previousValue;
    
    void RenderMuteButton();
    void ToggleMute();
};

} // namespace ArenaFighter