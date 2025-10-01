#pragma once
#include "../Core/UITheme.h"
#include "UIPanel.h"
#include <memory>
#include <vector>
#include <string>

namespace ArenaFighter {

class TabbedPanel : public UIPanel {
public:
    enum TabPosition {
        TOP,
        LEFT,
        BOTTOM,
        RIGHT
    };
    
    struct Tab {
        std::string label;
        void* icon;
        std::unique_ptr<UIPanel> content;
        bool enabled;
        float animationProgress;
    };
    
    TabbedPanel(const std::string& id, ImVec2 position, ImVec2 size);
    ~TabbedPanel();
    
    // Tab management
    void AddTab(const std::string& label, void* icon = nullptr);
    void SetTabContent(int index, std::unique_ptr<UIPanel> content);
    void SelectTab(int index);
    void RemoveTab(int index);
    void EnableTab(int index, bool enabled);
    
    // Configuration
    void SetTabPosition(TabPosition position) { m_tabPosition = position; }
    void SetTabSize(ImVec2 size) { m_tabSize = size; }
    void SetShowIcons(bool show) { m_showIcons = show; }
    void SetTabSpacing(float spacing) { m_tabSpacing = spacing; }
    
    // Styling
    void SetActiveTabColor(const UIStyle::Color& color) { m_activeTabColor = color; }
    void SetInactiveTabColor(const UIStyle::Color& color) { m_inactiveTabColor = color; }
    void SetTabBorderColor(const UIStyle::Color& color) { m_tabBorderColor = color; }
    
    // Override render
    void Render();
    void Update(float deltaTime) override;
    
    // Getters
    int GetActiveTab() const { return m_activeTabIndex; }
    Tab* GetTab(int index);
    
private:
    std::vector<std::unique_ptr<Tab>> m_tabs;
    int m_activeTabIndex;
    int m_hoveredTabIndex;
    TabPosition m_tabPosition;
    ImVec2 m_tabSize;
    float m_tabSpacing;
    bool m_showIcons;
    
    // Colors
    UIStyle::Color m_activeTabColor;
    UIStyle::Color m_inactiveTabColor;
    UIStyle::Color m_tabBorderColor;
    UIStyle::Color m_tabHoverColor;
    
    // Animation
    float m_tabAnimationSpeed;
    
    // Internal methods
    void RenderTabs();
    void RenderTabContent();
    ImVec2 GetTabPosition(int index) const;
    ImVec2 GetContentAreaPosition() const;
    ImVec2 GetContentAreaSize() const;
    bool IsTabHovered(int index, ImVec2 mousePos) const;
    void AnimateTabs(float deltaTime);
};

// Specialized settings panel with sidebar navigation
class SettingsPanel : public TabbedPanel {
public:
    SettingsPanel(const std::string& id, ImVec2 position, ImVec2 size);
    
    void Initialize();
    
private:
    void CreateGraphicsTab();
    void CreateSoundTab();
    void CreateControlsTab();
    void CreateGameplayTab();
};

} // namespace ArenaFighter