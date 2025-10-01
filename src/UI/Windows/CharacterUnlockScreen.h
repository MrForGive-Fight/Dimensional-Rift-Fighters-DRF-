#pragma once
#include "../Components/UIPanel.h"
#include "../Components/GameButton.h"
#include "../Components/GridLayout.h"
#include "../Core/UITheme.h"
#include <memory>
#include <vector>
#include <functional>

namespace ArenaFighter {

// Forward declarations
class Character3DPreview;
class ParticleSystem;
class UnlockAnimation;

class CharacterUnlockScreen : public UIPanel {
public:
    enum UnlockState {
        INITIAL_GLOW,
        REVEAL_CHARACTER,
        DISPLAY_INFO,
        CUSTOMIZATION,
        REWARD_DISPLAY,
        COMPLETE
    };
    
    enum UnlockType {
        PERMANENT_HERO,
        TEMPORARY_HERO,
        SPECIAL_UNLOCK,
        QUEST_REWARD
    };
    
    CharacterUnlockScreen();
    ~CharacterUnlockScreen();
    
    // Initialize with character data
    void SetUnlockData(const std::string& characterName, UnlockType type);
    void SetRewards(int exp, int gold, const std::vector<std::string>& items);
    
    // Start the unlock sequence
    void StartUnlockSequence();
    void SkipToEnd();
    
    // Callbacks
    void SetOnComplete(std::function<void()> callback) { m_onComplete = callback; }
    
    // Core methods
    void Update(float deltaTime) override;
    void Render() override;
    
private:
    // State management
    UnlockState m_currentState;
    float m_stateTimer;
    float m_transitionDuration;
    
    // Character data
    std::string m_characterName;
    std::string m_characterTitle;
    UnlockType m_unlockType;
    
    // Visual components
    std::unique_ptr<Character3DPreview> m_characterPreview;
    std::unique_ptr<ParticleSystem> m_particleEffects;
    std::unique_ptr<UnlockAnimation> m_unlockAnimation;
    
    // UI Elements
    std::unique_ptr<UIPanel> m_infoPanel;
    std::unique_ptr<UIPanel> m_customizationPanel;
    std::unique_ptr<UIPanel> m_rewardPanel;
    std::unique_ptr<GameButton> m_skipButton;
    std::unique_ptr<GameButton> m_nextButton;
    
    // Customization options
    std::unique_ptr<GameButton> m_helmToggle;
    std::unique_ptr<GameButton> m_armorToggle;
    bool m_showHelm;
    bool m_showArmor;
    
    // Rewards
    struct RewardData {
        int experience;
        int gold;
        std::vector<std::string> items;
    } m_rewards;
    
    // Callbacks
    std::function<void()> m_onComplete;
    
    // State handlers
    void UpdateInitialGlow(float deltaTime);
    void UpdateRevealCharacter(float deltaTime);
    void UpdateDisplayInfo(float deltaTime);
    void UpdateCustomization(float deltaTime);
    void UpdateRewardDisplay(float deltaTime);
    
    // Rendering methods
    void RenderGlowEffect();
    void RenderCharacterReveal();
    void RenderInfoPanel();
    void RenderCustomizationPanel();
    void RenderRewardPanel();
    void RenderParticles();
    
    // Transition to next state
    void TransitionToState(UnlockState newState);
};

// Character unlock animation controller
class UnlockAnimation {
public:
    UnlockAnimation();
    
    void StartGlowAnimation();
    void StartRevealAnimation();
    void StartCelebrationAnimation();
    
    void Update(float deltaTime);
    void Render(ImVec2 centerPos);
    
    float GetProgress() const { return m_progress; }
    bool IsComplete() const { return m_isComplete; }
    
private:
    enum AnimationType {
        GLOW_BURST,
        CHARACTER_REVEAL,
        CELEBRATION
    };
    
    AnimationType m_currentAnimation;
    float m_progress;
    float m_duration;
    bool m_isComplete;
    
    // Glow effect parameters
    float m_glowRadius;
    float m_glowIntensity;
    ImU32 m_glowColor;
    std::vector<float> m_lightRays;
    
