#pragma once
#include "UIScreen.h"
#include "../Components/UIPanel.h"
#include "../Components/GameButton.h"
#include "../../Characters/CharacterBase.h"
#include <memory>
#include <vector>
#include <array>

namespace ArenaFighter {

struct CharacterCard {
    std::string name;
    std::string category;
    std::string description;
    std::string iconPath;
    CharacterCategory categoryEnum;
    bool unlocked;
    float animationOffset;
};

class CharacterSelectScreen : public UIScreen {
public:
    CharacterSelectScreen();
    ~CharacterSelectScreen() override;
    
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render() override;
    void OnEnter() override;
    void OnExit() override;
    
    // Character selection
    const std::string& GetSelectedCharacter() const { return m_selectedCharacter; }
    int GetSelectedSkin() const { return m_selectedSkin; }
    
private:
    // Layout panels
    std::unique_ptr<UIPanel> m_backgroundPanel;
    std::unique_ptr<UIPanel> m_categoryPanel;
    std::unique_ptr<UIPanel> m_characterGridPanel;
    std::unique_ptr<UIPanel> m_previewPanel;
    std::unique_ptr<UIPanel> m_infoPanel;
    
    // Category buttons
    std::array<std::unique_ptr<GameButton>, 7> m_categoryButtons;
    
    // Character selection
    std::vector<CharacterCard> m_characters;
    std::vector<std::unique_ptr<GameButton>> m_characterButtons;
    int m_selectedIndex;
    std::string m_selectedCharacter;
    CharacterCategory m_selectedCategory;
    int m_selectedSkin;
    
    // Action buttons
    std::unique_ptr<GameButton> m_confirmButton;
    std::unique_ptr<GameButton> m_backButton;
    std::unique_ptr<GameButton> m_randomButton;
    
    // Skin selection
    std::array<std::unique_ptr<GameButton>, 4> m_skinButtons;
    
    // Visual state
    float m_previewRotation;
    float m_categoryAnimation;
    float m_selectionAnimation;
    ImVec2 m_gridScroll;
    
    // Character preview
    void* m_previewModel;
    float m_modelScale;
    ImVec2 m_modelOffset;
    
    // Methods
    void LoadCharacters();
    void CreateLayout();
    void FilterByCategory(CharacterCategory category);
    void SelectCharacter(int index);
    void UpdatePreview(float deltaTime);
    
    // Rendering sections
    void RenderCategories();
    void RenderCharacterGrid();
    void RenderPreview();
    void RenderCharacterInfo();
    void RenderSkinSelection();
    void RenderActions();
    
    // Callbacks
    void OnCategorySelected(CharacterCategory category);
    void OnCharacterSelected(int index);
    void OnSkinSelected(int skinIndex);
    void OnConfirm();
    void OnBack();
    void OnRandom();
    
    // Animation helpers
    void AnimateCharacterCards(float deltaTime);
    void AnimatePreview(float deltaTime);
    float GetCardAnimation(int index) const;
};

} // namespace ArenaFighter