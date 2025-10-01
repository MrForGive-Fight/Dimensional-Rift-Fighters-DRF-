#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include <map>
#include <functional>

// DFR Systems
#include "../GameModes/GameMode.h"
#include "../GameModes/GameModeManager.h"
#include "../Combat/CombatSystem.h"
#include "../Physics/PhysicsEngine.h"
#include "../Network/NetworkManager.h"

// UI Systems
#include "../UI/UISystem.h"
#include "../UI/MainMenu.h"
#include "../UI/CharacterSelection.h"
#include "../UI/CombatHUD.h"
#include "../UI/EquipmentScreen.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

using namespace DirectX;

namespace ArenaFighter {

enum class AppState {
    MainMenu,
    CharacterSelection,
    InGame,
    Equipment,
    Shop,
    Settings
};

class InputManager {
private:
    struct KeyState {
        bool isDown;
        bool wasPressed;
        bool wasReleased;
    };
    
    std::map<int, KeyState> m_keyStates;
    std::map<std::string, int> m_keyBindings;
    int m_mouseX, m_mouseY;
    bool m_mouseButtons[3];
    
public:
    InputManager();
    
    void initializeDefaultBindings();
    void onKeyDown(int key);
    void onKeyUp(int key);
    void onMouseMove(int x, int y);
    void onMouseDown(int button);
    void onMouseUp(int button);
    void update();
    
    bool isKeyDown(int key) const;
    bool wasKeyPressed(int key) const;
    bool isActionDown(const std::string& action) const;
    bool wasActionPressed(const std::string& action) const;
    void getMousePosition(int& x, int& y) const;
    bool isMouseButtonDown(int button) const;
    void rebindKey(const std::string& action, int newKey);
};

class GameApplication {
private:
    // Window
    HWND m_hwnd;
    int m_screenWidth;
    int m_screenHeight;

    // DirectX
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_deviceContext;
    IDXGISwapChain* m_swapChain;
    ID3D11RenderTargetView* m_renderTargetView;
    ID3D11DepthStencilView* m_depthStencilView;

    // DFR Core Systems
    std::unique_ptr<CombatSystem> m_combatSystem;
    std::unique_ptr<PhysicsEngine> m_physicsEngine;
    std::unique_ptr<NetworkManager> m_networkManager;
    std::unique_ptr<GameModeManager> m_gameModeManager;
    
    // Input System
    std::unique_ptr<InputManager> m_inputManager;

    // Current game state
    AppState m_currentState;
    GameMode* m_currentGameMode;

    // Timing
    float m_deltaTime;
    LARGE_INTEGER m_frequency;
    LARGE_INTEGER m_lastTime;

    bool m_isRunning;

public:
    GameApplication();
    ~GameApplication();

    bool initialize(HWND windowHandle);
    void run();
    void shutdown();

    // Window message handlers
    void onMouseMove(int x, int y);
    void onMouseDown(int x, int y);
    void onMouseUp(int x, int y);
    void onKeyDown(int key);
    void onKeyUp(int key);

private:
    bool initializeDirectX();
    bool initializeInput();
    bool initializeSystems();
    bool initializeUI();

    void update(float dt);
    void render();
    void render3DScene();
    void updateGameplay(float dt);
    void updateDeltaTime();
    void handleGameplayInput();

    // State transition handlers
    void onGameModeSelected(const std::string& modeName);
    void onCharacterSelectionConfirmed();
    void onCharacterSelectionCanceled();
    void onReturnToMainMenu();
    void onMatchEnd(PlayerID winner);

    void cleanup();
};

} // namespace ArenaFighter