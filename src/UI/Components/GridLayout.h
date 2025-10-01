#pragma once
#include "../Core/UITheme.h"
#include <imgui.h>
#include <vector>
#include <memory>
#include <functional>

namespace ArenaFighter {

// Forward declarations
class UIElement;

// Grid layout for inventory-style displays
class GridLayout {
public:
    GridLayout(int columns, int rows, float cellSize = UITheme::GRID_CELL_SIZE);
    ~GridLayout();
    
    // Configuration
    void SetCellSize(float size) { m_cellSize = size; }
    void SetSpacing(float spacing) { m_spacing = spacing; }
    void SetPadding(ImVec2 padding) { m_padding = padding; }
    void SetPosition(ImVec2 pos) { m_position = pos; }
    
    // Item management
    bool AddItem(int slot, std::unique_ptr<UIElement> item);
    void RemoveItem(int slot);
    void SwapItems(int slot1, int slot2);
    void Clear();
    
    // Interaction
    int GetSlotAtPosition(ImVec2 pos) const;
    ImVec2 GetSlotPosition(int slot) const;
    ImRect GetSlotBounds(int slot) const;
    
    // Selection
    void SetSelectedSlot(int slot) { m_selectedSlot = slot; }
    int GetSelectedSlot() const { return m_selectedSlot; }
    
    // Drag and drop
    void EnableDragDrop(bool enable) { m_dragDropEnabled = enable; }
    void SetDraggedSlot(int slot) { m_draggedSlot = slot; }
    int GetDraggedSlot() const { return m_draggedSlot; }
    
    // Rendering
    void Render();
    void Update(float deltaTime);
    
    // Callbacks
    void SetOnSlotClick(std::function<void(int)> callback) { m_onSlotClick = callback; }
    void SetOnSlotHover(std::function<void(int)> callback) { m_onSlotHover = callback; }
    void SetOnItemDrop(std::function<void(int, int)> callback) { m_onItemDrop = callback; }
    
private:
    int m_columns;
    int m_rows;
    float m_cellSize;
    float m_spacing;
    ImVec2 m_padding;
    ImVec2 m_position;
    
    // Items
    std::vector<std::unique_ptr<UIElement>> m_items;
    
    // Interaction state
    int m_selectedSlot;
    int m_hoveredSlot;
    int m_draggedSlot;
    bool m_dragDropEnabled;
    ImVec2 m_dragOffset;
    
    // Animation
    std::vector<float> m_slotAnimations;
    
    // Callbacks
    std::function<void(int)> m_onSlotClick;
    std::function<void(int)> m_onSlotHover;
    std::function<void(int, int)> m_onItemDrop;
    
    // Internal methods
    void RenderGrid();
    void RenderItems();
    void RenderDraggedItem();
    void HandleInput();
    void UpdateAnimations(float deltaTime);
};

// Inventory item for grid display
class InventoryItem : public UIElement {
public:
    enum ItemRarity {
        COMMON,
        RARE,
        EPIC,
        LEGENDARY
    };
    
    InventoryItem(const std::string& name, void* icon);
    
    // Properties
    void SetQuantity(int quantity) { m_quantity = quantity; }
    void SetRarity(ItemRarity rarity) { m_rarity = rarity; }
    void SetEquipped(bool equipped) { m_equipped = equipped; }
    void SetLocked(bool locked) { m_locked = locked; }
    void SetLevel(int level) { m_level = level; }
    
    // Rendering
    void Render() override;
    void RenderTooltip();
    
private:
    std::string m_name;
    std::string m_description;
    void* m_icon;
    int m_quantity;
    ItemRarity m_rarity;
    bool m_equipped;
    bool m_locked;
    int m_level;
    
    ImU32 GetRarityColor() const;
    ImU32 GetRarityGlow() const;
};

// Complete inventory panel
class InventoryPanel : public UIPanel {
public:
    InventoryPanel(const std::string& id, ImVec2 position, ImVec2 size);
    
    // Category tabs
    void AddCategory(const std::string& name, int columns, int rows);
    void SelectCategory(int index);
    
    // Item management
    void AddItem(const std::string& category, std::unique_ptr<InventoryItem> item);
    void RemoveItem(const std::string& category, int slot);
    
    // Character preview
    void SetCharacterModel(void* model) { m_characterModel = model; }
    void SetEquipmentSlots(const std::vector<std::string>& slots);
    
    void Render() override;
    
private:
    struct Category {
        std::string name;
        std::unique_ptr<GridLayout> grid;
    };
    
    std::vector<Category> m_categories;
    int m_activeCategory;
    
    // Character preview
    void* m_characterModel;
    float m_modelRotation;
    std::vector<std::string> m_equipmentSlots;
    
    // Layout
    float m_splitRatio; // Between character preview and inventory
    
    void RenderCharacterPreview();
    void RenderInventoryGrid();
    void RenderCategoryTabs();
};

} // namespace ArenaFighter