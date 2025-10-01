#pragma once

#include "UISystem.h"
#include <functional>

namespace ArenaFighter {

class MainMenuScreen {
private:
    std::shared_ptr<UIPanel> m_rootPanel;
    std::shared_ptr<UILabel> m_titleLabel;
    std::shared_ptr<UIPanel> m_menuPanel;
    std::shared_ptr<UIPanel> m_gameModePanel;
    
    // Buttons
    std::shared_ptr<UIButton> m_playButton;
    std::shared_ptr<UIButton> m_shopButton;
    std::shared_ptr<UIButton> m_inventoryButton;
    std::shared_ptr<UIButton> m_settingsButton;
    std::shared_ptr<UIButton> m_quitButton;
    
    // Game mode buttons
    std::shared_ptr<UIButton> m_ranked1v1Button;
    std::shared_ptr<UIButton> m_forGloryButton;
    std::shared_ptr<UIButton> m_battleRoyaleButton;
    std::shared_ptr<UIButton> m_practiceButton;
    std::shared_ptr<UIButton> m_backButton;
    
    // Callbacks
    std::function<void(const std::string&)> m_onGameModeSelected;
    std::function<void()> m_onShopClicked;
    std::function<void()> m_onInventoryClicked;
    std::function<void()> m_onSettingsClicked;
    std::function<void()> m_onQuitClicked;
    
    bool m_showingGameModes;
    
public:
    MainMenuScreen(ID3D11Device* device, ID3D11DeviceContext* context);
    ~MainMenuScreen() = default;
    
    void initialize();
    void update(float deltaTime);
    
    std::shared_ptr<UIPanel> getRootPanel() { return m_rootPanel; }
    
    // Set callbacks
    void setCallbacks(
        std::function<void(const std::string&)> onGameMode,
        std::function<void()> onShop,
        std::function<void()> onInventory,
        std::function<void()> onSettings,
        std::function<void()> onQuit
    );
    
private:
    void createMainMenuButtons();
    void createGameModeButtons();
    void showGameModeSelection();
    void showMainMenu();
    void animateTransition(bool toGameModes);
};

} // namespace ArenaFighter