#pragma once

#include "UIElement.h"
#include <functional>

namespace ArenaFighter {

class UIButton : public UIElement {
private:
    std::string m_text;
    std::function<void()> m_onClick;
    XMFLOAT4 m_normalColor;
    XMFLOAT4 m_hoverColor;
    XMFLOAT4 m_pressedColor;
    XMFLOAT4 m_disabledColor;
    ID3D11ShaderResourceView* m_iconTexture;

public:
    UIButton(const std::string& id, XMFLOAT2 pos, XMFLOAT2 size, const std::string& text);
    
    void render(ID3D11DeviceContext* context) override;
    void onMouseMove(int x, int y) override;
    void onMouseDown(int x, int y) override;
    void onMouseUp(int x, int y) override;
    
    void setText(const std::string& t) { m_text = t; }
    std::string getText() const { return m_text; }
    void setOnClick(std::function<void()> callback) { m_onClick = callback; }
    void setIcon(ID3D11ShaderResourceView* texture) { m_iconTexture = texture; }
    void setColors(XMFLOAT4 normal, XMFLOAT4 hover, XMFLOAT4 pressed);
    void setDisabledColor(XMFLOAT4 disabled) { m_disabledColor = disabled; }
};

} // namespace ArenaFighter