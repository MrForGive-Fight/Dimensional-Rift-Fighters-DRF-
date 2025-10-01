#pragma once

#include "UIElement.h"

namespace ArenaFighter {

class UIProgressBar : public UIElement {
private:
    float m_currentValue;
    float m_maxValue;
    XMFLOAT4 m_fillColor;
    XMFLOAT4 m_backgroundColor;
    bool m_showText;

public:
    UIProgressBar(const std::string& id, XMFLOAT2 pos, XMFLOAT2 size, float maxVal = 100.0f);
    
    void render(ID3D11DeviceContext* context) override;
    
    void setValue(float value);
    void setMaxValue(float value) { m_maxValue = value; }
    float getValue() const { return m_currentValue; }
    float getPercentage() const { return m_currentValue / m_maxValue; }
    void setFillColor(XMFLOAT4 c) { m_fillColor = c; }
    void setBackgroundColor(XMFLOAT4 c) { m_backgroundColor = c; }
    void setShowText(bool show) { m_showText = show; }
};

} // namespace ArenaFighter