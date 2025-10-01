#include "CombatHUD.h"
#include "CombatHUDConfig.h"
#include <sstream>

namespace ArenaFighter {

void CombatHUD::buildUI() {
    CombatHUDConfig config;
    
    m_rootPanel = std::make_shared<UIPanel>("CombatHUD", XMFLOAT2(0, 0), m_screenSize);
    m_rootPanel->setBackgroundColor(XMFLOAT4(0, 0, 0, 0));
    
    // Player stats panel (top left)
    auto statsPos = getAnchoredPosition(Anchor::TopLeft, config.m_layout.m_screenMargin, config.m_layout.m_screenMargin);
    auto statsSize = config.getScaledDimensions(config.m_layout.m_statsPanelSize, m_uiScale);
    auto statsPanel = std::make_shared<UIPanel>("PlayerStats", statsPos, statsSize);
    statsPanel->setBackgroundColor(config.m_visual.m_panelBackgroundColor);
    
    // Health bar
    float barPadding = getScaledValue(config.m_layout.m_elementPadding);
    XMFLOAT2 barSize(statsSize.x - 2 * barPadding, getScaledValue(30));
    m_healthBar = std::make_shared<UIProgressBar>("HealthBar", 
        XMFLOAT2(barPadding, barPadding), barSize, BASE_HEALTH);
    m_healthBar->setFillColor(config.m_visual.m_healthBarColor);
    m_healthBar->setValue(BASE_HEALTH);
    statsPanel->addChild(m_healthBar);
    
    // Mana bar
    m_manaBar = std::make_shared<UIProgressBar>("ManaBar", 
        XMFLOAT2(barPadding, barPadding + barSize.y + 5), 
        XMFLOAT2(barSize.x, getScaledValue(25)), BASE_MANA);
    m_manaBar->setFillColor(config.m_visual.m_manaBarColor);
    m_manaBar->setValue(BASE_MANA);
    statsPanel->addChild(m_manaBar);
    
    // Qi bar
    m_qiBar = std::make_shared<UIProgressBar>("QiBar", 
        XMFLOAT2(barPadding, barPadding + barSize.y + getScaledValue(25) + 10), 
        XMFLOAT2(barSize.x, getScaledValue(20)), 100.0f);
    m_qiBar->setFillColor(config.m_visual.m_qiBarColor);
    m_qiBar->setValue(0);
    statsPanel->addChild(m_qiBar);
    
    // Score label
    m_scoreLabel = std::make_shared<UILabel>("Score", 
        XMFLOAT2(barPadding, barPadding + barSize.y + getScaledValue(25) + getScaledValue(20) + 15), 
        "Kills: 0  Deaths: 0", 
        config.getScaledFontSize(config.m_visual.m_defaultFontSize, m_uiScale));
    statsPanel->addChild(m_scoreLabel);
    
    m_rootPanel->addChild(statsPanel);
    
    // Stance indicator (top center)
    auto stancePos = getAnchoredPosition(Anchor::TopCenter, -100, config.m_layout.m_screenMargin);
    auto stanceSize = config.getScaledDimensions(config.m_layout.m_stancePanelSize, m_uiScale);
    m_stanceIndicator = std::make_shared<UIPanel>("StancePanel", stancePos, stanceSize);
    m_stanceIndicator->setBackgroundColor(config.m_visual.m_lightStanceBgColor);
    
    auto stanceName = std::make_shared<UILabel>("StanceName", 
        XMFLOAT2(getScaledValue(10), getScaledValue(10)), 
        "Light - Orthodox Spear", 
        config.getScaledFontSize(config.m_visual.m_largeFontSize, m_uiScale));
    stanceName->setTextColor(config.m_visual.m_lightStanceColor);
    m_stanceIndicator->addChild(stanceName);
    
    m_rootPanel->addChild(m_stanceIndicator);
    
    // Skills panel (bottom center)
    auto skillsPos = getAnchoredPosition(Anchor::BottomCenter, -400, -config.m_layout.m_screenMargin - 160);
    auto skillsSize = config.getScaledDimensions(config.m_layout.m_skillsPanelSize, m_uiScale);
    m_skillsPanel = std::make_shared<UIPanel>("SkillsPanel", skillsPos, skillsSize);
    m_skillsPanel->setBackgroundColor(config.m_visual.m_panelBackgroundColor);
    
    // Create skill slots
    float skillX = getScaledValue(20);
    float skillSlotSize = getScaledValue(120);
    
    for (int i = 0; i < 4; i++) {
        auto skillSlot = std::make_shared<UIPanel>("Skill" + std::to_string(i), 
            XMFLOAT2(skillX, getScaledValue(20)), 
            XMFLOAT2(skillSlotSize, skillSlotSize));
        skillSlot->setBackgroundColor(XMFLOAT4(0.15f, 0.15f, 0.2f, 0.9f));
        
        // Skill name label
        auto nameLabel = std::make_shared<UILabel>("Skill" + std::to_string(i) + "Name", 
            XMFLOAT2(getScaledValue(10), getScaledValue(10)), 
            m_specialSkills[i].m_name, 
            config.getScaledFontSize(config.m_visual.m_smallFontSize, m_uiScale));
        nameLabel->setTextColor(XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f));
        skillSlot->addChild(nameLabel);
        
        // Mana cost label
        std::stringstream ss;
        ss << "Mana: " << static_cast<int>(m_specialSkills[i].m_manaCost);
        auto costLabel = std::make_shared<UILabel>("Skill" + std::to_string(i) + "Cost",
            XMFLOAT2(getScaledValue(10), skillSlotSize - getScaledValue(25)), 
            ss.str(), 
            config.getScaledFontSize(config.m_visual.m_smallFontSize, m_uiScale));
        costLabel->setTextColor(config.m_visual.m_manaBarColor);
        skillSlot->addChild(costLabel);
        
        m_skillsPanel->addChild(skillSlot);
        skillX += skillSlotSize + getScaledValue(config.m_layout.m_skillSlotSpacing);
    }
    
