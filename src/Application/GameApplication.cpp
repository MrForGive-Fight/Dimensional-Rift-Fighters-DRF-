#include "GameApplication.h"

#include "../UI/MainMenuScreen.h"
#include "../UI/CharacterSelectionScreen.h"
#include "../UI/CombatHUD.h"
#include "../UI/Core/UIManager.h"
#include "../Characters/CharacterBase.h"
#include "../Network/InputBuffer.h"
#include "../Combat/CombatEnums.h"

#include <chrono>
#include <iostream>

namespace ArenaFighter {

// Input Manager Implementation
InputManager::InputManager() : m_mouseX(0), m_mouseY(0) {
    m_mouseButtons[0] = m_mouseButtons[1] = m_mouseButtons[2] = false;
    initializeDefaultBindings();
}

void InputManager::initializeDefaultBindings() {
    // Movement keys
    m_keyBindings["MoveLeft"] = 'A';
    m_keyBindings["MoveRight"] = 'D';
    m_keyBindings["Jump"] = 'W';
    m_keyBindings["Crouch"] = 'S';
    
    // Combat actions - no cooldowns, only mana costs
    m_keyBindings["LightAttack"] = VK_LBUTTON;      // 5-15 mana
    m_keyBindings["MediumAttack"] = VK_RBUTTON;     // 20-30 mana
    m_keyBindings["HeavyAttack"] = VK_MBUTTON;      // 30-40 mana
    m_keyBindings["Special1"] = 'Q';                 // 20-40 mana
    m_keyBindings["Special2"] = 'E';                 // 20-40 mana
    m_keyBindings["Ultimate"] = 'R';                 // 50-70 mana
    
    // Gear switching (instant, no mana cost)
    m_keyBindings["Gear1"] = '1';
    m_keyBindings["Gear2"] = '2';
    m_keyBindings["Gear3"] = '3';
    m_keyBindings["Gear4"] = '4';
    
    // Defensive actions
    m_keyBindings["Block"] = VK_SHIFT;
    m_keyBindings["Dash"] = VK_SPACE;
    
    // System
    m_keyBindings["Pause"] = VK_ESCAPE;
    m_keyBindings["Menu"] = VK_TAB;
}

void InputManager::onKeyDown(int key) {
    m_keyStates[key].isDown = true;
    m_keyStates[key].wasPressed = true;
}

void InputManager::onKeyUp(int key) {
    m_keyStates[key].isDown = false;
    m_keyStates[key].wasReleased = true;
}

void InputManager::onMouseMove(int x, int y) {
    m_mouseX = x;
    m_mouseY = y;
}

void InputManager::onMouseDown(int button) {
    if (button >= 0 && button < 3) {
        m_mouseButtons[button] = true;
        
        // Map mouse buttons to virtual keys for unified input handling
        int vkey = 0;
        switch (button) {
            case 0: vkey = VK_LBUTTON; break;
            case 1: vkey = VK_RBUTTON; break;
            case 2: vkey = VK_MBUTTON; break;
        }
        if (vkey) onKeyDown(vkey);
    }
}

void InputManager::onMouseUp(int button) {
    if (button >= 0 && button < 3) {
        m_mouseButtons[button] = false;
        
        int vkey = 0;
        switch (button) {
            case 0: vkey = VK_LBUTTON; break;
            case 1: vkey = VK_RBUTTON; break;
            case 2: vkey = VK_MBUTTON; break;
        }
        if (vkey) onKeyUp(vkey);
    }
}

void InputManager::update() {
    // Clear single-frame states
    for (auto& pair : m_keyStates) {
        pair.second.wasPressed = false;
        pair.second.wasReleased = false;
    }
}

bool InputManager::isKeyDown(int key) const {
    auto it = m_keyStates.find(key);
    return it != m_keyStates.end() && it->second.isDown;
}

bool InputManager::wasKeyPressed(int key) const {
    auto it = m_keyStates.find(key);
    return it != m_keyStates.end() && it->second.wasPressed;
}

bool InputManager::isActionDown(const std::string& action) const {
    auto it = m_keyBindings.find(action);
    if (it != m_keyBindings.end()) {
        return isKeyDown(it->second);
    }
    return false;
}

bool InputManager::wasActionPressed(const std::string& action) const {
    auto it = m_keyBindings.find(action);
    if (it != m_keyBindings.end()) {
        return wasKeyPressed(it->second);
    }
    return false;
}

void InputManager::getMousePosition(int& x, int& y) const {
    x = m_mouseX;
    y = m_mouseY;
}

bool InputManager::isMouseButtonDown(int button) const {
    return button >= 0 && button < 3 && m_mouseButtons[button];
}

void InputManager::rebindKey(const std::string& action, int newKey) {
    m_keyBindings[action] = newKey;
}

// Game Application Implementation
GameApplication::GameApplication() 
    : m_hwnd(nullptr),
      m_screenWidth(1280),
      m_screenHeight(720),
      m_device(nullptr),
      m_deviceContext(nullptr),
      m_swapChain(nullptr),
      m_renderTargetView(nullptr),
      m_depthStencilView(nullptr),
      m_currentState(AppState::MainMenu),
      m_currentGameMode(nullptr),
      m_deltaTime(0.0f),
      m_isRunning(false) {
    
    QueryPerformanceFrequency(&m_frequency);
    QueryPerformanceCounter(&m_lastTime);
}

GameApplication::~GameApplication() {
    shutdown();
}

bool GameApplication::initialize(HWND windowHandle) {
    m_hwnd = windowHandle;
    
    // Get actual window size
    RECT rect;
    GetClientRect(m_hwnd, &rect);
    m_screenWidth = rect.right - rect.left;
    m_screenHeight = rect.bottom - rect.top;
    
    if (!initializeDirectX()) {
        std::cerr << "Failed to initialize DirectX" << std::endl;
        return false;
    }
    
    if (!initializeInput()) {
        std::cerr << "Failed to initialize input system" << std::endl;
        return false;
    }
    
    if (!initializeSystems()) {
        std::cerr << "Failed to initialize game systems" << std::endl;
        return false;
    }
    
    if (!initializeUI()) {
        std::cerr << "Failed to initialize UI" << std::endl;
        return false;
    }
    
    m_isRunning = true;
    return true;
}

bool GameApplication::initializeDirectX() {
    // Create swap chain description
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = m_screenWidth;
    swapChainDesc.BufferDesc.Height = m_screenHeight;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = m_hwnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = TRUE;
    
    // Create device and swap chain
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        &featureLevel,
        1,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        &m_swapChain,
        &m_device,
        nullptr,
        &m_deviceContext
    );
    
