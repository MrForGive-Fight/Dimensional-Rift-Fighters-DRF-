#include "UILabel.h"

namespace ArenaFighter {

UILabel::UILabel(const std::string& id, XMFLOAT2 pos, const std::string& text, float fontSize)
    : UIElement(id, pos, XMFLOAT2(0, 0)), 
      m_text(text), 
      m_fontSize(fontSize),
      m_textColor(1.0f, 1.0f, 1.0f, 1.0f), 
      m_centerAlign(false) {}

void UILabel::render(ID3D11DeviceContext* context) {
    if (!m_visible) return;
    
    // TODO: Render text at position with fontSize and textColor
}

} // namespace ArenaFighter