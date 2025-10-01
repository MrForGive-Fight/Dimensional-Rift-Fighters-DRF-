#pragma once

#include <memory>
#include <chrono>

namespace ArenaFighter {

class RenderingSystem;
class PhysicsEngine;
class NetworkLayer;
class CharacterSystem;
class CombatSystem;
class GameModesManager;

class GameEngine {
public:
    static GameEngine& Instance() {
        static GameEngine instance;
        return instance;
    }

    bool Initialize();
    void Run();
    void Shutdown();

    // Core Systems Access
    RenderingSystem* GetRenderer() { return m_renderer.get(); }
    PhysicsEngine* GetPhysics() { return m_physics.get(); }
    NetworkLayer* GetNetwork() { return m_network.get(); }
    CharacterSystem* GetCharacterSystem() { return m_characterSystem.get(); }
    CombatSystem* GetCombatSystem() { return m_combatSystem.get(); }
    GameModesManager* GetGameModes() { return m_gameModes.get(); }

    // Frame timing
    float GetDeltaTime() const { return m_deltaTime; }
    uint32_t GetFrameCount() const { return m_frameCount; }

private:
    GameEngine() = default;
    ~GameEngine() = default;
    GameEngine(const GameEngine&) = delete;
    GameEngine& operator=(const GameEngine&) = delete;

    void Update(float deltaTime);
    void Render();

    // Core Systems
    std::unique_ptr<RenderingSystem> m_renderer;
    std::unique_ptr<PhysicsEngine> m_physics;
    std::unique_ptr<NetworkLayer> m_network;
    std::unique_ptr<CharacterSystem> m_characterSystem;
    std::unique_ptr<CombatSystem> m_combatSystem;
    std::unique_ptr<GameModesManager> m_gameModes;

    // Timing
    std::chrono::steady_clock::time_point m_lastFrameTime;
    float m_deltaTime = 0.0f;
    uint32_t m_frameCount = 0;
    bool m_running = false;

    // Performance
    static constexpr float TARGET_FPS = 60.0f;
    static constexpr float TARGET_FRAME_TIME = 1.0f / TARGET_FPS;
};

} // namespace ArenaFighter