    if (FAILED(hr)) {
        return false;
    }
    
    // Create render target view
    ID3D11Texture2D* backBuffer;
    m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    m_device->CreateRenderTargetView(backBuffer, nullptr, &m_renderTargetView);
    backBuffer->Release();
    
    // Create depth stencil buffer
    D3D11_TEXTURE2D_DESC depthStencilDesc = {};
    depthStencilDesc.Width = m_screenWidth;
    depthStencilDesc.Height = m_screenHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    
    ID3D11Texture2D* depthStencilBuffer;
    m_device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer);
    m_device->CreateDepthStencilView(depthStencilBuffer, nullptr, &m_depthStencilView);
    depthStencilBuffer->Release();
    
    // Set render targets
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
    
    // Set viewport
    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(m_screenWidth);
    viewport.Height = static_cast<float>(m_screenHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    m_deviceContext->RSSetViewports(1, &viewport);
    
    return true;
}

bool GameApplication::initializeInput() {
    m_inputManager = std::make_unique<InputManager>();
    return true;
}

bool GameApplication::initializeSystems() {
    // Initialize core game systems
    m_combatSystem = std::make_unique<CombatSystem>();
    m_physicsEngine = std::make_unique<PhysicsEngine>();
    m_networkManager = std::make_unique<NetworkManager>();
    m_gameModeManager = std::make_unique<GameModeManager>();
    
    // Configure physics engine for 60Hz tick rate
    m_physicsEngine->setFixedTimeStep(1.0f / 60.0f);
    
    // Initialize game mode manager
    m_gameModeManager->initialize();
    
    return true;
}

bool GameApplication::initializeUI() {
    // Initialize UI manager
    UIManager::getInstance().initialize(m_device, m_deviceContext);
    
    return true;
}

void GameApplication::run() {
    MSG msg = {};
    
    while (m_isRunning) {
        // Handle Windows messages
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            
            if (msg.message == WM_QUIT) {
                m_isRunning = false;
            }
        }
        
        if (m_isRunning) {
            updateDeltaTime();
            update(m_deltaTime);
            render();
        }
    }
}

void GameApplication::updateDeltaTime() {
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    
    m_deltaTime = static_cast<float>(currentTime.QuadPart - m_lastTime.QuadPart) / 
                  static_cast<float>(m_frequency.QuadPart);
    
    // Cap delta time to prevent large jumps
    if (m_deltaTime > 0.1f) {
        m_deltaTime = 0.1f;
    }
    
    m_lastTime = currentTime;
}

