#include "GameApplicationAdapter.h"
#include "../Characters/CharacterFactory.h"
#include "../Shop/ItemManager.h"

namespace ArenaFighter {

bool DFRGameApplication::InitializeGameSystems() {
    // Initialize input system
    m_inputSystem = std::make_unique<DFRInputSystem>();
    
    // Initialize rendering system
    m_renderingSystem = std::make_unique<RenderingSystemAdapter::DFRRenderingSystem>(
        m_device, m_deviceContext);
    if (!m_renderingSystem->Initialize(m_screenWidth, m_screenHeight)) {
        return false;
    }
    
    // Initialize game mode manager
    m_gameModeManager = std::make_unique<GameModeManager>();
    
    // Initialize special move system
    m_specialMoveSystem = std::make_unique<SpecialMoveSystem>();
    
    // Initialize shop system
    m_shopSystem = std::make_unique<ShopSystemAdapter::DFRShopSystem>();
    m_shopSystem->SetPurchaseEventCallback(
        [this](const std::string& msg) { OnShopPurchase(msg); });
    m_shopSystem->SetItemEquippedCallback(
        [this](const DFRShopItem& item) { OnItemEquipped(item); });
    
    // Initialize UI system
    m_uiSystem = std::make_unique<UISystem>();
    if (!m_uiSystem->Initialize(m_device.Get(), m_deviceContext.Get(), 
                               m_screenWidth, m_screenHeight)) {
        return false;
    }
    
    // Initialize ranking system
    m_rankingSystem = std::make_unique<RankingSystem>();
    
    // Initialize gameplay manager adapter
    m_gameplayManager = std::make_unique<DFRGameplayManager>();
    
    // Connect all systems
    SystemIntegration::ConnectSystems(
        this, m_gameModeManager.get(), m_renderingSystem.get(),
        m_shopSystem.get(), m_specialMoveSystem.get(),
        m_uiSystem.get(), m_rankingSystem.get()
    );
    
    return true;
}

void DFRGameApplication::Run() {
    MSG msg = {0};
    
    while (m_isRunning) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                m_isRunning = false;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        if (!m_isRunning) break;
        
        UpdateDeltaTime();
        Update(m_deltaTime);
        Render();
    }
}

void DFRGameApplication::Update(float deltaTime) {
    // Update input system
    if (m_inputSystem) {
        m_inputSystem->Update(deltaTime);
    }
    
    // Update UI
    if (m_uiSystem) {
        m_uiSystem->Update(deltaTime);
    }
    
    // State-specific updates
    switch (m_currentState) {
        case DFRGameState::MainMenu:
            UpdateMainMenu(deltaTime);
            break;
            
        case DFRGameState::CharacterSelection:
            UpdateCharacterSelection(deltaTime);
            break;
            
        case DFRGameState::InGame:
            UpdateGameplay(deltaTime);
            break;
            
        case DFRGameState::Shop:
            UpdateShop(deltaTime);
            break;
            
        default:
            break;
    }
}

void DFRGameApplication::UpdateGameplay(float deltaTime) {
    if (!m_activeGameMode) return;
    
    // Handle input
    HandleGameplayInput();
    
    // Update special move system
    if (m_specialMoveSystem) {
        m_specialMoveSystem->Update(deltaTime);
    }
    
    // Update active game mode
    m_activeGameMode->Update(deltaTime);
    
    // Update characters with item effects
    auto activePlayer = GetActivePlayer();
    if (activePlayer) {
        ItemManager::GetInstance().UpdateConsumableEffects(activePlayer, deltaTime);
    }
    
    // Update particles
    if (m_renderingSystem) {
        m_renderingSystem->UpdateParticles(deltaTime);
    }
    
    // Update camera
    UpdateCamera();
    
    // Check for match end
    if (m_activeGameMode->GetMatchState() == MatchState::MatchEnd) {
        EndMatch();
    }
}

