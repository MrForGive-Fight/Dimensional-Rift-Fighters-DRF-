#include "MainMenu.h"

namespace ArenaFighter {

MainMenuScreen::MainMenuScreen(ID3D11Device* device, ID3D11DeviceContext* context)
    : m_showingGameModes(false) {
    initialize();
}

void MainMenuScreen::initialize() {
    // Create root panel
    m_rootPanel = std::make_shared<UIPanel>("MainMenuRoot", XMFLOAT2(0, 0), XMFLOAT2(1920, 1080));
    m_rootPanel->setBackgroundColor(XMFLOAT4(0.05f, 0.05f, 0.1f, 1.0f));
    
    // Create title
    m_titleLabel = std::make_shared<UILabel>("Title", XMFLOAT2(960, 100), "DFR - ANIME ARENA FIGHTER", 72.0f);
    m_titleLabel->setAnchor(UIAnchor::TopCenter);
    m_titleLabel->setTextColor(XMFLOAT4(0.9f, 0.9f, 1.0f, 1.0f));
    m_titleLabel->setAlignment(true, false);
    m_rootPanel->addChild(m_titleLabel);
    
    // Create main menu panel
    m_menuPanel = std::make_shared<UIPanel>("MenuPanel", XMFLOAT2(760, 300), XMFLOAT2(400, 500));
    m_menuPanel->setBackgroundColor(XMFLOAT4(0.1f, 0.1f, 0.15f, 0.9f));
    m_menuPanel->setBorder(true, XMFLOAT4(0.3f, 0.5f, 0.8f, 1.0f), 3.0f);
    m_rootPanel->addChild(m_menuPanel);
    
    // Create game mode selection panel (hidden initially)
    m_gameModePanel = std::make_shared<UIPanel>("GameModePanel", XMFLOAT2(660, 300), XMFLOAT2(600, 500));
    m_gameModePanel->setBackgroundColor(XMFLOAT4(0.1f, 0.1f, 0.15f, 0.9f));
    m_gameModePanel->setBorder(true, XMFLOAT4(0.3f, 0.5f, 0.8f, 1.0f), 3.0f);
    m_gameModePanel->setVisible(false);
    m_rootPanel->addChild(m_gameModePanel);
    
    createMainMenuButtons();
    createGameModeButtons();
}

void MainMenuScreen::createMainMenuButtons() {
    float buttonY = 50;
    float buttonSpacing = 90;
    
    // Play button
    m_playButton = std::make_shared<UIButton>("PlayButton", XMFLOAT2(50, buttonY), XMFLOAT2(300, 70), "PLAY");
    m_playButton->setFontSize(28.0f);
    m_playButton->setColors(
        XMFLOAT4(0.2f, 0.3f, 0.6f, 1.0f),   // Normal
        XMFLOAT4(0.3f, 0.4f, 0.7f, 1.0f),   // Hover
        XMFLOAT4(0.15f, 0.25f, 0.5f, 1.0f), // Pressed
        XMFLOAT4(0.2f, 0.2f, 0.2f, 0.5f)    // Disabled
    );
    m_playButton->setOnClick([this]() { showGameModeSelection(); });
    m_menuPanel->addChild(m_playButton);
    
    buttonY += buttonSpacing;
    
    // Shop button
    m_shopButton = std::make_shared<UIButton>("ShopButton", XMFLOAT2(50, buttonY), XMFLOAT2(300, 70), "SHOP");
    m_shopButton->setFontSize(28.0f);
    m_shopButton->setColors(
        XMFLOAT4(0.3f, 0.2f, 0.5f, 1.0f),
        XMFLOAT4(0.4f, 0.3f, 0.6f, 1.0f),
        XMFLOAT4(0.25f, 0.15f, 0.4f, 1.0f),
        XMFLOAT4(0.2f, 0.2f, 0.2f, 0.5f)
    );
    m_shopButton->setOnClick([this]() { 
        if (m_onShopClicked) m_onShopClicked(); 
    });
    m_menuPanel->addChild(m_shopButton);
    
    buttonY += buttonSpacing;
    
    // Inventory button
    m_inventoryButton = std::make_shared<UIButton>("InventoryButton", XMFLOAT2(50, buttonY), XMFLOAT2(300, 70), "EQUIPMENT");
    m_inventoryButton->setFontSize(28.0f);
    m_inventoryButton->setColors(
        XMFLOAT4(0.2f, 0.4f, 0.3f, 1.0f),
        XMFLOAT4(0.3f, 0.5f, 0.4f, 1.0f),
        XMFLOAT4(0.15f, 0.3f, 0.25f, 1.0f),
        XMFLOAT4(0.2f, 0.2f, 0.2f, 0.5f)
    );
    m_inventoryButton->setOnClick([this]() { 
        if (m_onInventoryClicked) m_onInventoryClicked(); 
    });
    m_menuPanel->addChild(m_inventoryButton);
    
    buttonY += buttonSpacing;
    
    // Settings button
    m_settingsButton = std::make_shared<UIButton>("SettingsButton", XMFLOAT2(50, buttonY), XMFLOAT2(300, 70), "SETTINGS");
    m_settingsButton->setFontSize(28.0f);
    m_settingsButton->setColors(
        XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),
        XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f),
        XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f),
        XMFLOAT4(0.2f, 0.2f, 0.2f, 0.5f)
    );
    m_settingsButton->setOnClick([this]() { 
        if (m_onSettingsClicked) m_onSettingsClicked(); 
    });
    m_menuPanel->addChild(m_settingsButton);
    
    buttonY += buttonSpacing;
    
    // Quit button
    m_quitButton = std::make_shared<UIButton>("QuitButton", XMFLOAT2(50, buttonY), XMFLOAT2(300, 70), "QUIT");
    m_quitButton->setFontSize(28.0f);
    m_quitButton->setColors(
        XMFLOAT4(0.5f, 0.2f, 0.2f, 1.0f),
        XMFLOAT4(0.6f, 0.3f, 0.3f, 1.0f),
        XMFLOAT4(0.4f, 0.15f, 0.15f, 1.0f),
        XMFLOAT4(0.2f, 0.2f, 0.2f, 0.5f)
    );
    m_quitButton->setOnClick([this]() { 
        if (m_onQuitClicked) m_onQuitClicked(); 
    });
    m_menuPanel->addChild(m_quitButton);
}