void GameApplication::update(float dt) {
    // Update input state
    m_inputManager->update();
    
    // Update based on current application state
    switch (m_currentState) {
        case AppState::MainMenu:
            UIManager::getInstance().update(dt);
            break;
            
        case AppState::CharacterSelection:
            UIManager::getInstance().update(dt);
            break;
            
        case AppState::InGame:
            updateGameplay(dt);
            break;
            
        case AppState::Equipment:
            UIManager::getInstance().update(dt);
            break;
            
        case AppState::Shop:
            UIManager::getInstance().update(dt);
            break;
            
        case AppState::Settings:
            UIManager::getInstance().update(dt);
            break;
    }
    
    // Check for global inputs (pause, etc.)
    if (m_inputManager->wasActionPressed("Pause")) {
        if (m_currentState == AppState::InGame) {
            // TODO: Implement pause menu
        }
    }
}

void GameApplication::updateGameplay(float dt) {
    if (!m_currentGameMode) return;
    
    // Handle gameplay input
    handleGameplayInput();
    
    // Update physics at fixed 60Hz tick rate
    m_physicsEngine->update(dt);
    
    // Update combat system
    m_combatSystem->update(dt);
    
    // Update network (30Hz send rate)
    static float networkAccumulator = 0.0f;
    networkAccumulator += dt;
    if (networkAccumulator >= 1.0f / 30.0f) {
        m_networkManager->update(networkAccumulator);
        networkAccumulator = 0.0f;
    }
    
    // Update current game mode
    m_currentGameMode->update(dt);
    
    // Update UI
    UIManager::getInstance().update(dt);
}

void GameApplication::handleGameplayInput() {
    if (!m_currentGameMode) return;
    
    // Get local player ID (assuming player 1 for now)
    PlayerID localPlayer = PlayerID::Player1;
    
    // Movement inputs
    float moveX = 0.0f;
    if (m_inputManager->isActionDown("MoveLeft")) moveX -= 1.0f;
    if (m_inputManager->isActionDown("MoveRight")) moveX += 1.0f;
    
    if (moveX != 0.0f) {
        m_combatSystem->handleMovementInput(localPlayer, moveX, 0.0f);
    }
    
    // Jump input
    if (m_inputManager->wasActionPressed("Jump")) {
        m_combatSystem->handleJump(localPlayer);
    }
    
    // Crouch input
    if (m_inputManager->isActionDown("Crouch")) {
        m_combatSystem->handleCrouch(localPlayer, true);
    } else if (m_inputManager->wasKeyReleased('S')) {
        m_combatSystem->handleCrouch(localPlayer, false);
    }
    
    // Combat actions - mana-based, no cooldowns
    if (m_inputManager->wasActionPressed("LightAttack")) {
        m_combatSystem->handleAttack(localPlayer, AttackType::Light);
    }
    
    if (m_inputManager->wasActionPressed("MediumAttack")) {
        m_combatSystem->handleAttack(localPlayer, AttackType::Medium);
    }
    
    if (m_inputManager->wasActionPressed("HeavyAttack")) {
        m_combatSystem->handleAttack(localPlayer, AttackType::Heavy);
    }
    
    if (m_inputManager->wasActionPressed("Special1")) {
        m_combatSystem->handleSpecialMove(localPlayer, 0);
    }
    
    if (m_inputManager->wasActionPressed("Special2")) {
        m_combatSystem->handleSpecialMove(localPlayer, 1);
    }
    
    if (m_inputManager->wasActionPressed("Ultimate")) {
        m_combatSystem->handleUltimate(localPlayer);
    }
    
    // Gear switching - instant, no mana cost
    for (int i = 0; i < 4; ++i) {
        std::string gearKey = "Gear" + std::to_string(i + 1);
        if (m_inputManager->wasActionPressed(gearKey)) {
            m_combatSystem->switchGear(localPlayer, i);
        }
    }
    
    // Defensive actions
    if (m_inputManager->isActionDown("Block")) {
        m_combatSystem->handleBlock(localPlayer, true);
    } else if (m_inputManager->wasKeyReleased(VK_SHIFT)) {
        m_combatSystem->handleBlock(localPlayer, false);
    }
    
    if (m_inputManager->wasActionPressed("Dash")) {
        m_combatSystem->handleDash(localPlayer);
    }
}

void GameApplication::render() {
    // Clear the screen
    float clearColor[4] = { 0.1f, 0.1f, 0.15f, 1.0f };
    m_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColor);
    m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    
    // Render based on current state
    switch (m_currentState) {
        case AppState::MainMenu:
        case AppState::CharacterSelection:
        case AppState::Equipment:
        case AppState::Shop:
        case AppState::Settings:
            UIManager::getInstance().render();
            break;
            
        case AppState::InGame:
            render3DScene();
            UIManager::getInstance().render();
            break;
    }
    
    // Present the frame
    m_swapChain->Present(1, 0); // VSync enabled
}

