#pragma once

#include "UIPanel.h"
#include "UIProgressBar.h"
#include "UILabel.h"

namespace ArenaFighter {

constexpr float BASE_HEALTH = 1000.0f;
constexpr float BASE_MANA = 100.0f;

class CombatUI : public UIPanel {
private:
    std::shared_ptr<UIProgressBar> m_healthBar;
    std::shared_ptr<UIProgressBar> m_manaBar;
    std::shared_ptr<UILabel> m_healthLabel;
    std::shared_ptr<UILabel> m_manaLabel;
    std::shared_ptr<UILabel> m_comboCounter;
    
    int m_currentCombo;
    float m_manaRegenTimer;

public:
    CombatUI(const std::string& id, XMFLOAT2 pos);
    
    void update(float deltaTime) override;
    void setHealth(float current, float max = BASE_HEALTH);
    void setMana(float current, float max = BASE_MANA);
    void setComboCount(int combo);
    void resetCombo();
    
    float getManaValue() const;
    float getHealthValue() const;
};

} // namespace ArenaFighter