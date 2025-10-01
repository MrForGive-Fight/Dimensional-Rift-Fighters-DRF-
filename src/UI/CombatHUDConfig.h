#pragma once

#include <DirectXMath.h>

namespace ArenaFighter {

using namespace DirectX;

// Configuration structure for CombatHUD
struct CombatHUDConfig {
    // UI Layout Settings
    struct Layout {
        float m_defaultUIScale = 1.0f;
        float m_minUIScale = 0.5f;
        float m_maxUIScale = 2.0f;
        
        // Panel dimensions (base values, scaled by resolution)
        XMFLOAT2 m_statsPanelSize = XMFLOAT2(400, 150);
        XMFLOAT2 m_skillsPanelSize = XMFLOAT2(800, 160);
        XMFLOAT2 m_stancePanelSize = XMFLOAT2(200, 80);
        XMFLOAT2 m_comboPanelSize = XMFLOAT2(200, 100);
        
        // Margins and padding
        float m_screenMargin = 20.0f;
        float m_elementPadding = 10.0f;
        float m_skillSlotSpacing = 20.0f;
    } m_layout;
    
    // Visual Settings
    struct Visual {
        // Colors
        XMFLOAT4 m_panelBackgroundColor = XMFLOAT4(0.05f, 0.05f, 0.08f, 0.8f);
        XMFLOAT4 m_healthBarColor = XMFLOAT4(0.8f, 0.2f, 0.2f, 1.0f);
        XMFLOAT4 m_manaBarColor = XMFLOAT4(0.2f, 0.5f, 0.9f, 1.0f);
        XMFLOAT4 m_qiBarColor = XMFLOAT4(0.9f, 0.7f, 0.2f, 1.0f);
        
        // Light stance colors
        XMFLOAT4 m_lightStanceColor = XMFLOAT4(0.3f, 0.7f, 1.0f, 1.0f);
        XMFLOAT4 m_lightStanceBgColor = XMFLOAT4(0.05f, 0.08f, 0.12f, 0.8f);
        
        // Dark stance colors
        XMFLOAT4 m_darkStanceColor = XMFLOAT4(1.0f, 0.3f, 0.3f, 1.0f);
        XMFLOAT4 m_darkStanceBgColor = XMFLOAT4(0.12f, 0.05f, 0.05f, 0.8f);
        
        // Font sizes (base values, scaled by resolution)
        float m_defaultFontSize = 16.0f;
        float m_largeFontSize = 24.0f;
        float m_smallFontSize = 12.0f;
        float m_comboFontSize = 36.0f;
    } m_visual;
    
    // Animation Settings
    struct Animation {
        // Damage numbers
        float m_damageNumberDuration = 2.0f;
        float m_damageNumberRiseSpeed = 100.0f;
        float m_damageNumberFadeStart = 1.0f;
        float m_criticalDamageScale = 1.5f;
        
        // UI transitions
        float m_fadeInDuration = 0.3f;
        float m_fadeOutDuration = 0.2f;
        float m_skillHighlightDuration = 0.5f;
        
        // Combo display
        float m_comboScalePulse = 1.2f;
        float m_comboPulseDuration = 0.2f;
    } m_animation;
    
    // Damage Number Settings
    struct DamageNumbers {
        // Colors
        XMFLOAT4 m_normalDamageColor = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
        XMFLOAT4 m_criticalDamageColor = XMFLOAT4(1.0f, 0.3f, 0.0f, 1.0f);
        XMFLOAT4 m_healingColor = XMFLOAT4(0.0f, 1.0f, 0.3f, 1.0f);
        
        // Display settings
        int m_maxConcurrentNumbers = 10;
        float m_horizontalSpread = 50.0f;
        float m_verticalOffset = -20.0f;
    } m_damageNumbers;
    
    // Game Balance Settings (from CLAUDE.md)
    struct Balance {
        // Mana system
        float m_baseMana = 100.0f;
        float m_manaRegenPerSecond = 5.0f;
        
        // Health
        float m_baseHealth = 1000.0f;
        
        // Qi/Ultimate
        float m_maxQi = 100.0f;
        float m_qiPerHit = 2.0f;
        float m_qiPerDamageTaken = 1.0f;
        
        // Combo system
        int m_maxComboHits = 15;
        float m_comboWindowTime = 3.0f;
    } m_balance;
    
    // Performance Settings
    struct Performance {
        bool m_enableDamageNumbers = true;
        bool m_enableParticleEffects = true;
        bool m_enableUIAnimations = true;
        int m_maxUIUpdateRate = 60; // FPS for UI updates
    } m_performance;
    
    // Input Validation
    struct Validation {
        float m_minDamageValue = 0.0f;
        float m_maxDamageValue = 99999.0f;
        float m_minHealValue = 0.0f;
        float m_maxHealValue = 9999.0f;
    } m_validation;
    
    // Default constructor with sensible defaults
    CombatHUDConfig() = default;
    
    // Helper methods
    float getScaledFontSize(float baseSize, float uiScale) const {
        return baseSize * uiScale;
    }
    
    XMFLOAT2 getScaledDimensions(XMFLOAT2 baseDim, float uiScale) const {
        return XMFLOAT2(baseDim.x * uiScale, baseDim.y * uiScale);
    }
    
    bool isValidDamageValue(float damage) const {
        return damage >= m_validation.m_minDamageValue && 
               damage <= m_validation.m_maxDamageValue;
    }
    
    bool isValidHealValue(float heal) const {
        return heal >= m_validation.m_minHealValue && 
               heal <= m_validation.m_maxHealValue;
    }
};

} // namespace ArenaFighter