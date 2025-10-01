#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace ArenaFighter {

using namespace DirectX;

enum class UIAnchor {
    TopLeft, TopCenter, TopRight,
    MiddleLeft, Center, MiddleRight,
    BottomLeft, BottomCenter, BottomRight
};

enum class UIState {
    Normal,
    Hovered,
    Pressed,
    Disabled
};

class UIElement {
protected:
    std::string m_id;
    XMFLOAT2 m_position;
    XMFLOAT2 m_size;
    XMFLOAT4 m_color;
    UIAnchor m_anchor;
    bool m_visible;
    bool m_enabled;
    UIState m_state;
    UIElement* m_parent;
    std::vector<std::shared_ptr<UIElement>> m_children;

public:
    UIElement(const std::string& id, XMFLOAT2 pos, XMFLOAT2 size);
    virtual ~UIElement() = default;

    virtual void update(float deltaTime);
    virtual void render(ID3D11DeviceContext* context) = 0;
    virtual void onMouseMove(int x, int y) {}
    virtual void onMouseDown(int x, int y) {}
    virtual void onMouseUp(int x, int y) {}

    const std::string& getId() const { return m_id; }
    void setPosition(XMFLOAT2 pos) { m_position = pos; }
    XMFLOAT2 getPosition() const { return m_position; }
    void setSize(XMFLOAT2 s) { m_size = s; }
    XMFLOAT2 getSize() const { return m_size; }
    void setVisible(bool v) { m_visible = v; }
    bool isVisible() const { return m_visible; }
    void setEnabled(bool e) { m_enabled = e; }
    bool isEnabled() const { return m_enabled; }
    void setColor(XMFLOAT4 c) { m_color = c; }
    XMFLOAT4 getColor() const { return m_color; }
    void setAnchor(UIAnchor a) { m_anchor = a; }
    UIAnchor getAnchor() const { return m_anchor; }

    void addChild(std::shared_ptr<UIElement> child);
    void removeChild(const std::string& childId);
    bool containsPoint(int x, int y) const;
    XMFLOAT2 getAbsolutePosition() const;
    
    std::vector<std::shared_ptr<UIElement>>& getChildren() { return m_children; }
    const std::vector<std::shared_ptr<UIElement>>& getChildren() const { return m_children; }
    std::shared_ptr<UIElement> findChild(const std::string& id);
};

} // namespace ArenaFighter