void DFRGameApplication::HandleGameplayInput() {
    if (!m_inputSystem) return;
    
    auto activePlayer = GetActivePlayer();
    if (!activePlayer) return;
    
    // Check for special moves (S+Direction, mana only)
    InputDirection specialDir;
    if (m_inputSystem->IsSpecialMoveTriggered(specialDir)) {
        HandleSpecialMove(specialDir);
    }
    
    // Check for gear skills (AS, AD, SD, ASD with cooldowns)
    int gearSkillIndex;
    if (m_inputSystem->IsGearSkillTriggered(gearSkillIndex)) {
        HandleGearSkill(gearSkillIndex);
    }
    
    // Handle movement
    float moveX, moveZ;
    m_inputSystem->GetMovement(moveX, moveZ);
    HandleMovement(moveX, moveZ);
    
    // Handle blocking
    HandleBlocking(m_inputSystem->IsBlockActive());
}

void DFRGameApplication::HandleSpecialMove(InputDirection direction) {
    auto player = GetActivePlayer();
    if (!player) return;
    
    // Special moves use mana only (no cooldowns)
    if (player->CanExecuteSpecialMove(direction)) {
        player->ExecuteSpecialMove(direction);
        
        // Create visual effect
        if (m_renderingSystem) {
            XMFLOAT3 playerPos(0, 0, 0); // Would get from world position
            m_renderingSystem->CreateSpecialMoveEffect(player, direction, playerPos);
        }
        
        // Apply damage to target
        auto enemy = GetActiveEnemy();
        if (enemy && m_specialMoveSystem) {
            float damage = CharacterStatsExtended::GetModifiedSpecialMoveDamage(
                player, direction);
            m_specialMoveSystem->ProcessSpecialMoveHit(player, enemy, direction);
        }
    }
}

void DFRGameApplication::HandleGearSkill(int skillIndex) {
    auto player = GetActivePlayer();
    if (!player) return;
    
    // Gear skills have both mana cost and cooldown
    if (!player->IsGearSkillOnCooldown(skillIndex)) {
        const auto& skill = player->GetGearSkills()[skillIndex];
        if (player->CanAffordSkill(skill.manaCost)) {
            player->ConsumeMana(skill.manaCost);
            player->StartGearSkillCooldown(skillIndex);
            
            // Apply item-modified cooldown
            float cooldown = ItemManager::GetInstance().CalculateGearSkillCooldown(
                player, skillIndex);
            // Would need to track this adjusted cooldown
            
            // Create visual effect
            if (m_renderingSystem) {
                XMFLOAT3 playerPos(0, 0, 0);
                m_renderingSystem->CreateGearSkillEffect(player, skillIndex, playerPos);
            }
            
            // Apply damage
            auto enemy = GetActiveEnemy();
            if (enemy) {
                float damage = CharacterStatsExtended::GetModifiedGearSkillDamage(
                    player, skillIndex);
                enemy->TakeDamage(damage);
            }
        }
    }
}

void DFRGameApplication::HandleMovement(float x, float z) {
    auto player = GetActivePlayer();
    if (!player || player->IsBlocking()) return;
    
    // Movement would be handled by physics/world system
    // For now, just track input
}

void DFRGameApplication::HandleBlocking(bool active) {
    auto player = GetActivePlayer();
    if (!player) return;
    
    if (active && !player->IsBlocking()) {
        player->StartBlocking();
        m_specialMoveSystem->OnBlockStart(player);
    }
    else if (!active && player->IsBlocking()) {
        player->StopBlocking();
        m_specialMoveSystem->OnBlockEnd(player);
    }
}

void DFRGameApplication::Render() {
    float clearColor[4] = { 0.05f, 0.05f, 0.1f, 1.0f };
    m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
    m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    
    switch (m_currentState) {
        case DFRGameState::MainMenu:
            RenderMainMenu();
            break;
            
        case DFRGameState::CharacterSelection:
            RenderCharacterSelection();
            break;
            
        case DFRGameState::InGame:
            RenderGameplay();
            break;
            
        case DFRGameState::Shop:
            RenderShop();
            break;
            
        default:
            break;
    }
    
    RenderUI();
    
    m_swapChain->Present(1, 0);
}

