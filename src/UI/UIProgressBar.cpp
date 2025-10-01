#include "UIProgressBar.h"

namespace ArenaFighter {

UIProgressBar::UIProgressBar(const std::string& id, XMFLOAT2 pos, XMFLOAT2 size, float maxVal)
    : UIElement(id, pos, size), 
      m_currentValue(0), 
      m_maxValue(maxVal),
      m_fillColor(0.2f, 0.8f, 0.2f, 1.0f),
      m_backgroundColor(0.2f, 0.2f, 0.2f, 0.8f),
      m_showText(true) {}

void UIProgressBar::render(ID3D11DeviceContext* context) {
    if (!m_visible) return;

    // TODO: Render background bar
    
    // Calculate fill percentage
    float fillPercentage = m_currentValue / m_maxValue;
    XMFLOAT2 fillSize(m_size.x * fillPercentage, m_size.y);
    
    // TODO: Render filled portion
    
    if (m_showText) {
        // TODO: Render percentage text
    }
}

void UIProgressBar::setValue(float value) { 
    m_currentValue = value > m_maxValue ? m_maxValue : (value < 0 ? 0 : value); 
}

} // namespace ArenaFighter