#pragma once

#include "UIElement.h"

namespace ArenaFighter {

class UIPanel : public UIElement {
private:
    XMFLOAT4 m_backgroundColor;
    bool m_hasBorder;
    XMFLOAT4 m_borderColor;
    float m_borderWidth;

public:
    UIPanel(const std::string& id, XMFLOAT2 pos, XMFLOAT2 size);
    
    void render(ID3D11DeviceContext* context) override;
    
    void setBackgroundColor(XMFLOAT4 color) { m_backgroundColor = color; }
    XMFLOAT4 getBackgroundColor() const { return m_backgroundColor; }
    void setBorder(bool enabled, XMFLOAT4 color = XMFLOAT4(1,1,1,1), float width = 2.0f);
    bool hasBorder() const { return m_hasBorder; }
    void clearChildren() { m_children.clear(); }
};

} // namespace ArenaFighter