#pragma once
#include <imgui.h>

namespace ArenaFighter {
namespace UIStyle {

// Color definitions
struct Color {
    float r, g, b, a;
    
    operator ImVec4() const { return ImVec4(r/255.0f, g/255.0f, b/255.0f, a/255.0f); }
    ImU32 ToU32() const { return IM_COL32(r, g, b, a); }
};

// UI Colors
const Color PANEL_BG = {26, 35, 50, 255};
const Color PANEL_BG_DARK = {18, 24, 35, 255};
const Color BUTTON_RED = {204, 51, 51, 255};
const Color BUTTON_RED_HOVER = {230, 80, 80, 255};
const Color BUTTON_RED_ACTIVE = {180, 30, 30, 255};
const Color BUTTON_GOLD = {184, 134, 11, 255};
const Color BUTTON_GOLD_HOVER = {220, 170, 50, 255};
const Color BUTTON_GOLD_ACTIVE = {160, 110, 0, 255};
const Color BORDER_BLUE = {52, 152, 219, 255};
const Color BORDER_GOLD = {255, 215, 0, 255};
const Color TEXT_WHITE = {255, 255, 255, 255};
const Color TEXT_GRAY = {180, 180, 180, 255};
const Color HEALTH_RED = {220, 20, 60, 255};
const Color MANA_BLUE = {30, 144, 255, 255};
const Color GAUGE_PURPLE = {147, 112, 219, 255};

// UI Dimensions
const float BORDER_WIDTH = 2.0f;
const float CORNER_RADIUS = 8.0f;
const float BUTTON_HEIGHT = 60.0f;
const float BUTTON_WIDTH = 300.0f;
const float PANEL_PADDING = 20.0f;
const float ELEMENT_SPACING = 10.0f;

// Animation speeds
const float HOVER_SCALE = 1.05f;
const float HOVER_SPEED = 10.0f;
const float FADE_SPEED = 5.0f;
const float SLIDE_SPEED = 8.0f;

// Grid system
const int GRID_COLUMNS = 12;
const float GRID_GUTTER = 15.0f;

// Z-order layers
enum Layer {
    BACKGROUND = 0,
    PANELS = 10,
    BUTTONS = 20,
    OVERLAYS = 30,
    POPUPS = 40,
    TOOLTIPS = 50
};

// Helper functions
inline void PushButtonStyle(const Color& base, const Color& hover = BUTTON_RED_HOVER, const Color& active = BUTTON_RED_ACTIVE) {
    ImGui::PushStyleColor(ImGuiCol_Button, base);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, active);
}

inline void PopButtonStyle() {
    ImGui::PopStyleColor(3);
}

inline void PushPanelStyle() {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, PANEL_BG);
    ImGui::PushStyleColor(ImGuiCol_Border, BORDER_BLUE);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, BORDER_WIDTH);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, CORNER_RADIUS);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(PANEL_PADDING, PANEL_PADDING));
}

inline void PopPanelStyle() {
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
}

} // namespace UIStyle
} // namespace ArenaFighter