#include "DragonGauge.h"
#include <cmath>
#include <algorithm>

namespace ArenaFighter {

// Dragon Gauge class definition (header would be in StanceVFX.h or separate file)
class DragonGauge {
public:
    DragonGauge();
    ~DragonGauge();
    
    void Update(float deltaTime, float gaugeValue);
    void Render();
    
    // Dragon state
    void SetGaugeValue(float value) { m_gaugeValue = std::clamp(value, 0.0f, 100.0f); }
    float GetGaugeValue() const { return m_gaugeValue; }
    
    // Visual configuration
    struct DragonConfig {
        float headX, headY;
        float bodySegments;
        float animationSpeed;
        float glowIntensity;
        bool isGlowing;
    };
    
private:
    float m_gaugeValue;
    
    // Blue Dragon (Left side)
    DragonConfig m_blueDragon;
    float m_blueDragonAnimation;
    
    // Red Dragon (Right side)
    DragonConfig m_redDragon;
    float m_redDragonAnimation;
    
    // Intertwining effect
    float m_intertwineProgress;
    bool m_dragonsIntertwined;
    
    // Gold glow effect (at 100%)
    bool m_goldGlowActive;
    float m_goldGlowIntensity;
    float m_goldPulseTimer;
    
    // Helper functions
    void UpdateBlueDragon(float deltaTime);
    void UpdateRedDragon(float deltaTime);
    void UpdateIntertwining(float deltaTime);
    void UpdateGoldGlow(float deltaTime);
    
    void RenderBlueDragon();
    void RenderRedDragon();
    void RenderIntertwining();
    void RenderGoldGlow();
    
