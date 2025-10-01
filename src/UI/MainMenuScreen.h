#pragma once

#include "UIPanel.h"
#include "UIButton.h"
#include "UILabel.h"
#include "UIProgressBar.h"
#include <functional>
#include <memory>
#include <d3d11.h>

namespace ArenaFighter {

class MainMenuScreen {
private:
    std::shared_ptr<UIPanel> m_rootPanel;
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_context;

    std::function<void(const std::string&)> m_onModeSelected;
    std::function<void()> m_onShopClicked;
    std::function<void()> m_onInventoryClicked;

public:
    MainMenuScreen(ID3D11Device* dev, ID3D11DeviceContext* ctx);
    
    std::shared_ptr<UIPanel> getRootPanel() { return m_rootPanel; }
    
    void setCallbacks(
        std::function<void(const std::string&)> modeCallback,
        std::function<void()> shopCallback,
        std::function<void()> inventoryCallback
    );

private:
    void buildUI();
    std::shared_ptr<UIPanel> createTitleBanner();
    std::shared_ptr<UIPanel> createCharacterPanel();
    std::shared_ptr<UIPanel> createGameModesPanel();
    std::shared_ptr<UIPanel> createFeaturesPanel();
    std::shared_ptr<UIPanel> createBottomPanel();
};

} // namespace ArenaFighter