void DFRGameApplication::RenderGameplay() {
    if (!m_renderingSystem) return;
    
    // Update camera
    m_renderingSystem->UpdateCamera(m_cameraPosition, m_cameraTarget);
    
    // Render arena
    m_renderingSystem->RenderArena();
    
    // Render characters
    XMFLOAT3 playerPos(-5, 0, 0);
    XMFLOAT3 enemyPos(5, 0, 0);
    
    if (GetActivePlayer()) {
        m_renderingSystem->RenderCharacter(GetActivePlayer(), playerPos);
    }
    
    if (GetActiveEnemy()) {
        m_renderingSystem->RenderCharacter(GetActiveEnemy(), enemyPos);
    }
    
    // Render particles
    m_renderingSystem->RenderParticles();
}

void DFRGameApplication::RenderUI() {
    if (!m_uiSystem) return;
    
    m_uiSystem->BeginFrame();
    
    switch (m_currentState) {
        case DFRGameState::MainMenu:
            m_uiSystem->ShowMainMenu();
            break;
            
        case DFRGameState::CharacterSelection:
            m_uiSystem->ShowCharacterSelection();
            break;
            
        case DFRGameState::InGame:
            if (GetActivePlayer() && GetActiveEnemy()) {
                m_uiSystem->ShowCombatHUD(GetActivePlayer(), GetActiveEnemy());
            }
            break;
            
        case DFRGameState::Shop:
            // Shop UI would go here
            break;
    }
    
    m_uiSystem->EndFrame();
}

void DFRGameApplication::StartMatch(GameModeType modeType) {
    // Create game mode
    auto config = m_gameModeManager->GetModeConfig(modeType);
    m_activeGameMode = m_gameModeManager->CreateGameMode(modeType, config);
    
    // Create test characters
    m_playerTeam.clear();
    m_enemyTeam.clear();
    
    m_playerTeam.push_back(CharacterFactory::GetInstance().CreateCharacterByName(
        "Hyuk Woon Sung"));
    m_enemyTeam.push_back(CharacterFactory::GetInstance().CreateCharacterByName(
        "Hyuk Woon Sung"));
    
    // Initialize characters
    for (auto& character : m_playerTeam) {
        character->Initialize();
        ItemManager::GetInstance().InitializeCharacter(character.get());
    }
    
    for (auto& character : m_enemyTeam) {
        character->Initialize();
        ItemManager::GetInstance().InitializeCharacter(character.get());
    }
    
    ChangeState(DFRGameState::InGame);
}

void DFRGameApplication::EndMatch() {
    auto result = m_activeGameMode->GetMatchResult();
    
    // Update rankings
    if (m_rankingSystem) {
        for (const auto& playerResult : result.playerResults) {
            m_rankingSystem->recordMatchResult(
                playerResult.playerId,
                playerResult.opponentId,
                playerResult.characterId,
                playerResult.opponentCharacterId,
                playerResult.maxCombo,
                playerResult.damageDealt,
                result.matchDuration
            );
        }
    }
    
    m_activeGameMode = nullptr;
    ChangeState(DFRGameState::PostMatch);
}

void DFRGameApplication::UpdateDeltaTime() {
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    m_deltaTime = static_cast<float>(currentTime.QuadPart - m_lastTime.QuadPart) /
                 static_cast<float>(m_frequency.QuadPart);
    m_lastTime = currentTime;
    if (m_deltaTime > 0.1f) m_deltaTime = 0.1f; // Cap at 100ms
}

void DFRGameApplication::UpdateCamera() {
    // Simple third-person camera
    m_cameraPosition = XMFLOAT3(0, 10, -20);
    m_cameraTarget = XMFLOAT3(0, 2, 0);
}

CharacterBase* DFRGameApplication::GetActivePlayer() {
    if (m_playerTeam.empty() || m_activePlayerIndex >= m_playerTeam.size()) {
        return nullptr;
    }
    return m_playerTeam[m_activePlayerIndex].get();
}

CharacterBase* DFRGameApplication::GetActiveEnemy() {
    if (m_enemyTeam.empty()) {
        return nullptr;
    }
    return m_enemyTeam[0].get();
}

