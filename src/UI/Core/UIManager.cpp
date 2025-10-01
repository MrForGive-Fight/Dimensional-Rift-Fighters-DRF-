#include "UIManager.h"
#include "../Screens/MainMenuScreen.h"
#include "../Screens/CharacterSelectScreen.h"
#include "../Screens/InGameHUD.h"
#include "../Animations/UIAnimation.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

namespace ArenaFighter {

UIManager::UIManager()
    : m_initialized(false),
      m_resolution(1920.0f, 1080.0f),
      m_uiScale(1.0f),
      m_currentScreen(ScreenType::MAIN_MENU),
      m_transitionTarget(ScreenType::MAIN_MENU),
      m_transitionTimer(0.0f),
      m_transitionDuration(0.3f),
      m_isTransitioning(false),
      m_showMetrics(false),
      m_showStyleEditor(false),
      m_showGrid(false) {
}

UIManager::~UIManager() {
    Shutdown();
}

void UIManager::Initialize(void* window, void* renderer) {
    if (m_initialized) return;
    
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    
    // Configure ImGui
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    
    // Set up style
    ApplyGlobalStyle();
    
    // Platform/Renderer initialization
    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX11_Init(static_cast<ID3D11Device*>(renderer), 
                        static_cast<ID3D11DeviceContext*>(renderer));
    
    // Load fonts
    io.Fonts->AddFontFromFileTTF("assets/fonts/RobotoBold.ttf", 18.0f * m_uiScale);
    io.Fonts->AddFontFromFileTTF("assets/fonts/RobotoBold.ttf", 24.0f * m_uiScale);
    io.Fonts->AddFontFromFileTTF("assets/fonts/RobotoBold.ttf", 36.0f * m_uiScale);
    io.Fonts->AddFontFromFileTTF("assets/fonts/RobotoBold.ttf", 48.0f * m_uiScale);
    
    // Create screens
    CreateScreens();
    
    m_initialized = true;
}

void UIManager::Shutdown() {
    if (!m_initialized) return;
    
    // Cleanup screens
    m_screens.clear();
    m_screenStack.clear();
    
    // Cleanup ImGui
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    
    m_initialized = false;
}

void UIManager::Update(float deltaTime) {
    if (!m_initialized) return;
    
    // Update transition
    if (m_isTransitioning) {
        UpdateTransition(deltaTime);
    }
    
    // Update animations
    UpdateAnimations(deltaTime);
    
    // Update current screen
    if (!m_screenStack.empty() && m_screenStack.back()) {
        m_screenStack.back()->Update(deltaTime);
    }
}

void UIManager::Render() {
    if (!m_initialized) return;
    
    // Start ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    
    // Render debug grid if enabled
    if (m_showGrid) {
        RenderGrid();
    }
    
    // Render screens (back to front for proper layering)
    for (auto& screen : m_screenStack) {
        if (screen && screen->IsVisible()) {
            screen->Render();
        }
    }
    
    // Render debug windows
    if (m_showMetrics) {
        ImGui::ShowMetricsWindow(&m_showMetrics);
    }
    
    if (m_showStyleEditor) {
        ImGui::ShowStyleEditor();
    }
    
    // End frame
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::PushScreen(ScreenType type) {
    auto it = m_screens.find(type);
    if (it != m_screens.end() && it->second) {
        it->second->OnEnter();
        m_screenStack.push_back(std::move(it->second));
        m_screens.erase(it);
    }
}

void UIManager::PopScreen() {
    if (!m_screenStack.empty()) {
        m_screenStack.back()->OnExit();
        m_screenStack.pop_back();
    }
}

void UIManager::TransitionTo(ScreenType type, float duration) {
    m_transitionTarget = type;
    m_transitionDuration = duration;
    m_transitionTimer = 0.0f;
    m_isTransitioning = true;
    
    // Start fade out of current screen
    if (!m_screenStack.empty()) {
        m_screenStack.back()->OnExit();
    }
}

UIScreen* UIManager::GetCurrentScreen() const {
    return m_screenStack.empty() ? nullptr : m_screenStack.back().get();
}

void UIManager::ProcessEvent(void* event) {
    if (!m_initialized) return;
    ImGui_ImplWin32_WndProcHandler(nullptr, 0, 0, 0);
}

bool UIManager::WantsCaptureMouse() const {
    return ImGui::GetIO().WantCaptureMouse;
}

bool UIManager::WantsCaptureKeyboard() const {
    return ImGui::GetIO().WantCaptureKeyboard;
}

void UIManager::SetResolution(int width, int height) {
    m_resolution = ImVec2(static_cast<float>(width), static_cast<float>(height));
    m_uiScale = std::min(width / 1920.0f, height / 1080.0f);
}

float UIManager::GetGridColumnWidth() const {
    float totalWidth = m_resolution.x - (UIStyle::GRID_GUTTER * (UIStyle::GRID_COLUMNS + 1));
    return totalWidth / UIStyle::GRID_COLUMNS;
}

ImVec2 UIManager::GetGridPosition(int column, int row, int columnSpan, int rowSpan) const {
    float colWidth = GetGridColumnWidth();
    float rowHeight = colWidth; // Square grid cells
    
    float x = UIStyle::GRID_GUTTER + (column * (colWidth + UIStyle::GRID_GUTTER));
    float y = UIStyle::GRID_GUTTER + (row * (rowHeight + UIStyle::GRID_GUTTER));
    
    float width = (colWidth * columnSpan) + (UIStyle::GRID_GUTTER * (columnSpan - 1));
    float height = (rowHeight * rowSpan) + (UIStyle::GRID_GUTTER * (rowSpan - 1));
    
    return ImVec2(width, height);
}

void UIManager::RegisterAnimation(const std::string& name, std::unique_ptr<UIAnimation> animation) {
    m_animations[name] = std::move(animation);
}

void UIManager::PlayAnimation(const std::string& name) {
    auto it = m_animations.find(name);
    if (it != m_animations.end() && it->second) {
        it->second->Play();
        m_activeAnimations.push_back(it->second.get());
    }
}

void UIManager::StopAnimation(const std::string& name) {
    auto it = m_animations.find(name);
    if (it != m_animations.end() && it->second) {
        it->second->Stop();
        auto activeIt = std::find(m_activeAnimations.begin(), m_activeAnimations.end(), it->second.get());
        if (activeIt != m_activeAnimations.end()) {
            m_activeAnimations.erase(activeIt);
        }
    }
}

void UIManager::ShowDebugWindow(bool* open) {
    if (!open || !*open) return;
    
    if (ImGui::Begin("UI Debug", open)) {
        ImGui::Text("Resolution: %.0fx%.0f", m_resolution.x, m_resolution.y);
        ImGui::Text("UI Scale: %.2f", m_uiScale);
        ImGui::Text("Active Screens: %zu", m_screenStack.size());
        ImGui::Text("Active Animations: %zu", m_activeAnimations.size());
        
        ImGui::Separator();
        
        ImGui::Checkbox("Show Metrics", &m_showMetrics);
        ImGui::Checkbox("Show Style Editor", &m_showStyleEditor);
        ImGui::Checkbox("Show Grid", &m_showGrid);
        
        ImGui::Separator();
        
        if (ImGui::CollapsingHeader("Screen Stack")) {
            for (size_t i = 0; i < m_screenStack.size(); ++i) {
                ImGui::Text("  [%zu] Screen", i);
            }
        }
        
        if (ImGui::CollapsingHeader("Animations")) {
            for (const auto& [name, anim] : m_animations) {
                ImGui::Text("  %s: %s", name.c_str(), 
                           anim->IsPlaying() ? "Playing" : "Stopped");
            }
        }
    }
    ImGui::End();
}

void UIManager::CreateScreens() {
    // Create main menu
    auto mainMenu = std::make_unique<MainMenuScreen>();
    mainMenu->SetUIManager(this);
    mainMenu->Initialize();
    m_screens[ScreenType::MAIN_MENU] = std::move(mainMenu);
    
    // Create character select
    auto charSelect = std::make_unique<CharacterSelectScreen>();
    charSelect->SetUIManager(this);
    charSelect->Initialize();
    m_screens[ScreenType::CHARACTER_SELECT] = std::move(charSelect);
    
    // Create in-game HUD
    auto hud = std::make_unique<InGameHUD>();
    hud->SetUIManager(this);
    hud->Initialize();
    m_screens[ScreenType::IN_GAME] = std::move(hud);
    
    // Start with main menu
    PushScreen(ScreenType::MAIN_MENU);
}

void UIManager::UpdateTransition(float deltaTime) {
    m_transitionTimer += deltaTime;
    
    if (m_transitionTimer >= m_transitionDuration * 0.5f && !m_screenStack.empty()) {
        // Halfway through - switch screens
        PopScreen();
        PushScreen(m_transitionTarget);
    }
    
    if (m_transitionTimer >= m_transitionDuration) {
        m_isTransitioning = false;
    }
}

void UIManager::UpdateAnimations(float deltaTime) {
    // Update active animations
    for (auto it = m_activeAnimations.begin(); it != m_activeAnimations.end();) {
        (*it)->Update(deltaTime);
        
        if (!(*it)->IsPlaying()) {
            it = m_activeAnimations.erase(it);
        } else {
            ++it;
        }
    }
}

void UIManager::ApplyGlobalStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Main
    style.WindowPadding = ImVec2(15, 15);
    style.FramePadding = ImVec2(5, 5);
    style.CellPadding = ImVec2(6, 6);
    style.ItemSpacing = ImVec2(12, 8);
    style.ItemInnerSpacing = ImVec2(8, 6);
    style.TouchExtraPadding = ImVec2(0, 0);
    style.IndentSpacing = 25;
    style.ScrollbarSize = 15;
    style.GrabMinSize = 10;
    
    // Borders
    style.WindowBorderSize = UIStyle::BORDER_WIDTH;
    style.ChildBorderSize = UIStyle::BORDER_WIDTH;
    style.PopupBorderSize = UIStyle::BORDER_WIDTH;
    style.FrameBorderSize = UIStyle::BORDER_WIDTH;
    style.TabBorderSize = UIStyle::BORDER_WIDTH;
    
    // Rounding
    style.WindowRounding = UIStyle::CORNER_RADIUS;
    style.ChildRounding = UIStyle::CORNER_RADIUS;
    style.FrameRounding = UIStyle::CORNER_RADIUS;
    style.PopupRounding = UIStyle::CORNER_RADIUS;
    style.ScrollbarRounding = UIStyle::CORNER_RADIUS;
    style.GrabRounding = UIStyle::CORNER_RADIUS;
    style.TabRounding = UIStyle::CORNER_RADIUS;
    
    // Colors
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text] = UIStyle::TEXT_WHITE;
    colors[ImGuiCol_TextDisabled] = UIStyle::TEXT_GRAY;
    colors[ImGuiCol_WindowBg] = UIStyle::PANEL_BG;
    colors[ImGuiCol_ChildBg] = UIStyle::PANEL_BG_DARK;
    colors[ImGuiCol_PopupBg] = UIStyle::PANEL_BG;
    colors[ImGuiCol_Border] = UIStyle::BORDER_BLUE;
    colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);
    colors[ImGuiCol_FrameBg] = UIStyle::PANEL_BG_DARK;
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.35f, 0.50f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.35f, 0.50f, 0.67f);
    colors[ImGuiCol_Button] = UIStyle::BUTTON_RED;
    colors[ImGuiCol_ButtonHovered] = UIStyle::BUTTON_RED_HOVER;
    colors[ImGuiCol_ButtonActive] = UIStyle::BUTTON_RED_ACTIVE;
}

