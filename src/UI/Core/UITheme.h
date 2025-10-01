#pragma once
#include "UIStyle.h"

namespace ArenaFighter {
namespace UITheme {

// Extended Lost Saga-style colors
const Color BACKGROUND_DARK = {18, 25, 38, 255};
const Color PANEL_BG_LOST = {28, 38, 54, 230};
const Color PANEL_HEADER = {35, 45, 65, 255};
const Color BORDER_ORANGE = {220, 140, 40, 255};
const Color BORDER_ORANGE_GLOW = {255, 180, 80, 255};
const Color TAB_INACTIVE = {45, 55, 75, 255};
const Color TAB_ACTIVE = {60, 70, 90, 255};
const Color ONLINE_GREEN = {40, 200, 40, 255};
const Color OFFLINE_GRAY = {100, 100, 100, 255};
const Color ITEM_COMMON = {150, 150, 150, 255};
const Color ITEM_RARE = {70, 130, 255, 255};
const Color ITEM_EPIC = {180, 70, 255, 255};
const Color ITEM_LEGENDARY = {255, 140, 0, 255};
const Color HOVER_OVERLAY = {255, 255, 255, 20};
const Color SELECTED_OVERLAY = {255, 180, 0, 40};

// Dimensions
const float TAB_HEIGHT = 42.0f;
const float TAB_WIDTH = 120.0f;
const float SIDEBAR_WIDTH = 200.0f;
const float GRID_CELL_SIZE = 64.0f;
const float GRID_SPACING = 4.0f;
const float MODAL_HEADER_HEIGHT = 36.0f;
const float CONTEXT_MENU_WIDTH = 180.0f;
const float SLIDER_HEIGHT = 20.0f;
const float SCROLLBAR_WIDTH = 12.0f;

// Fonts (to be loaded)
enum FontType {
    FONT_REGULAR = 0,
    FONT_BOLD,
    FONT_TITLE,
    FONT_SMALL,
    FONT_ICON
};

// Icon IDs for tab navigation
enum IconType {
    ICON_SETTINGS = 0,
    ICON_GRAPHICS,
    ICON_SOUND,
    ICON_CONTROLS,
    ICON_FRIENDS,
    ICON_GUILD,
    ICON_INVENTORY,
    ICON_QUEST,
    ICON_CLOSE,
    ICON_MINIMIZE,
    ICON_MAXIMIZE
};

} // namespace UITheme
} // namespace ArenaFighter