void GameApplication::render3DScene() {
    // TODO: Implement 3D scene rendering
    // This would include:
    // - Character rendering
    // - Stage rendering
    // - Effect rendering
    // - Debug visualization (if enabled)
}

// State transition handlers
void GameApplication::onGameModeSelected(const std::string& modeName) {
    // Transition to character selection
    m_currentState = AppState::CharacterSelection;
    
    // Store selected mode for later
    m_gameModeManager->setSelectedMode(modeName);
    
    // Create character selection screen
    auto charSelect = std::make_shared<CharacterSelectionScreen>(m_device, m_deviceContext);
    charSelect->setCallbacks(
        [this]() { onCharacterSelectionConfirmed(); },
        [this]() { onCharacterSelectionCanceled(); }
    );
    
    UIManager::getInstance().setCurrentScreen(charSelect->getRootPanel());
}

void GameApplication::onCharacterSelectionConfirmed() {
    // Get selected characters
    auto charSelectScreen = std::dynamic_pointer_cast<CharacterSelectionScreen>(
        UIManager::getInstance().getCurrentScreen());
    
    if (charSelectScreen) {
        auto selectedChars = charSelectScreen->getSelectedCharacters();
        
        // Create game mode
        m_currentGameMode = m_gameModeManager->createSelectedMode();
        if (m_currentGameMode) {
            // Initialize game mode with selected characters
            // TODO: Pass selected characters to game mode
            
            // Transition to gameplay
            m_currentState = AppState::InGame;
            
            // Create combat HUD
            auto combatHUD = std::make_shared<CombatHUD>();
            UIManager::getInstance().setCurrentScreen(combatHUD->getRootPanel());
            
            // Start the match
            m_currentGameMode->startMatch();
        }
    }
}

void GameApplication::onCharacterSelectionCanceled() {
    // Return to main menu
    onReturnToMainMenu();
}

void GameApplication::onReturnToMainMenu() {
    m_currentState = AppState::MainMenu;
    
    // Clean up current game mode if exists
    if (m_currentGameMode) {
        m_currentGameMode->endMatch();
        m_currentGameMode = nullptr;
    }
    
    // Create main menu screen
    auto mainMenu = std::make_shared<MainMenuScreen>(m_device, m_deviceContext);
    mainMenu->setCallbacks(
        [this](const std::string& mode) { onGameModeSelected(mode); },
        [this]() { /* TODO: Shop */ },
        [this]() { /* TODO: Inventory */ }
    );
    
    UIManager::getInstance().setCurrentScreen(mainMenu->getRootPanel());
}

void GameApplication::onMatchEnd(PlayerID winner) {
    // TODO: Show match results screen
    
    // For now, return to main menu after a delay
    onReturnToMainMenu();
}

// Window message handlers
void GameApplication::onMouseMove(int x, int y) {
    if (m_inputManager) {
        m_inputManager->onMouseMove(x, y);
    }
}

void GameApplication::onMouseDown(int x, int y) {
    if (m_inputManager) {
        // Determine which button was pressed (assuming left button for simplicity)
        m_inputManager->onMouseDown(0);
    }
}

void GameApplication::onMouseUp(int x, int y) {
    if (m_inputManager) {
        m_inputManager->onMouseUp(0);
    }
}

void GameApplication::onKeyDown(int key) {
    if (m_inputManager) {
        m_inputManager->onKeyDown(key);
    }
}

void GameApplication::onKeyUp(int key) {
    if (m_inputManager) {
        m_inputManager->onKeyUp(key);
    }
}

void GameApplication::shutdown() {
    cleanup();
}

void GameApplication::cleanup() {
    m_isRunning = false;
    
    // Clean up game systems
    if (m_currentGameMode) {
        m_currentGameMode->endMatch();
        m_currentGameMode = nullptr;
    }
    
    m_gameModeManager.reset();
    m_networkManager.reset();
    m_physicsEngine.reset();
    m_combatSystem.reset();
    m_inputManager.reset();
    
    // Clean up DirectX resources
    if (m_depthStencilView) {
        m_depthStencilView->Release();
        m_depthStencilView = nullptr;
    }
    
    if (m_renderTargetView) {
        m_renderTargetView->Release();
        m_renderTargetView = nullptr;
    }
    
    if (m_swapChain) {
        m_swapChain->Release();
        m_swapChain = nullptr;
    }
    
    if (m_deviceContext) {
        m_deviceContext->Release();
        m_deviceContext = nullptr;
    }
    
    if (m_device) {
        m_device->Release();
        m_device = nullptr;
    }
}

} // namespace ArenaFighter