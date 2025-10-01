#include "GameApplicationAdapter.h"
#include "../Characters/CharacterFactory.h"
#include "../Shop/ItemManager.h"
#include <algorithm>
#include <cmath>

namespace ArenaFighter {

// DFRInputSystem Implementation

void DFRInputSystem::Update(float deltaTime) {
    // Update S key hold time for blocking
    if (m_sKeyDown) {
        m_sKeyHoldTime += deltaTime;
    }
    
    // Update combo timer
    if (m_comboTimer > 0.0f) {
        m_comboTimer -= deltaTime;
        if (m_comboTimer <= 0.0f) {
            ResetCombos();
        }
    }
    
    // Check for completed combos
    CheckCombos();
}

void DFRInputSystem::OnKeyDown(int key) {
    switch (key) {
        case 'S':
            if (!m_sKeyDown) {
                m_sKeyDown = true;
                m_sKeyHoldTime = 0.0f;
                
                // Check for special moves if direction is held
                if (m_upPressed || m_downPressed || m_leftPressed || m_rightPressed) {
                    m_specialMoveReady = true;
                    if (m_upPressed) m_pendingDirection = InputDirection::Up;
                    else if (m_downPressed) m_pendingDirection = InputDirection::Down;
                    else if (m_leftPressed) m_pendingDirection = InputDirection::Left;
                    else if (m_rightPressed) m_pendingDirection = InputDirection::Right;
                }
            }
            break;
            
        case 'A':
            m_aKeyDown = true;
            m_comboTimer = COMBO_WINDOW;
            break;
            
        case 'D':
            m_dKeyDown = true;
            if (m_sKeyDown) {
                m_sdCombo = true;
                m_comboTimer = COMBO_WINDOW;
            }
            break;
            
        case VK_UP:
        case 'W':
            m_upPressed = true;
            if (m_sKeyDown && !m_specialMoveReady) {
                m_specialMoveReady = true;
                m_pendingDirection = InputDirection::Up;
            }
            break;
            
        case VK_DOWN:
            m_downPressed = true;
            if (m_sKeyDown && !m_specialMoveReady) {
                m_specialMoveReady = true;
                m_pendingDirection = InputDirection::Down;
            }
            break;
            
        case VK_LEFT:
            m_leftPressed = true;
            if (m_sKeyDown && !m_specialMoveReady) {
                m_specialMoveReady = true;
                m_pendingDirection = InputDirection::Left;
            }
            break;
            
        case VK_RIGHT:
            m_rightPressed = true;
            if (m_sKeyDown && !m_specialMoveReady) {
                m_specialMoveReady = true;
                m_pendingDirection = InputDirection::Right;
            }
            break;
    }
}

void DFRInputSystem::OnKeyUp(int key) {
    switch (key) {
        case 'S':
            m_sKeyDown = false;
            m_sKeyHoldTime = 0.0f;
            m_specialMoveReady = false;
            break;
            
        case 'A':
            m_aKeyDown = false;
            break;
            
        case 'D':
            m_dKeyDown = false;
            break;
            
        case VK_UP:
        case 'W':
            m_upPressed = false;
            break;
            
        case VK_DOWN:
            m_downPressed = false;
            break;
            
        case VK_LEFT:
            m_leftPressed = false;
            break;
            
        case VK_RIGHT:
            m_rightPressed = false;
            break;
    }
}

bool DFRInputSystem::IsSpecialMoveTriggered(InputDirection& outDirection) {
    if (m_specialMoveReady && m_sKeyDown && m_sKeyHoldTime < 1.0f) {
        // Can't use special moves while blocking
        outDirection = m_pendingDirection;
        m_specialMoveReady = false;
        return true;
    }
    return false;
}

bool DFRInputSystem::IsGearSkillTriggered(int& outSkillIndex) {
    // Check gear skill combos
    if (m_asCombo) {
        outSkillIndex = 0; // AS - First skill of gear 0
        m_asCombo = false;
        ResetCombos();
        return true;
    }
    if (m_adCombo) {
        outSkillIndex = 2; // AD - First skill of gear 1
        m_adCombo = false;
        ResetCombos();
        return true;
    }
    if (m_sdCombo) {
        outSkillIndex = 4; // SD - First skill of gear 2
        m_sdCombo = false;
        ResetCombos();
        return true;
    }
    if (m_asdCombo) {
        outSkillIndex = 6; // ASD - First skill of gear 3
        m_asdCombo = false;
        ResetCombos();
        return true;
    }
    return false;
}

void DFRInputSystem::GetMovement(float& x, float& z) const {
    x = 0.0f;
    z = 0.0f;
    
    // Can't move while blocking
    if (IsBlockActive()) return;
    
    if (m_leftPressed) x -= 1.0f;
    if (m_rightPressed) x += 1.0f;
    if (m_upPressed) z += 1.0f;
    if (m_downPressed && !m_sKeyDown) z -= 1.0f; // Can't move down while holding S
    
    // Normalize diagonal movement
    if (x != 0.0f && z != 0.0f) {
        float length = sqrtf(x * x + z * z);
        x /= length;
        z /= length;
    }
}

void DFRInputSystem::CheckCombos() {
    // AS combo
    if (m_aKeyDown && m_sKeyDown && !m_asCombo) {
        m_asCombo = true;
        m_comboTimer = COMBO_WINDOW;
    }
    
    // AD combo
    if (m_aKeyDown && m_dKeyDown && !m_adCombo) {
        m_adCombo = true;
        m_comboTimer = COMBO_WINDOW;
    }
    
    // ASD combo (must have AS first)
    if (m_asCombo && m_dKeyDown && !m_asdCombo) {
        m_asdCombo = true;
        m_asCombo = false; // Clear AS since we have ASD
        m_comboTimer = COMBO_WINDOW;
    }
}

void DFRInputSystem::ResetCombos() {
    m_asCombo = false;
    m_adCombo = false;
    m_sdCombo = false;
    m_asdCombo = false;
    m_comboTimer = 0.0f;
}

// DFRGameApplication Implementation

DFRGameApplication::DFRGameApplication()
    : m_hwnd(nullptr)
    , m_screenWidth(1920)
    , m_screenHeight(1080)
    , m_currentState(DFRGameState::MainMenu)
    , m_activeGameMode(nullptr)
    , m_activePlayerIndex(0)
    , m_deltaTime(0)
    , m_isRunning(true)
    , m_cameraPosition(0, 10, -20)
    , m_cameraTarget(0, 0, 0) {
    
    QueryPerformanceFrequency(&m_frequency);
    QueryPerformanceCounter(&m_lastTime);
}

DFRGameApplication::~DFRGameApplication() {
    // Systems will clean up automatically through unique_ptr
}

bool DFRGameApplication::Initialize(HWND windowHandle, int width, int height) {
    m_hwnd = windowHandle;
    m_screenWidth = width;
    m_screenHeight = height;
    
    if (!InitializeDirectX()) {
        MessageBox(m_hwnd, L"Failed to initialize DirectX", L"Error", MB_OK);
        return false;
    }
    
    if (!InitializeGameSystems()) {
        MessageBox(m_hwnd, L"Failed to initialize game systems", L"Error", MB_OK);
        return false;
    }
    
    return true;
}

bool DFRGameApplication::InitializeDirectX() {
    // Create swap chain description
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 2;
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
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };
    D3D_FEATURE_LEVEL featureLevel;
    
    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    
    // Create device and swap chain
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    ComPtr<IDXGISwapChain> swapChain;
    
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
        featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION,
        &swapChainDesc, &swapChain, &device, &featureLevel, &context
    );
    
    if (FAILED(hr)) {
        return false;
    }
    
    m_device = device;
    m_deviceContext = context;
    m_swapChain = swapChain;
    
    // Create render target view
    ComPtr<ID3D11Texture2D> backBuffer;
    hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer);
    if (FAILED(hr)) return false;
    
    hr = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_renderTargetView);
    if (FAILED(hr)) return false;
    
    // Create depth stencil
    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = m_screenWidth;
    depthDesc.Height = m_screenHeight;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    
    ComPtr<ID3D11Texture2D> depthStencilBuffer;
    hr = m_device->CreateTexture2D(&depthDesc, nullptr, &depthStencilBuffer);
    if (FAILED(hr)) return false;
    
    hr = m_device->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, &m_depthStencilView);
    if (FAILED(hr)) return false;
    
    // Set render targets
    m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
    
    // Set viewport
    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(m_screenWidth);
    viewport.Height = static_cast<float>(m_screenHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    m_deviceContext->RSSetViewports(1, &viewport);
    
    return true;
}