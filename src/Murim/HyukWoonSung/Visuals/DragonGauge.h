#pragma once
#include <memory>

namespace ArenaFighter {

class DragonGauge {
public:
    DragonGauge();
    ~DragonGauge();
    
    void Update(float deltaTime, float gaugeValue);
    void Render();
    
    void SetGaugeValue(float value);
    float GetGaugeValue() const;
    
    struct DragonConfig {
        float headX, headY;
        float bodySegments;
        float animationSpeed;
        float glowIntensity;
        bool isGlowing;
    };
    
private:
    float m_gaugeValue;
    
    DragonConfig m_blueDragon;
    float m_blueDragonAnimation;
    
    DragonConfig m_redDragon;
    float m_redDragonAnimation;
    
    float m_intertwineProgress;
    bool m_dragonsIntertwined;
    
    bool m_goldGlowActive;
    float m_goldGlowIntensity;
    float m_goldPulseTimer;
    
    void UpdateBlueDragon(float deltaTime);
    void UpdateRedDragon(float deltaTime);
    void UpdateIntertwining(float deltaTime);
    void UpdateGoldGlow(float deltaTime);
    
    void RenderBlueDragon();
    void RenderRedDragon();
    void RenderIntertwining();
    void RenderGoldGlow();
    
    float CalculateDragonCurve(float t, bool isBlue);
    void AnimateDragonMovement(DragonConfig& dragon, float deltaTime);
};

} // namespace ArenaFighter