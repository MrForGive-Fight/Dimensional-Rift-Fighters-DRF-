#include "MainMenuScreen.h"

namespace ArenaFighter {

MainMenuScreen::MainMenuScreen(ID3D11Device* dev, ID3D11DeviceContext* ctx)
    : m_device(dev), m_context(ctx) {
    buildUI();
}

void MainMenuScreen::setCallbacks(
    std::function<void(const std::string&)> modeCallback,
    std::function<void()> shopCallback,
    std::function<void()> inventoryCallback
) {
    m_onModeSelected = modeCallback;
    m_onShopClicked = shopCallback;
    m_onInventoryClicked = inventoryCallback;
}

void MainMenuScreen::buildUI() {
    m_rootPanel = std::make_shared<UIPanel>("MainMenu", XMFLOAT2(0, 0), XMFLOAT2(1920, 1080));
    m_rootPanel->setBackgroundColor(XMFLOAT4(0.05f, 0.05f, 0.1f, 1.0f));
    m_rootPanel->setBorder(false);

    auto titleBanner = createTitleBanner();
    m_rootPanel->addChild(titleBanner);

    auto leftPanel = createCharacterPanel();
    m_rootPanel->addChild(leftPanel);

    auto centerPanel = createGameModesPanel();
    m_rootPanel->addChild(centerPanel);

    auto rightPanel = createFeaturesPanel();
    m_rootPanel->addChild(rightPanel);

    auto bottomPanel = createBottomPanel();
    m_rootPanel->addChild(bottomPanel);
}

std::shared_ptr<UIPanel> MainMenuScreen::createTitleBanner() {
    auto banner = std::make_shared<UIPanel>("TitleBanner", XMFLOAT2(0, 0), XMFLOAT2(1920, 100));
    banner->setBackgroundColor(XMFLOAT4(0.1f, 0.15f, 0.25f, 0.95f));

    auto title = std::make_shared<UILabel>("GameTitle", XMFLOAT2(50, 30), "ARENA FIGHTER", 42.0f);
    title->setTextColor(XMFLOAT4(0.9f, 0.7f, 0.2f, 1.0f));
    banner->addChild(title);

    auto status = std::make_shared<UILabel>("ServerStatus", XMFLOAT2(1600, 40), "Server: Online", 18.0f);
    status->setTextColor(XMFLOAT4(0.2f, 0.9f, 0.2f, 1.0f));
    banner->addChild(status);

    return banner;
}

std::shared_ptr<UIPanel> MainMenuScreen::createCharacterPanel() {
    auto panel = std::make_shared<UIPanel>("CharacterPanel", XMFLOAT2(20, 120), XMFLOAT2(350, 600));
    panel->setBackgroundColor(XMFLOAT4(0.08f, 0.12f, 0.18f, 0.9f));

    auto nameLabel = std::make_shared<UILabel>("CharName", XMFLOAT2(20, 20), "Player Name", 24.0f);
    nameLabel->setTextColor(XMFLOAT4(1.0f, 0.9f, 0.3f, 1.0f));
    panel->addChild(nameLabel);

    auto levelLabel = std::make_shared<UILabel>("Level", XMFLOAT2(20, 50), "Level: 25", 18.0f);
    panel->addChild(levelLabel);

    auto expBar = std::make_shared<UIProgressBar>("ExpBar", XMFLOAT2(20, 80), XMFLOAT2(310, 25), 100.0f);
    expBar->setValue(65.0f);
    expBar->setFillColor(XMFLOAT4(0.3f, 0.6f, 0.9f, 1.0f));
    panel->addChild(expBar);

    auto guildBtn = std::make_shared<UIButton>("GuildBtn", XMFLOAT2(20, 500), XMFLOAT2(150, 40), "Guild Info");
    guildBtn->setColors(
        XMFLOAT4(0.25f, 0.35f, 0.5f, 1.0f),
        XMFLOAT4(0.35f, 0.45f, 0.6f, 1.0f),
        XMFLOAT4(0.2f, 0.3f, 0.45f, 1.0f)
    );
    panel->addChild(guildBtn);

    auto infoBtn = std::make_shared<UIButton>("InfoBtn", XMFLOAT2(180, 500), XMFLOAT2(150, 40), "Info");
    infoBtn->setColors(
        XMFLOAT4(0.25f, 0.35f, 0.5f, 1.0f),
        XMFLOAT4(0.35f, 0.45f, 0.6f, 1.0f),
        XMFLOAT4(0.2f, 0.3f, 0.45f, 1.0f)
    );
    panel->addChild(infoBtn);

    return panel;
}

std::shared_ptr<UIPanel> MainMenuScreen::createGameModesPanel() {
    auto panel = std::make_shared<UIPanel>("GameModesPanel", XMFLOAT2(390, 120), XMFLOAT2(700, 600));
    panel->setBackgroundColor(XMFLOAT4(0.08f, 0.12f, 0.18f, 0.9f));

    auto playBtn = std::make_shared<UIButton>("PlayBtn", XMFLOAT2(200, 50), XMFLOAT2(300, 80), "PLAY");
    playBtn->setColors(
        XMFLOAT4(0.8f, 0.2f, 0.2f, 1.0f),
        XMFLOAT4(0.9f, 0.3f, 0.3f, 1.0f),
        XMFLOAT4(0.7f, 0.15f, 0.15f, 1.0f)
    );
    panel->addChild(playBtn);

    std::vector<std::pair<std::string, std::string>> modes = {
        {"Ranked1v1", "Ranked 1v1"},
        {"TeamDeath", "Team Deathmatch"},
        {"BeastMode", "Beast Mode"},
        {"ForGlory", "For Glory"},
        {"DimRift", "Dimensional Rift"},
        {"Tournament", "Tournament"}
    };

    float startY = 160;
    float buttonHeight = 60;
    float spacing = 10;

    for (size_t i = 0; i < modes.size(); i++) {
        auto modeBtn = std::make_shared<UIButton>(
            modes[i].first,
            XMFLOAT2(50, startY + i * (buttonHeight + spacing)),
            XMFLOAT2(600, buttonHeight),
            modes[i].second
        );
        
        modeBtn->setColors(
            XMFLOAT4(0.15f, 0.25f, 0.4f, 1.0f),
            XMFLOAT4(0.25f, 0.35f, 0.5f, 1.0f),
            XMFLOAT4(0.1f, 0.2f, 0.35f, 1.0f)
        );

        std::string modeName = modes[i].first;
        modeBtn->setOnClick([this, modeName]() {
            if (m_onModeSelected) m_onModeSelected(modeName);
        });

        panel->addChild(modeBtn);
    }

    return panel;
}

std::shared_ptr<UIPanel> MainMenuScreen::createFeaturesPanel() {
    auto panel = std::make_shared<UIPanel>("FeaturesPanel", XMFLOAT2(1110, 120), XMFLOAT2(790, 600));
    panel->setBackgroundColor(XMFLOAT4(0.08f, 0.12f, 0.18f, 0.9f));

    struct FeatureButton {
        std::string id;
        std::string label;
        XMFLOAT2 position;
    };

    std::vector<FeatureButton> features = {
        {"Shop", "SHOP", XMFLOAT2(50, 50)},
        {"Inventory", "INVENTORY", XMFLOAT2(230, 50)},
        {"Tasks", "TASKS", XMFLOAT2(410, 50)},
        {"Energy", "ENERGY", XMFLOAT2(590, 50)},
        {"Gifts", "GIFTS", XMFLOAT2(50, 150)},
        {"Social", "SOCIAL", XMFLOAT2(230, 150)},
        {"Ranking", "RANKING", XMFLOAT2(410, 150)},
        {"Settings", "SETTINGS", XMFLOAT2(590, 150)}
    };

    for (const auto& feature : features) {
        auto btn = std::make_shared<UIButton>(
            feature.id,
            feature.position,
            XMFLOAT2(160, 80),
            feature.label
        );
        
        btn->setColors(
            XMFLOAT4(0.2f, 0.3f, 0.45f, 1.0f),
            XMFLOAT4(0.3f, 0.4f, 0.55f, 1.0f),
            XMFLOAT4(0.15f, 0.25f, 0.4f, 1.0f)
        );

        if (feature.id == "Shop") {
            btn->setOnClick([this]() {
                if (m_onShopClicked) m_onShopClicked();
            });
        } else if (feature.id == "Inventory") {
            btn->setOnClick([this]() {
                if (m_onInventoryClicked) m_onInventoryClicked();
            });
        }

        panel->addChild(btn);
    }

    auto eventsPanel = std::make_shared<UIPanel>("EventsPanel", XMFLOAT2(50, 280), XMFLOAT2(690, 280));
    eventsPanel->setBackgroundColor(XMFLOAT4(0.1f, 0.15f, 0.2f, 0.8f));
    
    auto eventsTitle = std::make_shared<UILabel>("EventsTitle", XMFLOAT2(20, 20), "Current Events", 22.0f);
    eventsTitle->setTextColor(XMFLOAT4(0.9f, 0.7f, 0.2f, 1.0f));
    eventsPanel->addChild(eventsTitle);

    auto noEvents = std::make_shared<UILabel>("NoEvents", XMFLOAT2(20, 60), "No Events - Look forward to the next events!", 16.0f);
    noEvents->setTextColor(XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f));
    eventsPanel->addChild(noEvents);

