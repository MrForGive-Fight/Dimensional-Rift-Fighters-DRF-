#include "CombatHUD.h"

namespace ArenaFighter {

// Continuation of CombatHUD implementation

void CombatHUD::updateSkillDisplay() {
    if (!m_skillsPanel) return;
    
    for (size_t i = 0; i < m_specialSkills.size(); i++) {
        std::string skillId = "Skill" + std::to_string(i);
        auto skillIcon = findChildRecursive<UIPanel>(m_skillsPanel, skillId);
        if (!skillIcon) continue;
        
        auto& skill = m_specialSkills[i];
        
        // Update skill availability based on mana
        if (m_playerStats.m_mana >= skill.m_manaCost) {
            skillIcon->setBackgroundColor(XMFLOAT4(0.25f, 0.3f, 0.4f, 0.9f));
        } else {
            skillIcon->setBackgroundColor(XMFLOAT4(0.2f, 0.15f, 0.15f, 0.9f));
        }
        
        auto manaCostLabel = findChildRecursive<UILabel>(skillIcon, skillId + "Cost");
        if (manaCostLabel) {
            if (m_playerStats.m_mana >= skill.m_manaCost) {
                manaCostLabel->setTextColor(XMFLOAT4(0.3f, 0.5f, 0.9f, 1.0f));
            } else {
                manaCostLabel->setTextColor(XMFLOAT4(0.9f, 0.3f, 0.3f, 1.0f));
            }
        }
    }
    
    updateGearSkillsDisplay();
}

void CombatHUD::updateGearSkillsDisplay() {
    if (!m_skillsPanel) return;
    
    std::vector<GearSkill*> gearSkills = {&m_weaponSkill, &m_helmetSkill, &m_armorSkill, &m_trinketSkill};
    
    for (size_t i = 0; i < gearSkills.size(); i++) {
        std::string gearId = "Gear" + std::to_string(i);
        auto gearIcon = findChildRecursive<UIPanel>(m_skillsPanel, gearId);
        if (!gearIcon) continue;
        
        auto& skill = *gearSkills[i];
        
        // Update gear availability based on mana and active state
        if (skill.isActive()) {
            gearIcon->setBackgroundColor(XMFLOAT4(0.4f, 0.35f, 0.5f, 0.9f));
        } else if (m_playerStats.m_mana >= skill.m_manaCost) {
            gearIcon->setBackgroundColor(XMFLOAT4(0.25f, 0.2f, 0.3f, 0.9f));
        } else {
            gearIcon->setBackgroundColor(XMFLOAT4(0.15f, 0.12f, 0.18f, 0.9f));
        }
        
        auto durationBar = findChildRecursive<UIProgressBar>(gearIcon, gearId + "Duration");
        if (durationBar) {
            if (skill.isActive() && skill.m_duration > 0) {
                durationBar->setMaxValue(skill.m_duration);
                durationBar->setValue(skill.m_currentDuration);
                durationBar->setVisible(true);
            } else {
                durationBar->setValue(0);
                durationBar->setVisible(false);
            }
        }
    }
}

void CombatHUD::updateStatsDisplay() {
    if (m_healthBar) {
        m_healthBar->setValue(m_playerStats.m_health);
        
        float healthPercent = m_playerStats.m_health / m_playerStats.m_maxHealth;
        if (healthPercent < 0.25f) {
            m_healthBar->setFillColor(XMFLOAT4(1.0f, 0.1f, 0.1f, 1.0f));
        } else if (healthPercent < 0.5f) {
            m_healthBar->setFillColor(XMFLOAT4(0.9f, 0.5f, 0.2f, 1.0f));
        } else {
            m_healthBar->setFillColor(XMFLOAT4(0.8f, 0.2f, 0.2f, 1.0f));
        }
    }
    
    if (m_manaBar) {
        m_manaBar->setValue(m_playerStats.m_mana);
    }
    
    if (m_qiBar) {
        m_qiBar->setValue(m_playerStats.m_qi);
        
        if (m_playerStats.m_qi >= m_playerStats.m_maxQi) {
            m_qiBar->setFillColor(XMFLOAT4(1.0f, 0.9f, 0.3f, 1.0f));
        } else {
            m_qiBar->setFillColor(XMFLOAT4(0.9f, 0.7f, 0.2f, 1.0f));
        }
    }
    
    auto playerStatsPanel = findChildRecursive<UIPanel>(m_rootPanel, "PlayerStats");
    if (playerStatsPanel) {
        auto ultReady = findChildRecursive<UILabel>(playerStatsPanel, "UltReady");
        if (!ultReady && isUltimateReady()) {
            ultReady = std::make_shared<UILabel>("UltReady", XMFLOAT2(10, 125), "ULTIMATE READY!", 20.0f);
            ultReady->setTextColor(XMFLOAT4(1.0f, 0.9f, 0.2f, 1.0f));
            playerStatsPanel->addChild(ultReady);
        } else if (ultReady) {
            ultReady->setVisible(isUltimateReady());
        }
    }
    
    if (m_scoreLabel) {
        m_scoreLabel->setText("Kills: " + std::to_string(m_playerStats.m_killCount) + 
                           "  Deaths: " + std::to_string(m_playerStats.m_deathCount));
    }
}

void CombatHUD::updateComboDisplay() {
    if (!m_comboLabel) return;
    
    m_comboLabel->setText(std::to_string(m_playerStats.m_comboCount));
    
    if (m_playerStats.m_comboCount >= 50) {
        m_comboLabel->setTextColor(XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f));
    } else if (m_playerStats.m_comboCount >= 20) {
        m_comboLabel->setTextColor(XMFLOAT4(1.0f, 0.6f, 0.2f, 1.0f));
    } else {
        m_comboLabel->setTextColor(XMFLOAT4(1.0f, 0.8f, 0.3f, 1.0f));
    }
    
    auto comboPanel = findChildRecursive<UIPanel>(m_rootPanel, "ComboPanel");
    if (comboPanel) {
        comboPanel->setVisible(m_playerStats.m_comboCount > 0);
    }
}

void CombatHUD::updateGearSkill(GearSkill& skill, float deltaTime) {
    if (skill.m_currentDuration > 0) {
        skill.m_currentDuration -= deltaTime;
        if (skill.m_currentDuration < 0) {
            skill.m_currentDuration = 0;
            skill.m_isActive = false;
        }
    }
}

void CombatHUD::onPlayerDeath() {
    m_playerStats.m_deathCount++;
    resetCombo();
    updateStatsDisplay();
    // Additional death effects would be triggered here
}

template<typename T>
std::shared_ptr<T> CombatHUD::findChildRecursive(std::shared_ptr<UIPanel> parent, const std::string& id) {
    if (!parent) return nullptr;
    
    auto child = parent->findChild(id);
    if (child) {
        return std::dynamic_pointer_cast<T>(child);
    }
    
    for (auto& c : parent->getChildren()) {
        if (auto panel = std::dynamic_pointer_cast<UIPanel>(c)) {
            auto found = findChildRecursive<T>(panel, id);
            if (found) return found;
        }
    }
    return nullptr;
}

// Explicit template instantiations
template std::shared_ptr<UILabel> CombatHUD::findChildRecursive<UILabel>(std::shared_ptr<UIPanel>, const std::string&);
template std::shared_ptr<UIPanel> CombatHUD::findChildRecursive<UIPanel>(std::shared_ptr<UIPanel>, const std::string&);
template std::shared_ptr<UIProgressBar> CombatHUD::findChildRecursive<UIProgressBar>(std::shared_ptr<UIPanel>, const std::string&);

} // namespace ArenaFighter