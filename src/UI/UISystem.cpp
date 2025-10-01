#include "UISystem.h"
#include "UIRenderer.h"
#include <algorithm>

namespace ArenaFighter {

// UIElement Implementation
UIElement::UIElement(const std::string& id, XMFLOAT2 pos, XMFLOAT2 size)
    : m_id(id)
    , m_position(pos)
    , m_size(size)
    , m_color(1.0f, 1.0f, 1.0f, 1.0f)
    , m_anchor(UIAnchor::TopLeft)
    , m_visible(true)
    , m_enabled(true)
    , m_state(UIState::Normal)
    , m_parent(nullptr) {
}

void UIElement::update(float deltaTime) {
    if (!m_visible || !m_enabled) return;
    
    // Update children
    for (auto& child : m_children) {
        child->update(deltaTime);
    }
}

void UIElement::onMouseMove(int x, int y) {
    if (!m_visible || !m_enabled) return;
    
    bool wasHovered = (m_state == UIState::Hovered);
    bool isHovered = containsPoint(x, y);
    
    if (isHovered && !wasHovered && m_state == UIState::Normal) {
        m_state = UIState::Hovered;
    } else if (!isHovered && wasHovered && m_state == UIState::Hovered) {
        m_state = UIState::Normal;
    }
    
    // Propagate to children
    for (auto& child : m_children) {
        child->onMouseMove(x, y);
    }
}

void UIElement::onMouseDown(int x, int y) {
    if (!m_visible || !m_enabled) return;
    
    if (containsPoint(x, y)) {
        m_state = UIState::Pressed;
    }
    
    // Propagate to children
    for (auto& child : m_children) {
        child->onMouseDown(x, y);
    }
}

void UIElement::onMouseUp(int x, int y) {
    if (!m_visible || !m_enabled) return;
    
    if (m_state == UIState::Pressed) {
        m_state = containsPoint(x, y) ? UIState::Hovered : UIState::Normal;
    }
    
    // Propagate to children
    for (auto& child : m_children) {
        child->onMouseUp(x, y);
    }
}

void UIElement::addChild(std::shared_ptr<UIElement> child) {
    child->m_parent = this;
    m_children.push_back(child);
}

void UIElement::removeChild(const std::string& childId) {
    m_children.erase(
        std::remove_if(m_children.begin(), m_children.end(),
            [&childId](const std::shared_ptr<UIElement>& elem) {
                return elem->getId() == childId;
            }),
        m_children.end()
    );
}

std::shared_ptr<UIElement> UIElement::findChild(const std::string& childId) {
    for (auto& child : m_children) {
        if (child->getId() == childId) {
            return child;
        }
        // Recursive search
        auto found = child->findChild(childId);
        if (found) return found;
    }
    return nullptr;
}

bool UIElement::containsPoint(int x, int y) const {
    XMFLOAT2 absPos = getAbsolutePosition();
    return x >= absPos.x && x <= absPos.x + m_size.x &&
           y >= absPos.y && y <= absPos.y + m_size.y;
}

XMFLOAT2 UIElement::getAbsolutePosition() const {
    XMFLOAT2 pos = m_position;
    
    if (m_parent) {
        XMFLOAT2 parentPos = m_parent->getAbsolutePosition();
        XMFLOAT2 anchoredPos = getAnchoredPosition(m_parent->getSize().x, m_parent->getSize().y);
        pos.x = parentPos.x + anchoredPos.x;
        pos.y = parentPos.y + anchoredPos.y;
    }
    
    return pos;
}

XMFLOAT2 UIElement::getAnchoredPosition(float parentWidth, float parentHeight) const {
    XMFLOAT2 anchoredPos = m_position;
    
    switch (m_anchor) {
        case UIAnchor::TopCenter:
            anchoredPos.x += (parentWidth - m_size.x) * 0.5f;
            break;
        case UIAnchor::TopRight:
            anchoredPos.x += parentWidth - m_size.x;
            break;
        case UIAnchor::MiddleLeft:
            anchoredPos.y += (parentHeight - m_size.y) * 0.5f;
            break;
        case UIAnchor::Center:
            anchoredPos.x += (parentWidth - m_size.x) * 0.5f;
            anchoredPos.y += (parentHeight - m_size.y) * 0.5f;
            break;
        case UIAnchor::MiddleRight:
            anchoredPos.x += parentWidth - m_size.x;
            anchoredPos.y += (parentHeight - m_size.y) * 0.5f;
            break;
        case UIAnchor::BottomLeft:
            anchoredPos.y += parentHeight - m_size.y;
            break;
        case UIAnchor::BottomCenter:
            anchoredPos.x += (parentWidth - m_size.x) * 0.5f;
            anchoredPos.y += parentHeight - m_size.y;
            break;
        case UIAnchor::BottomRight:
            anchoredPos.x += parentWidth - m_size.x;
            anchoredPos.y += parentHeight - m_size.y;
            break;
    }
    
    return anchoredPos;
}

// UIPanel Implementation
UIPanel::UIPanel(const std::string& id, XMFLOAT2 pos, XMFLOAT2 size)
    : UIElement(id, pos, size)
    , m_backgroundColor(0.2f, 0.2f, 0.2f, 0.9f)
    , m_hasBorder(false)
    , m_borderColor(1.0f, 1.0f, 1.0f, 1.0f)
    , m_borderWidth(2.0f)
    , m_cornerRadius(0.0f) {
}

void UIPanel::render(UIRenderer* renderer) {
    if (!m_visible) return;
    
    XMFLOAT2 absPos = getAbsolutePosition();
    
    // Render background
    if (m_cornerRadius > 0) {
        renderer->renderRoundedRect(absPos, m_size, m_backgroundColor, m_cornerRadius);
    } else {
        renderer->renderRect(absPos, m_size, m_backgroundColor);
    }
    
    // Render border
    if (m_hasBorder) {
        if (m_cornerRadius > 0) {
            renderer->renderRoundedRectOutline(absPos, m_size, m_borderColor, m_borderWidth, m_cornerRadius);
        } else {
            renderer->renderRectOutline(absPos, m_size, m_borderColor, m_borderWidth);
        }
    }
    
    // Render children
    for (auto& child : m_children) {
        child->render(renderer);
    }
}

void UIPanel::setBorder(bool enabled, XMFLOAT4 color, float width) {
    m_hasBorder = enabled;
    m_borderColor = color;
    m_borderWidth = width;
}

// UIButton Implementation
UIButton::UIButton(const std::string& id, XMFLOAT2 pos, XMFLOAT2 size, const std::string& text)
    : UIElement(id, pos, size)
    , m_text(text)
    , m_normalColor(0.3f, 0.3f, 0.3f, 1.0f)
    , m_hoverColor(0.4f, 0.4f, 0.4f, 1.0f)
    , m_pressedColor(0.2f, 0.2f, 0.2f, 1.0f)
    , m_disabledColor(0.15f, 0.15f, 0.15f, 0.7f)
    , m_iconTexture(nullptr)
    , m_fontSize(16.0f) {
}

void UIButton::render(UIRenderer* renderer) {
    if (!m_visible) return;
    
    XMFLOAT2 absPos = getAbsolutePosition();
    
    // Select color based on state
    XMFLOAT4 currentColor = m_normalColor;
    if (!m_enabled) {
        currentColor = m_disabledColor;
    } else {
        switch (m_state) {
            case UIState::Hovered:
                currentColor = m_hoverColor;
                break;
            case UIState::Pressed:
                currentColor = m_pressedColor;
                break;
        }
    }
    
    // Render button background
    renderer->renderRect(absPos, m_size, currentColor);
    renderer->renderRectOutline(absPos, m_size, XMFLOAT4(1, 1, 1, 0.5f), 1.0f);
    
    // Render icon if available
    if (m_iconTexture) {
        float iconSize = m_size.y * 0.7f;
        XMFLOAT2 iconPos = absPos;
        iconPos.x += 10;
        iconPos.y += (m_size.y - iconSize) * 0.5f;
        renderer->renderTexture(m_iconTexture, iconPos, XMFLOAT2(iconSize, iconSize));
    }
    
    // Render text
    if (!m_text.empty()) {
        XMFLOAT4 textColor = m_enabled ? XMFLOAT4(1, 1, 1, 1) : XMFLOAT4(0.5f, 0.5f, 0.5f, 1);
        float textX = absPos.x + m_size.x * 0.5f;
        if (m_iconTexture) {
            textX += m_size.y * 0.35f;
        }
        renderer->renderTextCentered(m_text, XMFLOAT2(textX, absPos.y + m_size.y * 0.5f), textColor, m_fontSize);
    }
    
    // Render children
    for (auto& child : m_children) {
        child->render(renderer);
    }
}

void UIButton::onMouseMove(int x, int y) {
    UIElement::onMouseMove(x, y);
}

void UIButton::onMouseDown(int x, int y) {
    UIElement::onMouseDown(x, y);
}

void UIButton::onMouseUp(int x, int y) {
    bool wasPressed = (m_state == UIState::Pressed);
    UIElement::onMouseUp(x, y);
    
    // Trigger click if button was released while hovered
    if (wasPressed && m_state == UIState::Hovered && m_onClick && m_enabled) {
        m_onClick();
    }
}

void UIButton::setColors(XMFLOAT4 normal, XMFLOAT4 hover, XMFLOAT4 pressed, XMFLOAT4 disabled) {
    m_normalColor = normal;
    m_hoverColor = hover;
    m_pressedColor = pressed;
    m_disabledColor = disabled;
}

// UILabel Implementation
UILabel::UILabel(const std::string& id, XMFLOAT2 pos, const std::string& text, float fontSize)
    : UIElement(id, pos, XMFLOAT2(0, 0))
    , m_text(text)
    , m_fontSize(fontSize)
    , m_textColor(1.0f, 1.0f, 1.0f, 1.0f)
    , m_centerAlign(false)
    , m_rightAlign(false)
    , m_wordWrap(false) {
    // Size will be calculated based on text
}

void UILabel::render(UIRenderer* renderer) {
    if (!m_visible || m_text.empty()) return;
    
    XMFLOAT2 absPos = getAbsolutePosition();
    
    if (m_wordWrap && m_size.x > 0) {
        renderer->renderTextWrapped(m_text, absPos, m_size.x, m_textColor, m_fontSize);
    } else {
        if (m_centerAlign) {
            renderer->renderTextCentered(m_text, absPos, m_textColor, m_fontSize);
        } else if (m_rightAlign) {
            renderer->renderTextRightAligned(m_text, absPos, m_textColor, m_fontSize);
        } else {
            renderer->renderText(m_text, absPos, m_textColor, m_fontSize);
        }
    }
    
    // Render children
    for (auto& child : m_children) {
        child->render(renderer);
    }
}

// UIProgressBar Implementation
UIProgressBar::UIProgressBar(const std::string& id, XMFLOAT2 pos, XMFLOAT2 size, float maxVal)
    : UIElement(id, pos, size)
    , m_currentValue(0.0f)
    , m_maxValue(maxVal)
    , m_fillColor(0.2f, 0.8f, 0.2f, 1.0f)
    , m_backgroundColor(0.1f, 0.1f, 0.1f, 1.0f)
    , m_showText(false)
    , m_smoothTransition(false)
    , m_transitionSpeed(2.0f)
    , m_targetValue(0.0f) {
}

void UIProgressBar::update(float deltaTime) {
    UIElement::update(deltaTime);
    
    // Smooth transition
    if (m_smoothTransition && m_currentValue != m_targetValue) {
        float diff = m_targetValue - m_currentValue;
        float change = diff * m_transitionSpeed * deltaTime;
        
        if (abs(diff) < 0.01f) {
            m_currentValue = m_targetValue;
        } else {
            m_currentValue += change;
        }
    }
}

void UIProgressBar::render(UIRenderer* renderer) {
    if (!m_visible) return;
    
    XMFLOAT2 absPos = getAbsolutePosition();
    
    // Render background
    renderer->renderRect(absPos, m_size, m_backgroundColor);
    
    // Render fill
    float fillWidth = m_size.x * getPercentage();
    if (fillWidth > 0) {
        renderer->renderRect(absPos, XMFLOAT2(fillWidth, m_size.y), m_fillColor);
    }
    
    // Render border
    renderer->renderRectOutline(absPos, m_size, XMFLOAT4(0.5f, 0.5f, 0.5f, 1), 1.0f);
    
    // Render text
    if (m_showText) {
        char buffer[32];
        sprintf_s(buffer, "%.0f%%", getPercentage() * 100);
        XMFLOAT2 textPos(absPos.x + m_size.x * 0.5f, absPos.y + m_size.y * 0.5f);
        renderer->renderTextCentered(buffer, textPos, XMFLOAT4(1, 1, 1, 1), 14.0f);
    }
    
    // Render children
    for (auto& child : m_children) {
        child->render(renderer);
    }
}

void UIProgressBar::setValue(float value) {
    m_targetValue = std::max(0.0f, std::min(value, m_maxValue));
    if (!m_smoothTransition) {
        m_currentValue = m_targetValue;
    }
}

void UIProgressBar::setSmoothTransition(bool smooth, float speed) {
    m_smoothTransition = smooth;
    m_transitionSpeed = speed;
}

// UIImage Implementation
UIImage::UIImage(const std::string& id, XMFLOAT2 pos, XMFLOAT2 size, ID3D11ShaderResourceView* texture)
    : UIElement(id, pos, size)
    , m_texture(texture)
    , m_tint(1.0f, 1.0f, 1.0f, 1.0f)
    , m_maintainAspectRatio(false) {
}

void UIImage::render(UIRenderer* renderer) {
    if (!m_visible || !m_texture) return;
    
    XMFLOAT2 absPos = getAbsolutePosition();
    renderer->renderTexture(m_texture, absPos, m_size, m_tint);
    
    // Render children
    for (auto& child : m_children) {
        child->render(renderer);
    }
}

// UIManager Implementation
UIManager::UIManager(ID3D11Device* device, ID3D11DeviceContext* context)
    : m_screenWidth(1920)
    , m_screenHeight(1080) {
    m_renderer = std::make_unique<UIRenderer>(device, context);
}

UIManager::~UIManager() = default;

void UIManager::addScreen(const std::string& name, std::shared_ptr<UIPanel> screen) {
    m_screens[name] = screen;
}

void UIManager::showScreen(const std::string& name) {
    auto it = m_screens.find(name);
    if (it != m_screens.end()) {
        m_currentScreen = name;
        it->second->setVisible(true);
    }
}

void UIManager::hideScreen(const std::string& name) {
    auto it = m_screens.find(name);
    if (it != m_screens.end()) {
        it->second->setVisible(false);
        if (m_currentScreen == name) {
            m_currentScreen.clear();
        }
    }
}

void UIManager::update(float deltaTime) {
    for (auto& [name, screen] : m_screens) {
        if (screen->isVisible()) {
            screen->update(deltaTime);
        }
    }
}

void UIManager::render() {
    m_renderer->begin();
    
    for (auto& [name, screen] : m_screens) {
        if (screen->isVisible()) {
            screen->render(m_renderer.get());
        }
    }
    
    m_renderer->end();
}

void UIManager::handleMouseMove(int x, int y) {
    for (auto& [name, screen] : m_screens) {
        if (screen->isVisible()) {
            screen->onMouseMove(x, y);
        }
    }
}

void UIManager::handleMouseDown(int x, int y) {
    for (auto& [name, screen] : m_screens) {
        if (screen->isVisible()) {
            screen->onMouseDown(x, y);
        }
    }
}

void UIManager::handleMouseUp(int x, int y) {
    for (auto& [name, screen] : m_screens) {
        if (screen->isVisible()) {
            screen->onMouseUp(x, y);
        }
    }
}

void UIManager::handleKeyDown(int key) {
    // Key handling can be implemented as needed
}

void UIManager::handleKeyUp(int key) {
    // Key handling can be implemented as needed
}

void UIManager::setScreenSize(int width, int height) {
    m_screenWidth = width;
    m_screenHeight = height;
    m_renderer->setViewportSize(width, height);
}

std::shared_ptr<UIPanel> UIManager::getScreen(const std::string& name) {
    auto it = m_screens.find(name);
    return it != m_screens.end() ? it->second : nullptr;
}

std::shared_ptr<UIPanel> UIManager::getCurrentScreen() {
    return getScreen(m_currentScreen);
}

} // namespace ArenaFighter