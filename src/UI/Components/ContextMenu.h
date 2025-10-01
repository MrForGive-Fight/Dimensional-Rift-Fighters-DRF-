#pragma once
#include "../Core/UITheme.h"
#include "UIPanel.h"
#include <vector>
#include <functional>
#include <string>

namespace ArenaFighter {

class ContextMenu : public UIPanel {
public:
    struct MenuItem {
        std::string label;
        void* icon;
        std::function<void()> callback;
        bool enabled;
        bool separator;
        float hoverAnimation;
    };
    
    ContextMenu();
    ~ContextMenu();
    
    // Menu items
    void AddItem(const std::string& label, std::function<void()> callback, void* icon = nullptr);
    void AddSeparator();
    void Clear();
    void EnableItem(int index, bool enabled);
    
    // Display
    void Show(ImVec2 position);
    void Hide();
    bool IsVisible() const { return m_isVisible; }
    
    // Rendering
    void Render() override;
    void Update(float deltaTime) override;
    
private:
    std::vector<MenuItem> m_items;
    bool m_isVisible;
    ImVec2 m_position;
    int m_hoveredItem;
    float m_width;
    float m_itemHeight;
    float m_fadeAnimation;
    
    // Internal methods
    void CalculateSize();
    void RenderMenuItem(const MenuItem& item, ImVec2 position, bool hovered);
    void HandleInput();
    bool IsMouseOverMenu() const;
};

// Global context menu manager
class ContextMenuManager {
public:
    static ContextMenuManager& Instance();
    
    void ShowContextMenu(ContextMenu* menu, ImVec2 position);
    void HideAll();
    void Update(float deltaTime);
    void Render();
    
private:
    ContextMenuManager() = default;
    std::vector<ContextMenu*> m_activeMenus;
};

// Common context menu presets
namespace ContextMenuPresets {
    std::unique_ptr<ContextMenu> CreatePlayerContextMenu(
        std::function<void()> onWhisper,
        std::function<void()> onInvite,
        std::function<void()> onViewProfile,
        std::function<void()> onAddFriend,
        std::function<void()> onBlock
    );
    
    std::unique_ptr<ContextMenu> CreateInventoryItemMenu(
        std::function<void()> onEquip,
        std::function<void()> onDrop,
        std::function<void()> onSell,
        std::function<void()> onEnhance
    );
}

} // namespace ArenaFighter