#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include <imgui.h>
#include "UIStyle.h"

namespace ArenaFighter {

class UIScreen;
class UIAnimation;

enum class ScreenType {
    MAIN_MENU,
    CHARACTER_SELECT,
    IN_GAME,
    PAUSE_MENU,
    SETTINGS,
    LOADING
};

class UIManager {
public:
    UIManager();
    ~UIManager();
    
    // Core functionality
    void Initialize(void* window, void* renderer);
    void Shutdown();
    void Update(float deltaTime);
    void Render();
    
    // Screen management
    void PushScreen(ScreenType type);
    void PopScreen();
    void TransitionTo(ScreenType type, float duration = 0.3f);
    UIScreen* GetCurrentScreen() const;
    
    // Input handling
    void ProcessEvent(void* event);
    bool WantsCaptureMouse() const;
    bool WantsCaptureKeyboard() const;
    
    // Resolution handling
    void SetResolution(int width, int height);
    ImVec2 GetResolution() const { return m_resolution; }
    
    // Grid system helpers
    float GetGridColumnWidth() const;
    ImVec2 GetGridPosition(int column, int row, int columnSpan = 1, int rowSpan = 1) const;
    
    // Animation system
    void RegisterAnimation(const std::string& name, std::unique_ptr<UIAnimation> animation);
    void PlayAnimation(const std::string& name);
    void StopAnimation(const std::string& name);
    
    // Debug
    void ShowDebugWindow(bool* open);
    
private:
    bool m_initialized;
    ImVec2 m_resolution;
    float m_uiScale;
    
    // Screen management
    std::vector<std::unique_ptr<UIScreen>> m_screenStack;
    std::unordered_map<ScreenType, std::unique_ptr<UIScreen>> m_screens;
    ScreenType m_currentScreen;
    ScreenType m_transitionTarget;
    float m_transitionTimer;
    float m_transitionDuration;
    bool m_isTransitioning;
    
    // Animation system
    std::unordered_map<std::string, std::unique_ptr<UIAnimation>> m_animations;
    std::vector<UIAnimation*> m_activeAnimations;
    
    // Debug
    bool m_showMetrics;
    bool m_showStyleEditor;
    bool m_showGrid;
    
    // Internal methods
    void CreateScreens();
    void UpdateTransition(float deltaTime);
    void UpdateAnimations(float deltaTime);
    void ApplyGlobalStyle();
    void RenderGrid();
};

} // namespace ArenaFighter