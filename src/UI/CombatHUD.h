#pragma once

#include "UIPanel.h"
#include "UIProgressBar.h"
#include "UILabel.h"
#include "CharacterData.h"
#include "../Combat/CombatEnums.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <memory>
#include <vector>
#include <queue>
#include <functional>

namespace ArenaFighter {

using namespace DirectX;
using Microsoft::WRL::ComPtr;

constexpr float MAX_COMBO_HITS = 15.0f;
constexpr float DAMAGE_NUMBER_DURATION = 2.0f;
constexpr float DAMAGE_NUMBER_RISE_SPEED = 100.0f;

enum class PlayerStance {
    Light,
    Dark
};

// Observer pattern for stats updates
class ICombatStatsObserver {
public:
    virtual ~ICombatStatsObserver() = default;
    virtual void onHealthChanged(float oldValue, float newValue) = 0;
    virtual void onManaChanged(float oldValue, float newValue) = 0;
    virtual void onQiChanged(float oldValue, float newValue) = 0;
    virtual void onComboChanged(int oldValue, int newValue) = 0;
};

// Damage number animation system
struct DamageNumber {
    XMFLOAT2 m_position;
    float m_value;
    float m_lifetime;
    XMFLOAT4 m_color;
    bool m_isCritical;
};

struct CombatStats {
    float m_health;
    float m_maxHealth;
    float m_mana;
    float m_maxMana;
    float m_qi;
    float m_maxQi;
    int m_comboCount;
    int m_killCount;
    int m_deathCount;
    
    CombatStats() : 
        m_health(BASE_HEALTH), m_maxHealth(BASE_HEALTH),
        m_mana(BASE_MANA), m_maxMana(BASE_MANA),
        m_qi(0), m_maxQi(100),
        m_comboCount(0), m_killCount(0), m_deathCount(0) {}
};

struct SpecialSkill {
    std::string m_name;
    float m_manaCost;
    
    bool isReady(float currentMana) const { return currentMana >= m_manaCost; }
};

struct GearSkill {
    std::string m_name;
    float m_manaCost;
    float m_duration;
    float m_currentDuration;
    bool m_isActive;
    
    bool isActive() const { return m_isActive && m_currentDuration > 0; }
    bool isReady(float currentMana) const { return !m_isActive && currentMana >= m_manaCost; }
};

class CombatHUD {
public:
    // Resolution-independent anchor points
    enum class Anchor {
        TopLeft, TopCenter, TopRight,
        MiddleLeft, Center, MiddleRight,
        BottomLeft, BottomCenter, BottomRight
    };

private:
    // DirectX resources
    ComPtr<ID3D11Device> m_device;
    ComPtr<ID3D11DeviceContext> m_context;
    
    std::shared_ptr<UIPanel> m_rootPanel;
    
    // Core UI elements
    std::shared_ptr<UIProgressBar> m_healthBar;
    std::shared_ptr<UIProgressBar> m_manaBar;
    std::shared_ptr<UIProgressBar> m_qiBar;
    std::shared_ptr<UILabel> m_comboLabel;
    std::shared_ptr<UILabel> m_scoreLabel;
    
    // Stance and skills
    std::shared_ptr<UIPanel> m_stanceIndicator;
    std::shared_ptr<UIPanel> m_skillsPanel;
    std::shared_ptr<UIPanel> m_gearSkillsPanel;
    
    // Combat state
    PlayerStance m_currentStance;
    CombatStats m_playerStats;
    std::vector<SpecialSkill> m_specialSkills;
    
    // Gear skills
    GearSkill m_weaponSkill;
    GearSkill m_helmetSkill;
    GearSkill m_armorSkill;
    GearSkill m_trinketSkill;
    
    bool m_isUltimateActive;
    
    // Resolution-aware positioning
    XMFLOAT2 m_screenSize;
    float m_uiScale;
    
    // Damage number system
    std::queue<DamageNumber> m_damageNumbers;
    
    // Observers
    std::vector<std::weak_ptr<ICombatStatsObserver>> m_observers;
    
    // Error handling
    std::function<void(const std::string&)> m_errorCallback;

public:
    explicit CombatHUD(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    virtual ~CombatHUD();
    
    void update(float deltaTime);
    void render();
    
    // Resolution handling
    void setScreenSize(float width, float height);
    void setUIScale(float scale);
    
    // Player actions
    void switchStance();
    void useSpecialSkill(int skillIndex);
    void useGearSkill(int gearIndex);
    void activateUltimate();
    
    // Combat events
    void addCombo();
    void resetCombo();
    void addKill();
    void addDeath();
    void showDamageNumber(XMFLOAT2 worldPos, float damage, bool isCritical = false);
    
    // Stats management
    void takeDamage(float damage);
    void restoreHealth(float amount);
    void restoreMana(float amount);
    void addQi(float amount);
    
    // Observer pattern
    void addObserver(std::weak_ptr<ICombatStatsObserver> observer);
    void removeObserver(std::weak_ptr<ICombatStatsObserver> observer);
    
    // Error handling
    void setErrorCallback(std::function<void(const std::string&)> callback);
    
    // Getters
    std::shared_ptr<UIPanel> getRootPanel() const { return m_rootPanel; }
    PlayerStance getCurrentStance() const { return m_currentStance; }
    bool isUltimateReady() const;
    bool isSpecialSkillReady(int skillIndex) const;
    const CombatStats& getStats() const { return m_playerStats; }

private:
    void buildUI();
    void updateStanceDisplay();
    void updateSkillDisplay();
    void updateGearSkillsDisplay();
    void updateStatsDisplay();
    void updateComboDisplay();
    void updateGearSkill(GearSkill& skill, float deltaTime);
    void updateGearSkillDisplay(int index, const GearSkill& skill);
    void updateDamageNumbers(float deltaTime);
    
    void onPlayerDeath();
    
    // Resolution-aware positioning
    XMFLOAT2 getAnchoredPosition(Anchor anchor, float offsetX = 0, float offsetY = 0) const;
    float getScaledValue(float value) const;
    
    // Observer notifications
    void notifyHealthChanged(float oldValue, float newValue);
    void notifyManaChanged(float oldValue, float newValue);
    void notifyQiChanged(float oldValue, float newValue);
    void notifyComboChanged(int oldValue, int newValue);
    
    // Error handling
    void reportError(const std::string& error);
    bool validateSkillIndex(int index) const;
    
    // Resource cleanup
    void releaseResources();
    
    template<typename T>
    std::shared_ptr<T> findChildRecursive(std::shared_ptr<UIPanel> parent, const std::string& id);
};

} // namespace ArenaFighter