void MainMenuScreen::createGameModeButtons() {
    // Title for game modes
    auto gameModeTitle = std::make_shared<UILabel>("GameModeTitle", XMFLOAT2(300, 20), "SELECT GAME MODE", 36.0f);
    gameModeTitle->setTextColor(XMFLOAT4(0.9f, 0.9f, 1.0f, 1.0f));
    gameModeTitle->setAlignment(true, false);
    m_gameModePanel->addChild(gameModeTitle);
    
    float buttonY = 80;
    float buttonSpacing = 90;
    
    // Ranked 1v1
    m_ranked1v1Button = std::make_shared<UIButton>("Ranked1v1", XMFLOAT2(100, buttonY), XMFLOAT2(400, 70), "RANKED 1v1");
    m_ranked1v1Button->setFontSize(24.0f);
    m_ranked1v1Button->setColors(
        XMFLOAT4(0.4f, 0.2f, 0.2f, 1.0f),
        XMFLOAT4(0.5f, 0.3f, 0.3f, 1.0f),
        XMFLOAT4(0.3f, 0.15f, 0.15f, 1.0f),
        XMFLOAT4(0.2f, 0.2f, 0.2f, 0.5f)
    );
    m_ranked1v1Button->setOnClick([this]() { 
        if (m_onGameModeSelected) m_onGameModeSelected("Ranked1v1"); 
    });
    m_gameModePanel->addChild(m_ranked1v1Button);
    
    buttonY += buttonSpacing;
    
    // For Glory 2v2
    m_forGloryButton = std::make_shared<UIButton>("ForGlory", XMFLOAT2(100, buttonY), XMFLOAT2(400, 70), "FOR GLORY (2v2)");
    m_forGloryButton->setFontSize(24.0f);
    m_forGloryButton->setColors(
        XMFLOAT4(0.2f, 0.3f, 0.5f, 1.0f),
        XMFLOAT4(0.3f, 0.4f, 0.6f, 1.0f),
        XMFLOAT4(0.15f, 0.25f, 0.4f, 1.0f),
        XMFLOAT4(0.2f, 0.2f, 0.2f, 0.5f)
    );
    m_forGloryButton->setOnClick([this]() { 
        if (m_onGameModeSelected) m_onGameModeSelected("ForGlory"); 
    });
    m_gameModePanel->addChild(m_forGloryButton);
    
    buttonY += buttonSpacing;
    
    // Battle Royale
    m_battleRoyaleButton = std::make_shared<UIButton>("BattleRoyale", XMFLOAT2(100, buttonY), XMFLOAT2(400, 70), "BATTLE ROYALE");
    m_battleRoyaleButton->setFontSize(24.0f);
    m_battleRoyaleButton->setColors(
        XMFLOAT4(0.4f, 0.3f, 0.2f, 1.0f),
        XMFLOAT4(0.5f, 0.4f, 0.3f, 1.0f),
        XMFLOAT4(0.3f, 0.25f, 0.15f, 1.0f),
        XMFLOAT4(0.2f, 0.2f, 0.2f, 0.5f)
    );
    m_battleRoyaleButton->setOnClick([this]() { 
        if (m_onGameModeSelected) m_onGameModeSelected("BattleRoyale"); 
    });
    m_gameModePanel->addChild(m_battleRoyaleButton);
    
    buttonY += buttonSpacing;
    
    // Practice Mode
    m_practiceButton = std::make_shared<UIButton>("Practice", XMFLOAT2(100, buttonY), XMFLOAT2(400, 70), "PRACTICE MODE");
    m_practiceButton->setFontSize(24.0f);
    m_practiceButton->setColors(
        XMFLOAT4(0.2f, 0.4f, 0.3f, 1.0f),
        XMFLOAT4(0.3f, 0.5f, 0.4f, 1.0f),
        XMFLOAT4(0.15f, 0.3f, 0.25f, 1.0f),
        XMFLOAT4(0.2f, 0.2f, 0.2f, 0.5f)
    );
    m_practiceButton->setOnClick([this]() { 
        if (m_onGameModeSelected) m_onGameModeSelected("Practice"); 
    });
    m_gameModePanel->addChild(m_practiceButton);
    
    buttonY += buttonSpacing;
    
    // Back button
    m_backButton = std::make_shared<UIButton>("BackButton", XMFLOAT2(100, buttonY), XMFLOAT2(400, 70), "BACK");
    m_backButton->setFontSize(24.0f);
    m_backButton->setColors(
        XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),
        XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f),
        XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f),
        XMFLOAT4(0.2f, 0.2f, 0.2f, 0.5f)
    );
    m_backButton->setOnClick([this]() { showMainMenu(); });
    m_gameModePanel->addChild(m_backButton);
}

void MainMenuScreen::showGameModeSelection() {
    m_showingGameModes = true;
    animateTransition(true);
}

void MainMenuScreen::showMainMenu() {
    m_showingGameModes = false;
    animateTransition(false);
}

void MainMenuScreen::animateTransition(bool toGameModes) {
    if (toGameModes) {
        m_menuPanel->setVisible(false);
        m_gameModePanel->setVisible(true);
    } else {
        m_gameModePanel->setVisible(false);
        m_menuPanel->setVisible(true);
    }
}

void MainMenuScreen::update(float deltaTime) {
    // Could add animations here
}

void MainMenuScreen::setCallbacks(
    std::function<void(const std::string&)> onGameMode,
    std::function<void()> onShop,
    std::function<void()> onInventory,
    std::function<void()> onSettings,
    std::function<void()> onQuit) {
    
    m_onGameModeSelected = onGameMode;
    m_onShopClicked = onShop;
    m_onInventoryClicked = onInventory;
    m_onSettingsClicked = onSettings;
    m_onQuitClicked = onQuit;
}

} // namespace ArenaFighter