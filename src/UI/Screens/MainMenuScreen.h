#pragma once
#include "UIScreen.h"
#include "../Components/UIPanel.h"
#include "../Components/GameButton.h"
#include <memory>
#include <vector>

namespace ArenaFighter {

class MainMenuScreen : public UIScreen {
public:
    MainMenuScreen();
    ~MainMenuScreen() override;
    
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render() override;
    void OnEnter() override;
    void OnExit() override;
    
private:
    // Panels
    std::unique_ptr<UIPanel> m_backgroundPanel;
    std::unique_ptr<UIPanel> m_logoPanel;
    std::unique_ptr<UIPanel> m_menuPanel;
    std::unique_ptr<UIPanel> m_newsPanel;
    
    // Buttons
    std::unique_ptr<GameButton> m_playButton;
    std::unique_ptr<GameButton> m_characterButton;
    std::unique_ptr<GameButton> m_settingsButton;
    std::unique_ptr<GameButton> m_quitButton;
    
    // Quick match buttons
    std::unique_ptr<GameButton> m_rankedButton;
    std::unique_ptr<GameButton> m_casualButton;
    std::unique_ptr<GameButton> m_trainingButton;
    
    // Visual elements
    float m_logoAnimation;
    float m_backgroundScrollX;
    float m_backgroundScrollY;
    std::vector<ParticleEffect> m_particles;
    
    // State
    bool m_showQuickMatch;
    float m_quickMatchAnimation;
    
    // Methods
    void CreateLayout();
    void AnimateLogo(float deltaTime);
    void AnimateBackground(float deltaTime);
    void UpdateParticles(float deltaTime);
    void RenderBackground();
    void RenderLogo();
    void RenderMainMenu();
    void RenderQuickMatch();
    void RenderNews();
    
    // Button callbacks
    void OnPlayClicked();
    void OnCharacterClicked();
    void OnSettingsClicked();
    void OnQuitClicked();
    void OnRankedClicked();
    void OnCasualClicked();
    void OnTrainingClicked();
};

} // namespace ArenaFighter