    // Reveal parameters
    float m_revealHeight;
    float m_fadeInAlpha;
    
    void RenderGlowBurst(ImVec2 center);
    void RenderLightRays(ImVec2 center);
    void RenderRevealEffect(ImVec2 center);
};

// 3D character preview window
class Character3DPreview {
public:
    Character3DPreview();
    ~Character3DPreview();
    
    void LoadCharacter(const std::string& characterName);
    void SetPose(const std::string& poseName);
    void SetRotation(float angle);
    void SetScale(float scale);
    
    void ShowHelmet(bool show);
    void ShowArmor(bool show);
    
    void Update(float deltaTime);
    void Render(ImVec2 position, ImVec2 size);
    
    // Preview actions
    void PlayAnimation(const std::string& animationName);
    void StopAnimation();
    
private:
    void* m_characterModel;
    void* m_renderTexture;
    
    float m_rotation;
    float m_scale;
    float m_animationTime;
    std::string m_currentAnimation;
    
    bool m_showHelmet;
    bool m_showArmor;
    
    // Platform effect
    void RenderPlatform(ImVec2 position, ImVec2 size);
    void RenderEnergyRings(ImVec2 center, float radius);
};

// Customization panel for character
class CharacterCustomizationPanel : public UIPanel {
public:
    CharacterCustomizationPanel();
    
    void SetCharacter(Character3DPreview* preview);
    void Initialize();
    
    void Render() override;
    
private:
    Character3DPreview* m_characterPreview;
    
    // Preview options
    struct PreviewOption {
        std::string name;
        std::string animationName;
        std::unique_ptr<GameButton> button;
    };
    std::vector<PreviewOption> m_previewOptions;
    
    // Decoration items
    struct DecorationCategory {
        std::string name;
        std::vector<std::string> items;
    };
    std::vector<DecorationCategory> m_decorationCategories;
    int m_selectedCategory;
    
    std::unique_ptr<GridLayout> m_decorationGrid;
    std::unique_ptr<GameButton> m_randomDecoButton;
    
    void CreatePreviewOptions();
    void CreateDecorationGrid();
    void OnPreviewSelected(const std::string& animationName);
    void OnDecorationSelected(int index);
};

// Reward display panel
class RewardDisplayPanel : public UIPanel {
public:
    RewardDisplayPanel();
    
    void SetHeroReward(const std::string& heroName, bool isPermanent);
    void SetExperienceReward(int exp);
    void SetGoldReward(int gold);
    void AddItemReward(const std::string& itemName, void* icon);
    
    void StartRevealAnimation();
    void Render() override;
    
private:
    struct Reward {
        enum Type { HERO, EXPERIENCE, GOLD, ITEM };
        Type type;
        std::string name;
        int value;
        void* icon;
        float revealDelay;
        float revealProgress;
    };
    
    std::vector<Reward> m_rewards;
    float m_animationTimer;
    bool m_isAnimating;
    
    // Shield frame decoration
    void RenderShieldFrame();
    void RenderReward(const Reward& reward, ImVec2 position);
    void RenderBattleAxes(ImVec2 position, ImVec2 size);
};

// Particle effects for unlock screen
class UnlockParticleSystem {
public:
    UnlockParticleSystem();
    
    void EmitSparkles(ImVec2 position, int count);
    void EmitLightOrbs(ImVec2 position, int count);
    void EmitCelebration(ImVec2 position);
    
    void Update(float deltaTime);
    void Render();
    
private:
    struct Particle {
        ImVec2 position;
        ImVec2 velocity;
        float lifetime;
        float maxLifetime;
        float size;
        ImU32 color;
        int type; // 0=sparkle, 1=orb, 2=star
    };
    
    std::vector<Particle> m_particles;
    
    void UpdateParticle(Particle& particle, float deltaTime);
    void RenderSparkle(const Particle& particle);
    void RenderOrb(const Particle& particle);
    void RenderStar(const Particle& particle);
};

} // namespace ArenaFighter