#include "GameEngine.h"
#include "../Rendering/RenderingSystem.h"
#include "../Physics/PhysicsEngine.h"
#include "../Network/NetworkLayer.h"
#include "../Characters/CharacterSystem.h"
#include "../Combat/CombatSystem.h"
#include "../GameModes/GameModesManager.h"

#include <iostream>
#include <thread>

namespace ArenaFighter {

bool GameEngine::Initialize() {
    std::cout << "Initializing Anime Arena Fighter Engine..." << std::endl;

    // Initialize Rendering System
    m_renderer = std::make_unique<RenderingSystem>();
    if (!m_renderer->Initialize()) {
        std::cerr << "Failed to initialize Rendering System" << std::endl;
        return false;
    }

    // Initialize Physics Engine
    m_physics = std::make_unique<PhysicsEngine>();
    if (!m_physics->Initialize()) {
        std::cerr << "Failed to initialize Physics Engine" << std::endl;
        return false;
    }

    // Initialize Network Layer
    m_network = std::make_unique<NetworkLayer>();
    if (!m_network->Initialize()) {
        std::cerr << "Failed to initialize Network Layer" << std::endl;
        return false;
    }

    // Initialize Character System
    m_characterSystem = std::make_unique<CharacterSystem>();
    if (!m_characterSystem->Initialize()) {
        std::cerr << "Failed to initialize Character System" << std::endl;
        return false;
    }

    // Initialize Combat System
    m_combatSystem = std::make_unique<CombatSystem>();
    if (!m_combatSystem->Initialize()) {
        std::cerr << "Failed to initialize Combat System" << std::endl;
        return false;
    }

    // Initialize Game Modes
    m_gameModes = std::make_unique<GameModesManager>();
    if (!m_gameModes->Initialize()) {
        std::cerr << "Failed to initialize Game Modes Manager" << std::endl;
        return false;
    }

    m_lastFrameTime = std::chrono::steady_clock::now();
    std::cout << "Engine initialized successfully!" << std::endl;
    return true;
}

void GameEngine::Run() {
    m_running = true;

    while (m_running) {
        // Calculate delta time
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsed = currentTime - m_lastFrameTime;
        m_deltaTime = elapsed.count();

        // Frame rate limiting
        if (m_deltaTime < TARGET_FRAME_TIME) {
            std::this_thread::sleep_for(
                std::chrono::duration<float>(TARGET_FRAME_TIME - m_deltaTime)
            );
            currentTime = std::chrono::steady_clock::now();
            elapsed = currentTime - m_lastFrameTime;
            m_deltaTime = elapsed.count();
        }

        m_lastFrameTime = currentTime;
        m_frameCount++;

        // Update all systems
        Update(m_deltaTime);

        // Render
        Render();
    }
}

void GameEngine::Update(float deltaTime) {
    // Update input

    // Update network
    m_network->Update(deltaTime);

    // Update physics
    m_physics->Update(deltaTime);

    // Update character system
    m_characterSystem->Update(deltaTime);

    // Update combat
    m_combatSystem->Update(deltaTime);

    // Update game modes
    m_gameModes->Update(deltaTime);
}

void GameEngine::Render() {
    m_renderer->BeginFrame();
    
    // Render game world
    m_characterSystem->Render(m_renderer.get());
    m_combatSystem->Render(m_renderer.get());
    
    // Render UI
    m_gameModes->RenderUI(m_renderer.get());
    
    m_renderer->EndFrame();
}

void GameEngine::Shutdown() {
    m_running = false;

    // Shutdown in reverse order
    if (m_gameModes) m_gameModes->Shutdown();
    if (m_combatSystem) m_combatSystem->Shutdown();
    if (m_characterSystem) m_characterSystem->Shutdown();
    if (m_network) m_network->Shutdown();
    if (m_physics) m_physics->Shutdown();
    if (m_renderer) m_renderer->Shutdown();

    std::cout << "Engine shutdown complete" << std::endl;
}

} // namespace ArenaFighter