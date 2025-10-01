#pragma once
#include <imgui.h>

namespace ArenaFighter {

class UIManager;

class UIScreen {
public:
    UIScreen() : m_active(false), m_visible(true), m_uiManager(nullptr) {}
    virtual ~UIScreen() = default;
    
    // Lifecycle methods
    virtual void Initialize() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render() = 0;
    virtual void OnEnter() { m_active = true; }
    virtual void OnExit() { m_active = false; }
    
    // State management
    bool IsActive() const { return m_active; }
    bool IsVisible() const { return m_visible; }
    void SetVisible(bool visible) { m_visible = visible; }
    
    // UI Manager reference
    void SetUIManager(UIManager* manager) { m_uiManager = manager; }
    UIManager* GetUIManager() const { return m_uiManager; }
    
protected:
    bool m_active;
    bool m_visible;
    UIManager* m_uiManager;
};

// Particle effect for menu backgrounds
struct ParticleEffect {
    ImVec2 position;
    ImVec2 velocity;
    float size;
    float alpha;
    float lifetime;
    ImU32 color;
};

} // namespace ArenaFighter