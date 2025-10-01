#include "CombatHUD.h"
#include "CombatHUDConfig.h"
#include <algorithm>

namespace ArenaFighter {

// Observer notification methods
void CombatHUD::notifyHealthChanged(float oldValue, float newValue) {
    for (auto it = m_observers.begin(); it != m_observers.end();) {
        if (auto observer = it->lock()) {
            observer->onHealthChanged(oldValue, newValue);
            ++it;
        } else {
            it = m_observers.erase(it);
        }
    }
}

void CombatHUD::notifyManaChanged(float oldValue, float newValue) {
    for (auto it = m_observers.begin(); it != m_observers.end();) {
        if (auto observer = it->lock()) {
            observer->onManaChanged(oldValue, newValue);
            ++it;
        } else {
            it = m_observers.erase(it);
        }
    }
}

void CombatHUD::notifyQiChanged(float oldValue, float newValue) {
    for (auto& weakObserver : m_observers) {
        if (auto observer = weakObserver.lock()) {
            observer->onQiChanged(oldValue, newValue);
        }
    }
}

void CombatHUD::notifyComboChanged(int oldValue, int newValue) {
    for (auto& weakObserver : m_observers) {
        if (auto observer = weakObserver.lock()) {
            observer->onComboChanged(oldValue, newValue);
        }
    }
}

// Error handling
void CombatHUD::reportError(const std::string& error) {
    if (m_errorCallback) {
        m_errorCallback("CombatHUD: " + error);
    }
}

bool CombatHUD::validateSkillIndex(int index) const {
    return index >= 0 && index < static_cast<int>(m_specialSkills.size());
}

// Resource management
void CombatHUD::releaseResources() {
    // Clean up any DirectX resources if needed
    m_device.Reset();
    m_context.Reset();
    
    // Clear UI elements
    m_rootPanel.reset();
    m_healthBar.reset();
    m_manaBar.reset();
    m_qiBar.reset();
    m_comboLabel.reset();
    m_scoreLabel.reset();
    m_stanceIndicator.reset();
    m_skillsPanel.reset();
    m_gearSkillsPanel.reset();
    
    // Clear observers
    m_observers.clear();
}

// Position helpers
XMFLOAT2 CombatHUD::getAnchoredPosition(Anchor anchor, float offsetX, float offsetY) const {
    XMFLOAT2 pos(0, 0);
    
    switch (anchor) {
        case Anchor::TopLeft:
            pos = XMFLOAT2(offsetX, offsetY);
            break;
        case Anchor::TopCenter:
            pos = XMFLOAT2(m_screenSize.x / 2.0f + offsetX, offsetY);
            break;
        case Anchor::TopRight:
            pos = XMFLOAT2(m_screenSize.x - offsetX, offsetY);
            break;
        case Anchor::BottomLeft:
            pos = XMFLOAT2(offsetX, m_screenSize.y - offsetY);
            break;
        case Anchor::BottomCenter:
            pos = XMFLOAT2(m_screenSize.x / 2.0f + offsetX, m_screenSize.y - offsetY);
            break;
        case Anchor::BottomRight:
            pos = XMFLOAT2(m_screenSize.x - offsetX, m_screenSize.y - offsetY);
            break;
        case Anchor::Center:
            pos = XMFLOAT2(m_screenSize.x / 2.0f + offsetX, m_screenSize.y / 2.0f + offsetY);
            break;
        case Anchor::MiddleLeft:
            pos = XMFLOAT2(offsetX, m_screenSize.y / 2.0f + offsetY);
            break;
        case Anchor::MiddleRight:
            pos = XMFLOAT2(m_screenSize.x - offsetX, m_screenSize.y / 2.0f + offsetY);
            break;
        default:
            break;
    }
    
    return pos;
}

float CombatHUD::getScaledValue(float value) const {
    return value * m_uiScale;
}

// Template specializations
template<>
std::shared_ptr<UILabel> CombatHUD::findChildRecursive<UILabel>(std::shared_ptr<UIPanel> parent, const std::string& id) {
    if (!parent) return nullptr;
    
    auto child = parent->findChild(id);
    if (auto typed = std::dynamic_pointer_cast<UILabel>(child)) {
        return typed;
    }
    
    // Search in children recursively
    for (const auto& child : parent->getChildren()) {
        if (auto panel = std::dynamic_pointer_cast<UIPanel>(child)) {
            if (auto found = findChildRecursive<UILabel>(panel, id)) {
                return found;
            }
        }
    }
    
    return nullptr;
}

template<>
std::shared_ptr<UIProgressBar> CombatHUD::findChildRecursive<UIProgressBar>(std::shared_ptr<UIPanel> parent, const std::string& id) {
    if (!parent) return nullptr;
    
    auto child = parent->findChild(id);
    if (auto typed = std::dynamic_pointer_cast<UIProgressBar>(child)) {
        return typed;
    }
    
    // Search in children recursively
    for (const auto& child : parent->getChildren()) {
        if (auto panel = std::dynamic_pointer_cast<UIPanel>(child)) {
            if (auto found = findChildRecursive<UIProgressBar>(panel, id)) {
                return found;
            }
        }
    }
    
    return nullptr;
}

template<>
std::shared_ptr<UIPanel> CombatHUD::findChildRecursive<UIPanel>(std::shared_ptr<UIPanel> parent, const std::string& id) {
    if (!parent) return nullptr;
    
    auto child = parent->findChild(id);
    if (auto typed = std::dynamic_pointer_cast<UIPanel>(child)) {
        return typed;
    }
    
    // Search in children recursively
    for (const auto& child : parent->getChildren()) {
        if (auto panel = std::dynamic_pointer_cast<UIPanel>(child)) {
            if (auto found = findChildRecursive<UIPanel>(panel, id)) {
                return found;
            }
        }
    }
    
    return nullptr;
}

} // namespace ArenaFighter