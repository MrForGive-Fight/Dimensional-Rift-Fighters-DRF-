#pragma once
#include "UIScreen.h"
#include "../Components/UIPanel.h"
#include "../Components/GameButton.h"
#include <memory>
#include <array>

namespace ArenaFighter {

class CharacterBase;

class InGameHUD : public UIScreen {
public:
    InGameHUD();
    ~InGameHUD() override;
    
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render() override;
    void OnEnter() override;
    void OnExit() override;
    
    // Player references
    void SetPlayer1(CharacterBase* player) { m_player1 = player; }
    void SetPlayer2(CharacterBase* player) { m_player2 = player; }
    
    // HUD notifications
    void ShowComboCounter(int player, int hits);
    void ShowDamageNumber(int player, float damage, ImVec2 worldPos);
    void ShowSystemMessage(const std::string& message, float duration = 3.0f);
    
private:
    // Player references
    CharacterBase* m_player1;
    CharacterBase* m_player2;
    
    // Health/Mana bars
    struct PlayerUI {
        std::unique_ptr<UIPanel> healthPanel;
        std::unique_ptr<UIPanel> manaPanel;
        std::unique_ptr<UIPanel> portraitPanel;
        std::unique_ptr<UIPanel> gearPanel;
        float healthAnimation;
        float manaAnimation;
        float healthDamagePreview;
        float manaDamagePreview;
    };
    
    PlayerUI m_player1UI;
    PlayerUI m_player2UI;
    
    // Timer and round info
    std::unique_ptr<UIPanel> m_timerPanel;
    std::unique_ptr<UIPanel> m_roundPanel;
    int m_roundTime;
    int m_currentRound;
    std::array<bool, 3> m_player1Rounds;
    std::array<bool, 3> m_player2Rounds;
    
    // Special gauges (character specific)
    std::unique_ptr<UIPanel> m_specialGauge1;
    std::unique_ptr<UIPanel> m_specialGauge2;
    
    // Combo and damage display
    struct ComboDisplay {
        int hitCount;
        float displayTimer;
        float scale;
        ImVec2 position;
    };
    ComboDisplay m_combo1;
    ComboDisplay m_combo2;
    
    struct DamageNumber {
        float damage;
        ImVec2 position;
        float timer;
        float velocity;
        ImU32 color;
    };
    std::vector<DamageNumber> m_damageNumbers;
    
    // System messages
    struct SystemMessage {
        std::string text;
        float timer;
        float fadeIn;
    };
    std::vector<SystemMessage> m_systemMessages;
    
    // Input display
    std::unique_ptr<UIPanel> m_inputDisplay1;
    std::unique_ptr<UIPanel> m_inputDisplay2;
    bool m_showInputDisplay;
    
    // Methods
    void CreateLayout();
    void UpdateHealthMana(float deltaTime);
    void UpdateCombos(float deltaTime);
    void UpdateDamageNumbers(float deltaTime);
    void UpdateSystemMessages(float deltaTime);
    
    // Rendering
    void RenderPlayerUI(const PlayerUI& ui, CharacterBase* player, bool rightSide);
    void RenderHealthBar(const PlayerUI& ui, float current, float max, bool rightSide);
    void RenderManaBar(const PlayerUI& ui, float current, float max, bool rightSide);
    void RenderPortrait(const PlayerUI& ui, CharacterBase* player, bool rightSide);
    void RenderGearIndicator(const PlayerUI& ui, CharacterBase* player, bool rightSide);
    void RenderTimer();
    void RenderRounds();
    void RenderCombos();
    void RenderDamageNumbers();
    void RenderSystemMessages();
    void RenderSpecialGauges();
    void RenderInputDisplay();
    
    // Character specific HUD elements
    void RenderRouGauge(CharacterBase* rou, bool rightSide);
    void RenderHyukStanceIndicator(CharacterBase* hyuk, bool rightSide);
    
    // Helper functions
    ImU32 GetHealthBarColor(float percent) const;
    ImU32 GetDamageColor(float damage) const;
    void AnimateValue(float& current, float target, float speed, float deltaTime);
};

} // namespace ArenaFighter