    // Dragon animation helpers
    float CalculateDragonCurve(float t, bool isBlue);
    void AnimateDragonMovement(DragonConfig& dragon, float deltaTime);
};

DragonGauge::DragonGauge()
    : m_gaugeValue(0.0f),
      m_blueDragonAnimation(0.0f),
      m_redDragonAnimation(0.0f),
      m_intertwineProgress(0.0f),
      m_dragonsIntertwined(false),
      m_goldGlowActive(false),
      m_goldGlowIntensity(0.0f),
      m_goldPulseTimer(0.0f) {
    
    // Initialize blue dragon (left side)
    m_blueDragon.headX = -5.0f;
    m_blueDragon.headY = 2.0f;
    m_blueDragon.bodySegments = 10.0f;
    m_blueDragon.animationSpeed = 2.0f;
    m_blueDragon.glowIntensity = 0.5f;
    m_blueDragon.isGlowing = false;
    
    // Initialize red dragon (right side)
    m_redDragon.headX = 5.0f;
    m_redDragon.headY = 2.0f;
    m_redDragon.bodySegments = 10.0f;
    m_redDragon.animationSpeed = 2.2f; // Slightly different timing
    m_redDragon.glowIntensity = 0.5f;
    m_redDragon.isGlowing = false;
}

DragonGauge::~DragonGauge() = default;

void DragonGauge::Update(float deltaTime, float gaugeValue) {
    m_gaugeValue = gaugeValue;
    
    UpdateBlueDragon(deltaTime);
    UpdateRedDragon(deltaTime);
    UpdateIntertwining(deltaTime);
    
    // Check for 100% gauge
    if (m_gaugeValue >= 100.0f && !m_goldGlowActive) {
        m_goldGlowActive = true;
        m_goldGlowIntensity = 1.0f;
    } else if (m_gaugeValue < 100.0f && m_goldGlowActive) {
        m_goldGlowActive = false;
    }
    
    if (m_goldGlowActive) {
        UpdateGoldGlow(deltaTime);
    }
}

void DragonGauge::Render() {
    RenderBlueDragon();
    RenderRedDragon();
    
    if (m_dragonsIntertwined) {
        RenderIntertwining();
    }
    
    if (m_goldGlowActive) {
        RenderGoldGlow();
    }
}

void DragonGauge::UpdateBlueDragon(float deltaTime) {
    m_blueDragonAnimation += deltaTime * m_blueDragon.animationSpeed;
    
    // Blue dragon becomes more active as gauge fills
    float activity = m_gaugeValue / 100.0f;
    m_blueDragon.glowIntensity = 0.3f + (activity * 0.7f);
    m_blueDragon.isGlowing = activity > 0.25f;
    
    // Animate dragon movement
    AnimateDragonMovement(m_blueDragon, deltaTime);
}

void DragonGauge::UpdateRedDragon(float deltaTime) {
    m_redDragonAnimation += deltaTime * m_redDragon.animationSpeed;
    
    // Red dragon becomes more active as gauge fills
    float activity = m_gaugeValue / 100.0f;
    m_redDragon.glowIntensity = 0.3f + (activity * 0.7f);
    m_redDragon.isGlowing = activity > 0.25f;
    
    // Animate dragon movement
    AnimateDragonMovement(m_redDragon, deltaTime);
}

void DragonGauge::UpdateIntertwining(float deltaTime) {
    // Dragons start intertwining at 50% gauge
    if (m_gaugeValue >= 50.0f) {
        if (!m_dragonsIntertwined) {
            m_dragonsIntertwined = true;
            m_intertwineProgress = 0.0f;
        }
        
        // Progress intertwining
        float targetProgress = (m_gaugeValue - 50.0f) / 50.0f; // 0 to 1
        m_intertwineProgress = std::lerp(m_intertwineProgress, targetProgress, deltaTime * 2.0f);
    } else {
        if (m_dragonsIntertwined) {
            m_intertwineProgress -= deltaTime * 3.0f;
            if (m_intertwineProgress <= 0.0f) {
                m_dragonsIntertwined = false;
                m_intertwineProgress = 0.0f;
            }
        }
    }
}

void DragonGauge::UpdateGoldGlow(float deltaTime) {
    m_goldPulseTimer += deltaTime;
    
    // Pulsing gold effect
    float pulse = 0.7f + 0.3f * sin(m_goldPulseTimer * 4.0f);
    m_goldGlowIntensity = pulse;
}

float DragonGauge::CalculateDragonCurve(float t, bool isBlue) {
    // S-curve for dragon body
    float curve = sin(t * 3.14159f) * (isBlue ? 1.0f : -1.0f);
    float animation = isBlue ? m_blueDragonAnimation : m_redDragonAnimation;
    
    // Add swimming motion
    curve += 0.3f * sin((t * 6.0f) + animation);
    
    return curve;
}

void DragonGauge::AnimateDragonMovement(DragonConfig& dragon, float deltaTime) {
    // Dragons slowly orbit around the gauge area
    static float orbitAngle = 0.0f;
    orbitAngle += deltaTime * 0.5f;
    
    // Subtle movement
    dragon.headX += cos(orbitAngle) * 0.1f * deltaTime;
    dragon.headY += sin(orbitAngle) * 0.05f * deltaTime;
}

void DragonGauge::RenderBlueDragon() {
    // Render blue dragon on left side
    // Implementation depends on graphics system
    
    // Dragon body segments
    for (int i = 0; i < static_cast<int>(m_blueDragon.bodySegments); i++) {
        float t = i / m_blueDragon.bodySegments;
        float x = m_blueDragon.headX + (t * 3.0f);
        float y = m_blueDragon.headY + CalculateDragonCurve(t, true);
        
        // Render segment at (x, y) with blue glow
    }
    
    // Dragon head with eyes
    if (m_blueDragon.isGlowing) {
        // Render glowing blue eyes
    }
}

void DragonGauge::RenderRedDragon() {
    // Render red dragon on right side
    // Implementation depends on graphics system
    
    // Dragon body segments
    for (int i = 0; i < static_cast<int>(m_redDragon.bodySegments); i++) {
        float t = i / m_redDragon.bodySegments;
        float x = m_redDragon.headX - (t * 3.0f);
        float y = m_redDragon.headY + CalculateDragonCurve(t, false);
        
        // Render segment at (x, y) with red glow
    }
    
    // Dragon head with eyes
    if (m_redDragon.isGlowing) {
        // Render glowing red eyes
    }
}

void DragonGauge::RenderIntertwining() {
    // Render intertwining effect between dragons
    if (m_intertwineProgress > 0.0f) {
        // Create connecting energy between dragons
        int connectionPoints = static_cast<int>(m_intertwineProgress * 20);
        
        for (int i = 0; i < connectionPoints; i++) {
            float t = i / static_cast<float>(connectionPoints);
            float x = std::lerp(m_blueDragon.headX, m_redDragon.headX, t);
            float y = m_blueDragon.headY + sin(t * 3.14159f) * m_intertwineProgress;
            
            // Render energy connection point
        }
    }
}

void DragonGauge::RenderGoldGlow() {
    if (m_goldGlowActive) {
        // Render gold aura around both dragons
        float glowRadius = 4.0f * m_goldGlowIntensity;
        
        // Blue dragon gold glow
        // Red dragon gold glow
        
        // Central gold energy where they meet
        if (m_dragonsIntertwined) {
            // Render central gold orb
        }
    }
}

} // namespace ArenaFighter