void DFRGameApplication::ChangeState(DFRGameState newState) {
    m_currentState = newState;
    
    // State-specific initialization
    switch (newState) {
        case DFRGameState::CharacterSelection:
            // Initialize character selection
            break;
            
        case DFRGameState::Shop:
            // Initialize shop display
            break;
    }
}

// Callbacks

void DFRGameApplication::OnMatchEvent(const std::string& event) {
    // Display match event in UI
    if (m_uiSystem) {
        // m_uiSystem->ShowNotification(event);
    }
}

void DFRGameApplication::OnMatchEnd(const MatchResult& result) {
    EndMatch();
}

void DFRGameApplication::OnShopPurchase(const std::string& message) {
    // Show purchase result
    MessageBoxA(m_hwnd, message.c_str(), "Shop", MB_OK);
}

void DFRGameApplication::OnItemEquipped(const DFRShopItem& item) {
    // Apply item to active character
    auto player = GetActivePlayer();
    if (player) {
        auto equipped = m_shopSystem->GetEquippedItems();
        ItemManager::GetInstance().ApplyEquippedItems(player, equipped);
    }
}

// Input handlers

void DFRGameApplication::OnKeyDown(int key) {
    if (m_inputSystem) {
        m_inputSystem->OnKeyDown(key);
    }
    
    // State-specific key handling
    switch (m_currentState) {
        case DFRGameState::MainMenu:
            if (key == VK_RETURN) {
                StartMatch(GameModeType::Versus);
            }
            break;
    }
}

void DFRGameApplication::OnKeyUp(int key) {
    if (m_inputSystem) {
        m_inputSystem->OnKeyUp(key);
    }
}

void DFRGameApplication::OnMouseMove(int x, int y) {
    if (m_uiSystem) {
        m_uiSystem->OnMouseMove(x, y);
    }
}

void DFRGameApplication::OnMouseClick(int button, int x, int y) {
    if (m_uiSystem) {
        m_uiSystem->OnMouseClick(button, x, y);
    }
}

void DFRGameApplication::Shutdown() {
    m_isRunning = false;
}

// Stub implementations for menu updates

void DFRGameApplication::UpdateMainMenu(float deltaTime) {
    // Main menu logic
}

void DFRGameApplication::UpdateCharacterSelection(float deltaTime) {
    // Character selection logic
}

void DFRGameApplication::UpdateShop(float deltaTime) {
    // Shop logic
}

void DFRGameApplication::RenderMainMenu() {
    // Render main menu background
}

void DFRGameApplication::RenderCharacterSelection() {
    // Render character selection
}

void DFRGameApplication::RenderShop() {
    // Render shop
}

// SystemIntegration Implementation

void SystemIntegration::ConnectSystems(
    DFRGameApplication* app,
    GameModeManager* modeManager,
    RenderingSystemAdapter::DFRRenderingSystem* renderer,
    ShopSystemAdapter::DFRShopSystem* shop,
    SpecialMoveSystem* specialMoves,
    UISystem* ui,
    RankingSystem* ranking) {
    
    // Set up callbacks and connections between systems
    // This would be expanded based on actual system needs
}

void SystemIntegration::ApplyShopItemsToCharacter(
    CharacterBase* character,
    const std::vector<DFRShopItem>& equippedItems) {
    
    ItemManager::GetInstance().ApplyEquippedItems(character, equippedItems);
}

void SystemIntegration::CreateCombatEffects(
    RenderingSystemAdapter::DFRRenderingSystem* renderer,
    CharacterBase* attacker,
    CharacterBase* target,
    bool isSpecialMove,
    int skillIndex) {
    
    if (!renderer || !attacker || !target) return;
    
    XMFLOAT3 targetPos(0, 0, 0); // Would get from world position
    
    if (isSpecialMove) {
        // Special move effects are based on element
        renderer->CreateHitEffect(targetPos, 100.0f);
    } else {
        // Gear skill effects
        renderer->CreateGearSkillEffect(attacker, skillIndex, targetPos);
    }
}

} // namespace ArenaFighter