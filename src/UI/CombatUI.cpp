#include "CombatUI.h"

namespace ArenaFighter {

constexpr float MANA_REGEN = 5.0f;

CombatUI::CombatUI(const std::string& id, XMFLOAT2 pos)
    : UIPanel(id, pos, XMFLOAT2(300, 100)),
      m_currentCombo(0), m_manaRegenTimer(0.0f) {
    
    // Create health bar
    m_healthBar = std::make_shared<UIProgressBar>("healthBar", 
        XMFLOAT2(10, 10), XMFLOAT2(280, 20), BASE_HEALTH);
    m_healthBar->setFillColor(XMFLOAT4(0.8f, 0.2f, 0.2f, 1.0f));
    
    // Create mana bar
    m_manaBar = std::make_shared<UIProgressBar>("manaBar", 
        XMFLOAT2(10, 35), XMFLOAT2(280, 20), BASE_MANA);
    m_manaBar->setFillColor(XMFLOAT4(0.2f, 0.5f, 0.9f, 1.0f));
    
    // Create labels
    m_healthLabel = std::make_shared<UILabel>("healthLabel", 
        XMFLOAT2(10, 10), "HP: 1000/1000");
    m_manaLabel = std::make_shared<UILabel>("manaLabel", 
        XMFLOAT2(10, 35), "MP: 100/100");
    m_comboCounter = std::make_shared<UILabel>("comboCounter", 
        XMFLOAT2(10, 60), "Combo: 0");
    
    // Add children
    addChild(m_healthBar);
    addChild(m_manaBar);
    addChild(m_healthLabel);
    addChild(m_manaLabel);
    addChild(m_comboCounter);
}

void CombatUI::update(float deltaTime) {
    UIPanel::update(deltaTime);
    
    // Handle mana regeneration
    m_manaRegenTimer += deltaTime;
    if (m_manaRegenTimer >= 1.0f) {
        float currentMana = m_manaBar->getValue();
        if (currentMana < BASE_MANA) {
            setMana(currentMana + MANA_REGEN);
        }
        m_manaRegenTimer = 0.0f;
    }
}

void CombatUI::setHealth(float current, float max) {
    m_healthBar->setMaxValue(max);
    m_healthBar->setValue(current);
    
    char buffer[32];
    sprintf_s(buffer, "HP: %.0f/%.0f", current, max);
    m_healthLabel->setText(buffer);
}

void CombatUI::setMana(float current, float max) {
    m_manaBar->setMaxValue(max);
    m_manaBar->setValue(current);
    
    char buffer[32];
    sprintf_s(buffer, "MP: %.0f/%.0f", current, max);
    m_manaLabel->setText(buffer);
}

void CombatUI::setComboCount(int combo) {
    m_currentCombo = combo;
    char buffer[32];
    sprintf_s(buffer, "Combo: %d", combo);
    m_comboCounter->setText(buffer);
}

void CombatUI::resetCombo() {
    setComboCount(0);
}

float CombatUI::getManaValue() const {
    return m_manaBar->getValue();
}

float CombatUI::getHealthValue() const {
    return m_healthBar->getValue();
}

} // namespace ArenaFighter