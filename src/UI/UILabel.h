#pragma once

#include "UIElement.h"

namespace ArenaFighter {

class UILabel : public UIElement {
private:
    std::string m_text;
    float m_fontSize;
    XMFLOAT4 m_textColor;
    bool m_centerAlign;

public:
    UILabel(const std::string& id, XMFLOAT2 pos, const std::string& text, float fontSize = 16.0f);
    
    void render(ID3D11DeviceContext* context) override;
    
    void setText(const std::string& t) { m_text = t; }
    std::string getText() const { return m_text; }
    void setFontSize(float size) { m_fontSize = size; }
    void setTextColor(XMFLOAT4 c) { m_textColor = c; }
    void setCenterAlign(bool center) { m_centerAlign = center; }
};

} // namespace ArenaFighter