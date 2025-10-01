#pragma once
#include <imgui.h>
#include <string>

namespace ArenaFighter {

// Base class for all UI elements
class UIElement {
public:
    UIElement() : m_visible(true), m_enabled(true) {}
    virtual ~UIElement() = default;
    
    // Core methods
    virtual void Update(float deltaTime) {}
    virtual void Render() = 0;
    virtual bool HandleInput(const ImVec2& mousePos, bool clicked) { return false; }
    
    // Properties
    void SetPosition(ImVec2 pos) { m_position = pos; }
    void SetSize(ImVec2 size) { m_size = size; }
    void SetVisible(bool visible) { m_visible = visible; }
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    
    ImVec2 GetPosition() const { return m_position; }
    ImVec2 GetSize() const { return m_size; }
    bool IsVisible() const { return m_visible; }
    bool IsEnabled() const { return m_enabled; }
    
    // Bounds checking
    bool Contains(ImVec2 point) const {
        return point.x >= m_position.x && point.x <= m_position.x + m_size.x &&
               point.y >= m_position.y && point.y <= m_position.y + m_size.y;
    }
    
protected:
    ImVec2 m_position;
    ImVec2 m_size;
    bool m_visible;
    bool m_enabled;
};

} // namespace ArenaFighter