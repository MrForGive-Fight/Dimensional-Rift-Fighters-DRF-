#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

using namespace DirectX;

namespace ArenaFighter {

// Forward declarations
class UIElement;
class UIPanel;
class UIButton;
class UILabel;
class UIImage;
class UIProgressBar;
class UIRenderer;

// UI Anchor points
enum class UIAnchor {
    TopLeft,
    TopCenter,
    TopRight,
    MiddleLeft,
    Center,
    MiddleRight,
    BottomLeft,
    BottomCenter,
    BottomRight
};

// UI State
enum class UIState {
    Normal,
    Hovered,
    Pressed,
    Disabled
};

// Base UI Element
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
    
    // Core methods
    virtual void update(float deltaTime);
    virtual void render(UIRenderer* renderer) = 0;
    virtual void onMouseMove(int x, int y);
    virtual void onMouseDown(int x, int y);
    virtual void onMouseUp(int x, int y);
    
    // Getters/Setters
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
    void setAnchor(UIAnchor anchor) { m_anchor = anchor; }
    UIAnchor getAnchor() const { return m_anchor; }
    
    // Children management
    void addChild(std::shared_ptr<UIElement> child);
    void removeChild(const std::string& childId);
    std::shared_ptr<UIElement> findChild(const std::string& childId);
    
    // Utility
    bool containsPoint(int x, int y) const;
    XMFLOAT2 getAbsolutePosition() const;
    XMFLOAT2 getAnchoredPosition(float parentWidth, float parentHeight) const;
};

// UI Panel - Container for other elements
class UIPanel : public UIElement {
private:
    XMFLOAT4 m_backgroundColor;
    bool m_hasBorder;
    XMFLOAT4 m_borderColor;
    float m_borderWidth;
    float m_cornerRadius;
    
public:
    UIPanel(const std::string& id, XMFLOAT2 pos, XMFLOAT2 size);
    
    void render(UIRenderer* renderer) override;
    
    void setBackgroundColor(XMFLOAT4 color) { m_backgroundColor = color; }
    XMFLOAT4 getBackgroundColor() const { return m_backgroundColor; }
    void setBorder(bool enabled, XMFLOAT4 color = XMFLOAT4(1,1,1,1), float width = 2.0f);
    void setCornerRadius(float radius) { m_cornerRadius = radius; }
};

// UI Button
class UIButton : public UIElement {
private:
    std::string m_text;
    std::function<void()> m_onClick;
    XMFLOAT4 m_normalColor;
    XMFLOAT4 m_hoverColor;
    XMFLOAT4 m_pressedColor;
    XMFLOAT4 m_disabledColor;
    ID3D11ShaderResourceView* m_iconTexture;
    float m_fontSize;
    
public:
    UIButton(const std::string& id, XMFLOAT2 pos, XMFLOAT2 size, const std::string& text);
    
    void render(UIRenderer* renderer) override;
    void onMouseMove(int x, int y) override;
    void onMouseDown(int x, int y) override;
    void onMouseUp(int x, int y) override;
    
    void setText(const std::string& t) { m_text = t; }
    std::string getText() const { return m_text; }
    void setOnClick(std::function<void()> callback) { m_onClick = callback; }
    void setIcon(ID3D11ShaderResourceView* texture) { m_iconTexture = texture; }
    void setColors(XMFLOAT4 normal, XMFLOAT4 hover, XMFLOAT4 pressed, XMFLOAT4 disabled);
    void setFontSize(float size) { m_fontSize = size; }
};

// UI Label
class UILabel : public UIElement {
private:
    std::string m_text;
    float m_fontSize;
    XMFLOAT4 m_textColor;
    bool m_centerAlign;
    bool m_rightAlign;
    bool m_wordWrap;
    
public:
    UILabel(const std::string& id, XMFLOAT2 pos, const std::string& text, float fontSize = 16.0f);
    
    void render(UIRenderer* renderer) override;
    
    void setText(const std::string& t) { m_text = t; }
    std::string getText() const { return m_text; }
    void setFontSize(float size) { m_fontSize = size; }
    void setTextColor(XMFLOAT4 c) { m_textColor = c; }
    void setAlignment(bool center, bool right = false) { m_centerAlign = center; m_rightAlign = right; }
    void setWordWrap(bool wrap) { m_wordWrap = wrap; }
};

// UI Progress Bar
class UIProgressBar : public UIElement {
private:
    float m_currentValue;
    float m_maxValue;
    XMFLOAT4 m_fillColor;
    XMFLOAT4 m_backgroundColor;
    bool m_showText;
    bool m_smoothTransition;
    float m_transitionSpeed;
    float m_targetValue;
    
public:
    UIProgressBar(const std::string& id, XMFLOAT2 pos, XMFLOAT2 size, float maxVal = 100.0f);
    
    void update(float deltaTime) override;
    void render(UIRenderer* renderer) override;
    
    void setValue(float value);
    void setMaxValue(float value) { m_maxValue = value; }
    float getValue() const { return m_currentValue; }
    float getPercentage() const { return m_currentValue / m_maxValue; }
    void setFillColor(XMFLOAT4 c) { m_fillColor = c; }
    void setBackgroundColor(XMFLOAT4 c) { m_backgroundColor = c; }
    void setShowText(bool show) { m_showText = show; }
    void setSmoothTransition(bool smooth, float speed = 2.0f);
};

// UI Image
class UIImage : public UIElement {
private:
    ID3D11ShaderResourceView* m_texture;
    XMFLOAT4 m_tint;
    bool m_maintainAspectRatio;
    
public:
    UIImage(const std::string& id, XMFLOAT2 pos, XMFLOAT2 size, ID3D11ShaderResourceView* texture = nullptr);
    
    void render(UIRenderer* renderer) override;
    
    void setTexture(ID3D11ShaderResourceView* texture) { m_texture = texture; }
    void setTint(XMFLOAT4 tint) { m_tint = tint; }
    void setMaintainAspectRatio(bool maintain) { m_maintainAspectRatio = maintain; }
};

// UI Manager - Manages all UI screens
class UIManager {
private:
    std::unordered_map<std::string, std::shared_ptr<UIPanel>> m_screens;
    std::string m_currentScreen;
    std::unique_ptr<UIRenderer> m_renderer;
    int m_screenWidth;
    int m_screenHeight;
    
public:
    UIManager(ID3D11Device* device, ID3D11DeviceContext* context);
    ~UIManager();
    
    void addScreen(const std::string& name, std::shared_ptr<UIPanel> screen);
    void showScreen(const std::string& name);
    void hideScreen(const std::string& name);
    
    void update(float deltaTime);
    void render();
    
    void handleMouseMove(int x, int y);
    void handleMouseDown(int x, int y);
    void handleMouseUp(int x, int y);
    void handleKeyDown(int key);
    void handleKeyUp(int key);
    
    void setScreenSize(int width, int height);
    std::shared_ptr<UIPanel> getScreen(const std::string& name);
    std::shared_ptr<UIPanel> getCurrentScreen();
    
    UIRenderer* getRenderer() { return m_renderer.get(); }
};

} // namespace ArenaFighter