#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <memory>
#include <string>
#include <vector>

#include "../GameModes/GameModeManager.h"
#include "../Rendering/RenderingSystemAdapter.h"
#include "../Shop/ShopSystemAdapter.h"
#include "../Integration/GameplayManagerAdapter.h"
#include "../Combat/SpecialMoveSystem.h"
#include "../UI/UISystem.h"
#include "../Ranking/RankingSystem.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

namespace ArenaFighter {

using Microsoft::WRL::ComPtr;
using namespace DirectX;

/**
 * @brief Game states aligned with DFR design
 */
enum class DFRGameState {
    MainMenu,
    ModeSelection,
    CharacterSelection,
    InGame,
    Equipment,
    Shop,
    Rankings,
    Settings,
    PostMatch
};

/**
 * @brief Input system that properly handles DFR's skill distinction
 */
class DFRInputSystem {
private:
    // Key states
    bool m_sKeyDown;
    float m_sKeyHoldTime;
    bool m_aKeyDown;
    bool m_dKeyDown;
    bool m_upPressed, m_downPressed, m_leftPressed, m_rightPressed;
    
    // Special move tracking
    bool m_specialMoveReady;
    InputDirection m_pendingDirection;
    
    // Gear skill tracking
    bool m_asCombo;  // A then S
    bool m_adCombo;  // A then D
    bool m_sdCombo;  // S then D
    bool m_asdCombo; // A then S then D
    
    float m_comboTimer;
    static constexpr float COMBO_WINDOW = 0.3f; // 300ms to complete combos
    
public:
    DFRInputSystem()
        : m_sKeyDown(false), m_sKeyHoldTime(0.0f)
        , m_aKeyDown(false), m_dKeyDown(false)
        , m_upPressed(false), m_downPressed(false)
        , m_leftPressed(false), m_rightPressed(false)
        , m_specialMoveReady(false), m_pendingDirection(InputDirection::Up)
        , m_asCombo(false), m_adCombo(false), m_sdCombo(false), m_asdCombo(false)
        , m_comboTimer(0.0f) {}
    
    void Update(float deltaTime);
    void OnKeyDown(int key);
    void OnKeyUp(int key);
    
    // Special move queries (S+Direction, mana only)
    bool IsSpecialMoveTriggered(InputDirection& outDirection);
    
    // Gear skill queries (AS, AD, SD, ASD with cooldowns)
    bool IsGearSkillTriggered(int& outSkillIndex);
    
    // Block query
    bool IsBlockActive() const { return m_sKeyDown && m_sKeyHoldTime >= 1.0f; }
    float GetBlockHoldTime() const { return m_sKeyHoldTime; }
    
    // Movement
    void GetMovement(float& x, float& z) const;
    
private:
    void CheckCombos();
    void ResetCombos();
};

/**
 * @brief Main game application using DFR systems
 */
class DFRGameApplication {
private:
    // Window
    HWND m_hwnd;
    int m_screenWidth;
    int m_screenHeight;
    
    // DirectX with ComPtr
    ComPtr<ID3D11Device> m_device;
    ComPtr<ID3D11DeviceContext> m_deviceContext;
    ComPtr<IDXGISwapChain> m_swapChain;
    ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    ComPtr<ID3D11DepthStencilView> m_depthStencilView;
    
    // DFR Game Systems
    std::unique_ptr<GameModeManager> m_gameModeManager;
    std::unique_ptr<RenderingSystemAdapter::DFRRenderingSystem> m_renderingSystem;
    std::unique_ptr<ShopSystemAdapter::DFRShopSystem> m_shopSystem;
    std::unique_ptr<SpecialMoveSystem> m_specialMoveSystem;
    std::unique_ptr<UISystem> m_uiSystem;
    std::unique_ptr<RankingSystem> m_rankingSystem;
    std::unique_ptr<DFRInputSystem> m_inputSystem;
    
    // Adapters for provided systems
    std::unique_ptr<DFRGameplayManager> m_gameplayManager;
    
    // Game state
    DFRGameState m_currentState;
    GameMode* m_activeGameMode;
    std::vector<std::unique_ptr<CharacterBase>> m_playerTeam;
    std::vector<std::unique_ptr<CharacterBase>> m_enemyTeam;
    int m_activePlayerIndex;
    
    // Timing
    float m_deltaTime;
    LARGE_INTEGER m_frequency;
    LARGE_INTEGER m_lastTime;
    bool m_isRunning;
    
    // Camera
    XMFLOAT3 m_cameraPosition;
    XMFLOAT3 m_cameraTarget;
    
public:
    DFRGameApplication();
    ~DFRGameApplication();
    
    bool Initialize(HWND windowHandle, int width, int height);
    void Run();
    void Shutdown();
    
    // Input handling
    void OnKeyDown(int key);
    void OnKeyUp(int key);
    void OnMouseMove(int x, int y);
    void OnMouseClick(int button, int x, int y);
    
private:
    // Initialization
    bool InitializeDirectX();
    bool InitializeGameSystems();
    
    // Update
    void Update(float deltaTime);
    void UpdateMainMenu(float deltaTime);
    void UpdateCharacterSelection(float deltaTime);
    void UpdateGameplay(float deltaTime);
    void UpdateShop(float deltaTime);
    
    // Render
    void Render();
    void RenderMainMenu();
    void RenderCharacterSelection();
    void RenderGameplay();
    void RenderShop();
    void RenderUI();
    
    // Gameplay
    void HandleGameplayInput();
    void HandleSpecialMove(InputDirection direction);
    void HandleGearSkill(int skillIndex);
    void HandleMovement(float x, float z);
    void HandleBlocking(bool active);
    
    // State transitions
    void ChangeState(DFRGameState newState);
    void StartMatch(GameModeType modeType);
    void EndMatch();
    
    // Utility
    void UpdateDeltaTime();
    void UpdateCamera();
    CharacterBase* GetActivePlayer();
    CharacterBase* GetActiveEnemy();
    
    // Callbacks
    void OnMatchEvent(const std::string& event);
    void OnMatchEnd(const MatchResult& result);
    void OnShopPurchase(const std::string& message);
    void OnItemEquipped(const DFRShopItem& item);
};

/**
 * @brief Integration helper to connect all systems
 */
class SystemIntegration {
public:
    static void ConnectSystems(
        DFRGameApplication* app,
        GameModeManager* modeManager,
        RenderingSystemAdapter::DFRRenderingSystem* renderer,
        ShopSystemAdapter::DFRShopSystem* shop,
        SpecialMoveSystem* specialMoves,
        UISystem* ui,
        RankingSystem* ranking
    );
    
    static void ApplyShopItemsToCharacter(
        CharacterBase* character,
        const std::vector<DFRShopItem>& equippedItems
    );
    
    static void CreateCombatEffects(
        RenderingSystemAdapter::DFRRenderingSystem* renderer,
        CharacterBase* attacker,
        CharacterBase* target,
        bool isSpecialMove,
        int skillIndex = -1
    );
};

} // namespace ArenaFighter