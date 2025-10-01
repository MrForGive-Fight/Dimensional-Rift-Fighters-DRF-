#include "UIButton.h"

namespace ArenaFighter {

UIButton::UIButton(const std::string& id, XMFLOAT2 pos, XMFLOAT2 size, const std::string& text)
    : UIElement(id, pos, size), 
      m_text(text), 
      m_iconTexture(nullptr),
      m_normalColor(0.2f, 0.3f, 0.5f, 1.0f),
      m_hoverColor(0.3f, 0.4f, 0.6f, 1.0f),
      m_pressedColor(0.15f, 0.25f, 0.45f, 1.0f),
      m_disabledColor(0.15f, 0.15f, 0.15f, 0.5f) {}

void UIButton::render(ID3D11DeviceContext* context) {
    if (!m_visible) return;

    XMFLOAT4 renderColor;
    if (!m_enabled) renderColor = m_disabledColor;
    else if (m_state == UIState::Pressed) renderColor = m_pressedColor;
    else if (m_state == UIState::Hovered) renderColor = m_hoverColor;
    else renderColor = m_normalColor;

    // TODO: Render button background with renderColor
    // TODO: Render icon if present
    // TODO: Render text centered
}

void UIButton::onMouseMove(int x, int y) {
    if (!m_enabled) return;
    
    if (containsPoint(x, y)) {
        if (m_state != UIState::Hovered && m_state != UIState::Pressed) {
            m_state = UIState::Hovered;
        }
    } else {
        m_state = UIState::Normal;
    }
}

void UIButton::onMouseDown(int x, int y) {
    if (!m_enabled) return;
    
    if (containsPoint(x, y)) {
        m_state = UIState::Pressed;
    }
}

void UIButton::onMouseUp(int x, int y) {
    if (!m_enabled) return;
    
    if (m_state == UIState::Pressed && containsPoint(x, y)) {
        if (m_onClick) m_onClick();
        m_state = UIState::Hovered;
    } else {
        m_state = UIState::Normal;
    }
}

void UIButton::setColors(XMFLOAT4 normal, XMFLOAT4 hover, XMFLOAT4 pressed) {
    m_normalColor = normal;
    m_hoverColor = hover;
    m_pressedColor = pressed;
}

} // namespace ArenaFighter