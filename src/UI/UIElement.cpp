#include "UIElement.h"
#include <algorithm>

namespace ArenaFighter {

UIElement::UIElement(const std::string& id, XMFLOAT2 pos, XMFLOAT2 size)
    : m_id(id), m_position(pos), m_size(size),
      m_color(1.0f, 1.0f, 1.0f, 1.0f),
      m_anchor(UIAnchor::TopLeft),
      m_visible(true), m_enabled(true),
      m_state(UIState::Normal), m_parent(nullptr) {}

void UIElement::update(float deltaTime) {
    for (auto& child : m_children) {
        if (child->isVisible()) {
            child->update(deltaTime);
        }
    }
}

void UIElement::addChild(std::shared_ptr<UIElement> child) {
    child->m_parent = this;
    m_children.push_back(child);
}

void UIElement::removeChild(const std::string& childId) {
    m_children.erase(
        std::remove_if(m_children.begin(), m_children.end(),
            [&childId](const std::shared_ptr<UIElement>& child) {
                return child->getId() == childId;
            }),
        m_children.end()
    );
}

bool UIElement::containsPoint(int x, int y) const {
    return x >= m_position.x && x <= m_position.x + m_size.x &&
           y >= m_position.y && y <= m_position.y + m_size.y;
}

XMFLOAT2 UIElement::getAbsolutePosition() const {
    if (m_parent) {
        XMFLOAT2 parentPos = m_parent->getAbsolutePosition();
        return XMFLOAT2(parentPos.x + m_position.x, parentPos.y + m_position.y);
    }
    return m_position;
}

std::shared_ptr<UIElement> UIElement::findChild(const std::string& id) {
    for (auto& child : m_children) {
        if (child->getId() == id) {
            return child;
        }
    }
    return nullptr;
}

} // namespace ArenaFighter