void UIManager::RenderGrid() {
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    float colWidth = GetGridColumnWidth();
    float rowHeight = colWidth;
    
    ImU32 gridColor = IM_COL32(255, 255, 255, 30);
    ImU32 gutterColor = IM_COL32(255, 100, 100, 20);
    
    // Draw columns
    for (int i = 0; i <= UIStyle::GRID_COLUMNS; ++i) {
        float x = UIStyle::GRID_GUTTER + i * (colWidth + UIStyle::GRID_GUTTER);
        drawList->AddLine(ImVec2(x, 0), ImVec2(x, m_resolution.y), gridColor);
        
        // Gutter lines
        if (i < UIStyle::GRID_COLUMNS) {
            float gutterX = x + colWidth;
            drawList->AddRectFilled(ImVec2(gutterX, 0), 
                                   ImVec2(gutterX + UIStyle::GRID_GUTTER, m_resolution.y), 
                                   gutterColor);
        }
    }
    
    // Draw rows
    int numRows = static_cast<int>(m_resolution.y / (rowHeight + UIStyle::GRID_GUTTER));
    for (int i = 0; i <= numRows; ++i) {
        float y = UIStyle::GRID_GUTTER + i * (rowHeight + UIStyle::GRID_GUTTER);
        drawList->AddLine(ImVec2(0, y), ImVec2(m_resolution.x, y), gridColor);
        
        // Gutter lines
        if (i < numRows) {
            float gutterY = y + rowHeight;
            drawList->AddRectFilled(ImVec2(0, gutterY), 
                                   ImVec2(m_resolution.x, gutterY + UIStyle::GRID_GUTTER), 
                                   gutterColor);
        }
    }
}

} // namespace ArenaFighter