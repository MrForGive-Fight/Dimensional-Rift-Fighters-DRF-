#include "UIPanel.h"

namespace ArenaFighter {

UIPanel::UIPanel(const std::string& id, XMFLOAT2 pos, XMFLOAT2 size)
    : UIElement(id, pos, size),
      m_backgroundColor(0.1f, 0.1f, 0.15f, 0.9f),
      m_hasBorder(true),
      m_borderColor(0.3f, 0.5f, 0.8f, 1.0f),
      m_borderWidth(2.0f) {}

void UIPanel::render(ID3D11DeviceContext* context) {
    if (!m_visible) return;

    // TODO: Implement DirectX rendering for background panel
    
    // Render children
    for (auto& child : m_children) {
        if (child->isVisible()) {
            child->render(context);
        }
    }
    
    // TODO: Implement border rendering if enabled
}

void UIPanel::setBorder(bool enabled, XMFLOAT4 color, float width) {
    m_hasBorder = enabled;
    m_borderColor = color;
    m_borderWidth = width;
}

} // namespace ArenaFighter