    // Gear skills (smaller slots)
    skillX += getScaledValue(40);
    float gearSlotSize = getScaledValue(80);
    
    std::vector<std::string> gearNames = {"Weapon", "Helmet", "Armor", "Trinket"};
    for (int i = 0; i < 4; i++) {
        auto gearSlot = std::make_shared<UIPanel>("Gear" + std::to_string(i), 
            XMFLOAT2(skillX, getScaledValue(40)), 
            XMFLOAT2(gearSlotSize, gearSlotSize));
        gearSlot->setBackgroundColor(XMFLOAT4(0.15f, 0.12f, 0.18f, 0.9f));
        
        // Gear label  
        auto gearLabel = std::make_shared<UILabel>("Gear" + std::to_string(i) + "Name",
            XMFLOAT2(getScaledValue(5), getScaledValue(5)), 
            gearNames[i], 
            config.getScaledFontSize(config.m_visual.m_smallFontSize, m_uiScale));
        gearLabel->setTextColor(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
        gearSlot->addChild(gearLabel);
        
        // Duration bar for active buffs
        auto durationBar = std::make_shared<UIProgressBar>("Gear" + std::to_string(i) + "Duration",
            XMFLOAT2(getScaledValue(5), gearSlotSize - getScaledValue(10)), 
            XMFLOAT2(gearSlotSize - getScaledValue(10), getScaledValue(5)), 
            1.0f);
        durationBar->setFillColor(config.m_visual.m_qiBarColor);
        durationBar->setVisible(false);
        gearSlot->addChild(durationBar);
        
        m_skillsPanel->addChild(gearSlot);
        skillX += gearSlotSize + getScaledValue(10);
    }
    
    m_rootPanel->addChild(m_skillsPanel);
    
    // Combo counter (right side)
    auto comboPos = getAnchoredPosition(Anchor::MiddleRight, -220, 0);
    auto comboSize = config.getScaledDimensions(config.m_layout.m_comboPanelSize, m_uiScale);
    auto comboPanel = std::make_shared<UIPanel>("ComboPanel", comboPos, comboSize);
    comboPanel->setBackgroundColor(XMFLOAT4(0.08f, 0.05f, 0.05f, 0.8f));
    comboPanel->setVisible(false);
    
    auto comboTitle = std::make_shared<UILabel>("ComboTitle", 
        XMFLOAT2(getScaledValue(10), getScaledValue(10)), 
        "COMBO", 
        config.getScaledFontSize(config.m_visual.m_defaultFontSize, m_uiScale));
    comboPanel->addChild(comboTitle);
    
    m_comboLabel = std::make_shared<UILabel>("ComboCount", 
        XMFLOAT2(getScaledValue(10), getScaledValue(40)), 
        "0", 
        config.getScaledFontSize(config.m_visual.m_comboFontSize, m_uiScale));
    m_comboLabel->setTextColor(XMFLOAT4(1.0f, 0.8f, 0.3f, 1.0f));
    comboPanel->addChild(m_comboLabel);
    
    m_rootPanel->addChild(comboPanel);
    
    // Timer (top center)
    auto timerPos = getAnchoredPosition(Anchor::TopCenter, -100, stanceSize.y + config.m_layout.m_screenMargin + 10);
    auto timerPanel = std::make_shared<UIPanel>("TimerPanel", 
        timerPos, 
        XMFLOAT2(getScaledValue(200), getScaledValue(60)));
    timerPanel->setBackgroundColor(config.m_visual.m_panelBackgroundColor);
    
    auto timerLabel = std::make_shared<UILabel>("Timer", 
        XMFLOAT2(getScaledValue(50), getScaledValue(20)), 
        "3:00", 
        config.getScaledFontSize(config.m_visual.m_largeFontSize, m_uiScale));
    timerLabel->setTextColor(XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f));
    timerPanel->addChild(timerLabel);
    
    m_rootPanel->addChild(timerPanel);
}

void CombatHUD::updateStanceDisplay() {
    if (!m_stanceIndicator) return;
    
    CombatHUDConfig config;
    auto stanceName = findChildRecursive<UILabel>(m_stanceIndicator, "StanceName");
    if (!stanceName) return;
    
    if (m_currentStance == PlayerStance::Light) {
        stanceName->setText("Light - Orthodox Spear");
        stanceName->setTextColor(config.m_visual.m_lightStanceColor);
        m_stanceIndicator->setBackgroundColor(config.m_visual.m_lightStanceBgColor);
    } else {
        stanceName->setText("Dark - Heavenly Demon");
        stanceName->setTextColor(config.m_visual.m_darkStanceColor);
        m_stanceIndicator->setBackgroundColor(config.m_visual.m_darkStanceBgColor);
    }
}

} // namespace ArenaFighter