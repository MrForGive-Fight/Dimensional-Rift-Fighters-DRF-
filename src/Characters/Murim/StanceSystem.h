#pragma once

#include <string>
#include <functional>

namespace ArenaFighter {

enum class StanceType {
    Light,  // Orthodox Spear - Blue theme
    Dark    // Heavenly Demon - Red theme
};

/**
 * @brief Stance system for characters with dual combat styles
 * 
 * Manages stance switching, visual effects, and stance-specific modifiers
 * Used by Hyuk Woon Sung for Light/Dark stance mechanics
 */
class StanceSystem {
public:
    StanceSystem();
    ~StanceSystem() = default;
    
    // Public constants for gauge management
    static constexpr float GAUGE_PER_HIT = 2.0f;
    static constexpr float GAUGE_PER_COMBO = 5.0f;
    
    // Core functionality
    StanceType GetCurrentStance() const { return m_currentStance; }
    void SwitchStance();
    
    // Visual effects
    std::string GetStanceColor() const;
    std::string GetStanceAura() const;
    std::string GetSwitchEffect() const { return "YinYangShatter"; }
    
    // Stance-specific modifiers
    float GetDamageModifier() const;
    float GetSpeedModifier() const;
    float GetDefenseModifier() const;
    
    // Tempered True Blossom Gauge
    float GetGaugeLevel() const { return m_temperedGauge; }
    void AddGauge(float amount);
    void ConsumeGauge(float amount);
    bool CanUseUltimate() const { return m_temperedGauge >= 100.0f; }
    
    // Update system
    void Update(float deltaTime);
    
    // Callbacks for stance changes
    void SetOnStanceChange(std::function<void(StanceType, StanceType)> callback) {
        m_onStanceChange = callback;
    }
    
private:
    StanceType m_currentStance = StanceType::Light;
    float m_temperedGauge = 0.0f;  // 0-100
    float m_switchCooldown = 0.0f; // Brief cooldown after switching
    
    // Gauge gain rates
    static constexpr float GAUGE_PER_HIT = 2.0f;
    static constexpr float GAUGE_PER_COMBO = 5.0f;
    static constexpr float GAUGE_DECAY_RATE = 1.0f; // Per second
    static constexpr float MAX_GAUGE = 100.0f;
    
    // Stance switch cooldown
    static constexpr float SWITCH_COOLDOWN = 0.5f;
    
    std::function<void(StanceType, StanceType)> m_onStanceChange;
};

} // namespace ArenaFighter