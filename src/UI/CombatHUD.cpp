#include "CombatHUD.h"
#include "CombatHUDConfig.h"
#include <cmath>
#include <algorithm>
#include <sstream>

namespace ArenaFighter {

CombatHUD::CombatHUD(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context) 
    : m_device(device),
      m_context(context),
      m_currentStance(PlayerStance::Light),
      m_isUltimateActive(false),
      m_screenSize(1920.0f, 1080.0f),
      m_uiScale(1.0f) {
    
    if (!m_device || !m_context) {
        reportError("CombatHUD: Invalid DirectX device or context");
        return;
    }
    
    // Initialize special skills (mana-based, no cooldowns)
    m_specialSkills.push_back({"Rising Dragon", 25.0f});
    m_specialSkills.push_back({"Whirlwind Slash", 30.0f});
    m_specialSkills.push_back({"Phoenix Dive", 35.0f});
    m_specialSkills.push_back({"Thunder Strike", 40.0f});
    
    // Initialize gear skills (mana-based, no cooldowns)
    m_weaponSkill = {"Blade Dance", 20.0f, 10.0f, 0.0f, false};
    m_helmetSkill = {"Iron Will", 30.0f, 15.0f, 0.0f, false};
    m_armorSkill = {"Stone Skin", 45.0f, 20.0f, 0.0f, false};
    m_trinketSkill = {"Spirit Shield", 60.0f, 10.0f, 0.0f, false};
    
    buildUI();
}

CombatHUD::~CombatHUD() {
    releaseResources();
}

void CombatHUD::update(float deltaTime) {
    // Input validation
    deltaTime = std::clamp(deltaTime, 0.0f, 0.1f); // Cap at 100ms to prevent issues
    
    // Update gear skill durations
    updateGearSkill(m_weaponSkill, deltaTime);
    updateGearSkill(m_helmetSkill, deltaTime);
    updateGearSkill(m_armorSkill, deltaTime);
    updateGearSkill(m_trinketSkill, deltaTime);
    
    // Natural mana regeneration
    float oldMana = m_playerStats.m_mana;
    if (m_playerStats.m_mana < m_playerStats.m_maxMana) {
        m_playerStats.m_mana += MANA_REGEN * deltaTime;
        if (m_playerStats.m_mana > m_playerStats.m_maxMana) {
            m_playerStats.m_mana = m_playerStats.m_maxMana;
        }
        if (oldMana != m_playerStats.m_mana) {
            notifyManaChanged(oldMana, m_playerStats.m_mana);
        }
    }
    
    // Update damage numbers
    updateDamageNumbers(deltaTime);
    
    // Update displays
    updateStanceDisplay();
    updateSkillDisplay();
    updateGearSkillsDisplay();
    updateStatsDisplay();
}

void CombatHUD::render() {
    if (m_rootPanel) {
        // Render is handled by UIRenderer typically
        // This is a placeholder for any HUD-specific rendering
    }
}

void CombatHUD::setScreenSize(float width, float height) {
    m_screenSize = XMFLOAT2(width, height);
    // Rebuild UI with new dimensions
    buildUI();
}

void CombatHUD::setUIScale(float scale) {
    CombatHUDConfig config;
    m_uiScale = std::clamp(scale, config.m_layout.m_minUIScale, config.m_layout.m_maxUIScale);
    // Rebuild UI with new scale
    buildUI();
}

void CombatHUD::switchStance() {
    m_currentStance = (m_currentStance == PlayerStance::Light) ? 
                      PlayerStance::Dark : PlayerStance::Light;
    updateStanceDisplay();
}

void CombatHUD::useSpecialSkill(int skillIndex) {
    if (!validateSkillIndex(skillIndex)) {
        reportError("Invalid skill index: " + std::to_string(skillIndex));
        return;
    }
    
    auto& skill = m_specialSkills[skillIndex];
    if (skill.isReady(m_playerStats.m_mana)) {
        float oldMana = m_playerStats.m_mana;
        m_playerStats.m_mana -= skill.m_manaCost;
        notifyManaChanged(oldMana, m_playerStats.m_mana);
        updateSkillDisplay();
        updateStatsDisplay();
    }
}

void CombatHUD::useGearSkill(int gearIndex) {
    if (gearIndex < 0 || gearIndex > 3) {
        reportError("Invalid gear index: " + std::to_string(gearIndex));
        return;
    }
    
    GearSkill* skill = nullptr;
    switch (gearIndex) {
        case 0: skill = &m_weaponSkill; break;
        case 1: skill = &m_helmetSkill; break;
        case 2: skill = &m_armorSkill; break;
        case 3: skill = &m_trinketSkill; break;
    }
    
    if (skill && skill->isReady(m_playerStats.m_mana)) {
        float oldMana = m_playerStats.m_mana;
        m_playerStats.m_mana -= skill->m_manaCost;
        skill->m_isActive = true;
        skill->m_currentDuration = skill->m_duration;
        notifyManaChanged(oldMana, m_playerStats.m_mana);
        updateGearSkillsDisplay();
        updateStatsDisplay();
    }
}

void CombatHUD::activateUltimate() {
    if (isUltimateReady()) {
        m_isUltimateActive = true;
        float oldQi = m_playerStats.m_qi;
        m_playerStats.m_qi = 0;
        notifyQiChanged(oldQi, m_playerStats.m_qi);
        updateStatsDisplay();
    }
}

void CombatHUD::addCombo() {
    int oldCombo = m_playerStats.m_comboCount;
    m_playerStats.m_comboCount++;
    if (m_playerStats.m_comboCount > MAX_COMBO_HITS) {
        m_playerStats.m_comboCount = MAX_COMBO_HITS;
    }
    notifyComboChanged(oldCombo, m_playerStats.m_comboCount);
    updateComboDisplay();
}

void CombatHUD::resetCombo() {
    int oldCombo = m_playerStats.m_comboCount;
    m_playerStats.m_comboCount = 0;
    notifyComboChanged(oldCombo, m_playerStats.m_comboCount);
    updateComboDisplay();
}

void CombatHUD::addKill() {
    m_playerStats.m_killCount++;
    updateStatsDisplay();
}

void CombatHUD::addDeath() {
    m_playerStats.m_deathCount++;
    resetCombo();
    updateStatsDisplay();
}

void CombatHUD::showDamageNumber(XMFLOAT2 worldPos, float damage, bool isCritical) {
    CombatHUDConfig config;
    
    if (!config.m_validation.isValidDamageValue(damage)) {
        reportError("Invalid damage value: " + std::to_string(damage));
        return;
    }
    
    if (config.m_performance.m_enableDamageNumbers) {
        DamageNumber num;
        num.m_position = worldPos;
        num.m_value = damage;
        num.m_lifetime = 0.0f;
        num.m_isCritical = isCritical;
        num.m_color = isCritical ? config.m_damageNumbers.m_criticalDamageColor 
                                 : config.m_damageNumbers.m_normalDamageColor;
        
        m_damageNumbers.push(num);
        
        // Limit queue size
        while (m_damageNumbers.size() > static_cast<size_t>(config.m_damageNumbers.m_maxConcurrentNumbers)) {
            m_damageNumbers.pop();
        }
    }
}

void CombatHUD::takeDamage(float damage) {
    CombatHUDConfig config;
    damage = std::clamp(damage, 0.0f, config.m_validation.m_maxDamageValue);
    
    float oldHealth = m_playerStats.m_health;
    m_playerStats.m_health = std::max(0.0f, m_playerStats.m_health - damage);
    notifyHealthChanged(oldHealth, m_playerStats.m_health);
    updateStatsDisplay();
    
    // Add Qi on damage taken
    float oldQi = m_playerStats.m_qi;
    m_playerStats.m_qi = std::min(m_playerStats.m_qi + config.m_balance.m_qiPerDamageTaken * damage, 
                                  m_playerStats.m_maxQi);
    notifyQiChanged(oldQi, m_playerStats.m_qi);
    
    if (m_playerStats.m_health <= 0) {
        onPlayerDeath();
    }
}

void CombatHUD::restoreHealth(float amount) {
    CombatHUDConfig config;
    
    if (!config.m_validation.isValidHealValue(amount)) {
        reportError("Invalid heal value: " + std::to_string(amount));
        return;
    }
    
    float oldHealth = m_playerStats.m_health;
    m_playerStats.m_health = std::min(m_playerStats.m_health + amount, m_playerStats.m_maxHealth);
    notifyHealthChanged(oldHealth, m_playerStats.m_health);
    updateStatsDisplay();
}

void CombatHUD::restoreMana(float amount) {
    amount = std::max(0.0f, amount);
    float oldMana = m_playerStats.m_mana;
    m_playerStats.m_mana = std::min(m_playerStats.m_mana + amount, m_playerStats.m_maxMana);
    notifyManaChanged(oldMana, m_playerStats.m_mana);
    updateStatsDisplay();
}

void CombatHUD::addQi(float amount) {
    amount = std::max(0.0f, amount);
    float oldQi = m_playerStats.m_qi;
    m_playerStats.m_qi = std::min(m_playerStats.m_qi + amount, m_playerStats.m_maxQi);
    notifyQiChanged(oldQi, m_playerStats.m_qi);
    updateStatsDisplay();
}

void CombatHUD::addObserver(std::weak_ptr<ICombatStatsObserver> observer) {
    m_observers.push_back(observer);
}

void CombatHUD::removeObserver(std::weak_ptr<ICombatStatsObserver> observer) {
    m_observers.erase(
        std::remove_if(m_observers.begin(), m_observers.end(),
            [&observer](const std::weak_ptr<ICombatStatsObserver>& o) {
                return o.expired() || o.lock() == observer.lock();
            }),
        m_observers.end()
    );
}

void CombatHUD::setErrorCallback(std::function<void(const std::string&)> callback) {
    m_errorCallback = callback;
}

bool CombatHUD::isUltimateReady() const {
    return m_playerStats.m_qi >= m_playerStats.m_maxQi && !m_isUltimateActive;
}

bool CombatHUD::isSpecialSkillReady(int skillIndex) const {
    if (!validateSkillIndex(skillIndex)) return false;
    return m_specialSkills[skillIndex].isReady(m_playerStats.m_mana);
}

// buildUI() implementation moved to CombatHUDBuilder.cpp
// updateStanceDisplay() implementation moved to CombatHUDBuilder.cpp

void CombatHUD::updateSkillDisplay() {
    if (!m_skillsPanel) return;
    
    // Update special skill displays
    for (int i = 0; i < 4; i++) {
        auto skillSlot = m_skillsPanel->findChild("Skill" + std::to_string(i));
        if (!skillSlot) continue;
        
        bool canUse = m_specialSkills[i].isReady(m_playerStats.m_mana);
        float alpha = canUse ? 1.0f : 0.5f;
        
        // Update visual state based on availability
        skillSlot->setBackgroundColor(XMFLOAT4(0.15f * alpha, 0.15f * alpha, 0.2f * alpha, 0.9f));
    }
}

void CombatHUD::updateGearSkillsDisplay() {
    // Update gear skill duration bars
    updateGearSkillDisplay(0, m_weaponSkill);
    updateGearSkillDisplay(1, m_helmetSkill);
    updateGearSkillDisplay(2, m_armorSkill);
    updateGearSkillDisplay(3, m_trinketSkill);
}

void CombatHUD::updateGearSkillDisplay(int index, const GearSkill& skill) {
    auto gearSlot = m_skillsPanel ? m_skillsPanel->findChild("Gear" + std::to_string(index)) : nullptr;
    if (!gearSlot) return;
    
    auto durationBar = std::dynamic_pointer_cast<UIProgressBar>(
        gearSlot->findChild("Gear" + std::to_string(index) + "Duration"));
    
    if (durationBar) {
        durationBar->setVisible(skill.isActive());
        if (skill.isActive()) {
            durationBar->setValue(skill.m_currentDuration / skill.m_duration);
        }
    }
}

void CombatHUD::updateDamageNumbers(float deltaTime) {
    CombatHUDConfig config;
    
    // Process queue into vector for animation
    std::vector<DamageNumber> activeNumbers;
    while (!m_damageNumbers.empty()) {
        activeNumbers.push_back(m_damageNumbers.front());
        m_damageNumbers.pop();
    }
    
    // Animate and remove expired numbers
    for (auto& num : activeNumbers) {
        num.m_lifetime += deltaTime;
        
        // Vertical movement
        num.m_position.y -= config.m_animation.m_damageNumberRiseSpeed * deltaTime;
        
        // Fade out
        if (num.m_lifetime > config.m_animation.m_damageNumberFadeStart) {
            float fadeProgress = (num.m_lifetime - config.m_animation.m_damageNumberFadeStart) /
                               (config.m_animation.m_damageNumberDuration - config.m_animation.m_damageNumberFadeStart);
            num.m_color.w = 1.0f - fadeProgress;
        }
        
        // Keep alive numbers
        if (num.m_lifetime < config.m_animation.m_damageNumberDuration) {
            m_damageNumbers.push(num);
        }
    }
}

// Helper methods implementation moved to CombatHUDHelpers.cpp

void CombatHUD::updateGearSkill(GearSkill& skill, float deltaTime) {
    if (skill.m_isActive && skill.m_currentDuration > 0) {
        skill.m_currentDuration -= deltaTime;
        if (skill.m_currentDuration <= 0) {
            skill.m_isActive = false;
            skill.m_currentDuration = 0;
        }
    }
}

void CombatHUD::updateStatsDisplay() {
    // Update health bar
    if (m_healthBar) {
        m_healthBar->setValue(m_playerStats.m_health);
    }
    
    // Update mana bar
    if (m_manaBar) {
        m_manaBar->setValue(m_playerStats.m_mana);
    }
    
    // Update qi bar
    if (m_qiBar) {
        m_qiBar->setValue(m_playerStats.m_qi);
    }
    
    // Update score label
    if (m_scoreLabel) {
        std::stringstream ss;
        ss << "Kills: " << m_playerStats.m_killCount << "  Deaths: " << m_playerStats.m_deathCount;
        m_scoreLabel->setText(ss.str());
    }
}

void CombatHUD::updateComboDisplay() {
    auto comboPanel = m_rootPanel ? m_rootPanel->findChild("ComboPanel") : nullptr;
    if (!comboPanel) return;
    
    comboPanel->setVisible(m_playerStats.m_comboCount > 0);
    
    if (m_comboLabel) {
        m_comboLabel->setText(std::to_string(m_playerStats.m_comboCount));
    }
}

void CombatHUD::onPlayerDeath() {
    resetCombo();
    addDeath();
    // Additional death handling can be added here
}

// Template implementation moved to CombatHUDHelpers.cpp

} // namespace ArenaFighter