    panel->addChild(eventsPanel);

    return panel;
}

std::shared_ptr<UIPanel> MainMenuScreen::createBottomPanel() {
    auto panel = std::make_shared<UIPanel>("BottomPanel", XMFLOAT2(0, 940), XMFLOAT2(1920, 140));
    panel->setBackgroundColor(XMFLOAT4(0.05f, 0.08f, 0.12f, 0.95f));

    auto chatBg = std::make_shared<UIPanel>("ChatBg", XMFLOAT2(20, 20), XMFLOAT2(1400, 80));
    chatBg->setBackgroundColor(XMFLOAT4(0.08f, 0.08f, 0.1f, 0.9f));
    panel->addChild(chatBg);

    std::vector<std::string> controls = {"Recording", "Streaming", "Settings"};
    float btnX = 1450;
    
    for (const auto& ctrl : controls) {
        auto btn = std::make_shared<UIButton>(
            ctrl + "Btn",
            XMFLOAT2(btnX, 30),
            XMFLOAT2(140, 60),
            ctrl
        );
        btn->setColors(
            XMFLOAT4(0.2f, 0.25f, 0.3f, 1.0f),
            XMFLOAT4(0.3f, 0.35f, 0.4f, 1.0f),
            XMFLOAT4(0.15f, 0.2f, 0.25f, 1.0f)
        );
        panel->addChild(btn);
        btnX += 150;
    }

    return panel;
}

} // namespace ArenaFighter