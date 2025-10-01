#pragma once

#include <vector>
#include <memory>
#include "../Rou.h"

namespace ArenaFighter {

// Particle types for evolution effects
enum class ParticleType {
    EVOLUTION_BURST,
    FORM_TRANSITION,
    AURA_PULSE,
    GAUGE_GLOW,
    EMERGENCY_FLASH,
    SKILL_TRAIL
};

// Base particle structure
struct Particle {
    float x, y, z;
    float vx, vy, vz;
    float lifetime;
    float size;
    float alpha;
    ParticleType type;
    RouEvolutionForm form;
};

// VFX configuration for each form
struct FormVFXConfig {
    float particleColor[4];      // RGBA
    float auraColor[4];          // RGBA
    float glowIntensity;
    float particleSize;
    int particleCount;
    float effectDuration;
    std::string modelPath;
    std::string texturePath;
};

class EvolutionVFX {
public:
    EvolutionVFX();
    ~EvolutionVFX();
    
    // Main update
    void Update(float deltaTime);
    void Render();
    
    // Evolution effects
    void PlayEvolutionEffect(RouEvolutionForm fromForm, RouEvolutionForm toForm);
    void PlayEmergencyProtocolEffect();
    void PlayGaugeThresholdEffect(float gaugePercent);
    
    // Form-specific effects
    void UpdateFormAura(RouEvolutionForm form, float intensity);
    void PlayFormSpecificEffect(RouEvolutionForm form, const std::string& effectName);
    
    // Special move effects
    void PlayGoblinPanicJump();
    void PlayGoblinSurvivalBite();
    void PlayGoblinRush();
    
    void PlayHobgoblinShadowUpper();
    void PlayHobgoblinDarkCounter();
    void PlayHobgoblinPhantomStrike();
    
    void PlayOgreSlam();
    void PlayOgreGroundQuake();
    void PlayOgreBrutalCharge();
    
    void PlayApostleDemonAscension();
    void PlayApostleLordsTerritory();
    void PlayApostleOrbBarrage();
    
    void PlayVajrayaksaHeavenSplitter();
    void PlayVajrayaksaOverlordsDecree();
    void PlayVajrayaksaThousandArms();
    
    // Gauge visual feedback
    void SetGaugeGlowIntensity(float intensity);
    void PulseGaugeEffect();
    
    // Size scaling animation
    void AnimateFormTransition(float fromScale, float toScale, float duration);
    
private:
    // VFX configurations for each form
    static constexpr FormVFXConfig FORM_CONFIGS[5] = {
        // Goblin - Green/Yellow theme
        {{0.5f, 0.8f, 0.3f, 1.0f}, {0.4f, 0.7f, 0.2f, 0.5f}, 0.3f, 0.5f, 50, 1.0f, "goblin", "goblin_tex"},
        // Hobgoblin - Purple/Red theme
        {{0.7f, 0.3f, 0.5f, 1.0f}, {0.6f, 0.2f, 0.4f, 0.5f}, 0.5f, 0.7f, 75, 1.2f, "hobgoblin", "hobgoblin_tex"},
        // Ogre - Orange/Brown theme
        {{0.8f, 0.5f, 0.2f, 1.0f}, {0.7f, 0.4f, 0.1f, 0.5f}, 0.7f, 1.0f, 100, 1.5f, "ogre", "ogre_tex"},
        // Apostle Lord - Black/Red theme
        {{0.9f, 0.1f, 0.1f, 1.0f}, {0.2f, 0.0f, 0.0f, 0.7f}, 0.9f, 1.2f, 150, 2.0f, "apostle", "apostle_tex"},
        // Vajrayaksa - Gold/White theme
        {{1.0f, 0.9f, 0.4f, 1.0f}, {1.0f, 1.0f, 0.8f, 0.8f}, 1.0f, 1.5f, 200, 2.5f, "vajrayaksa", "vajrayaksa_tex"}
    };
    
    // Active particles
    std::vector<Particle> m_particles;
    static constexpr int MAX_PARTICLES = 1000;
    
    // Current visual state
    RouEvolutionForm m_currentForm;
    float m_auraIntensity;
    float m_gaugeGlow;
    float m_transitionProgress;
    bool m_isTransitioning;
    
    // Effect timers
    float m_evolutionEffectTimer;
    float m_emergencyFlashTimer;
    float m_gaugeThresholdTimer;
    
    // Helper functions
    void SpawnParticle(ParticleType type, float x, float y, float z);
    void SpawnEvolutionBurst(RouEvolutionForm form, int count);
    void SpawnAuraParticles(RouEvolutionForm form);
    void UpdateParticles(float deltaTime);
    void CleanupParticles();
    
    // Rendering helpers
    void RenderParticles();
    void RenderAura(RouEvolutionForm form, float intensity);
    void RenderGaugeGlow(float intensity);
    
    // Effect generators
    void CreateSpiralEffect(float centerX, float centerY, int particleCount);
    void CreateExplosionEffect(float centerX, float centerY, float radius);
    void CreateTrailEffect(float startX, float startY, float endX, float endY);
    void CreatePillarEffect(float x, float y, float height);
    void CreateGroundCrackEffect(float x, float y, float length);
};

// Special effect presets
class VFXPresets {
public:
    // Evolution transitions
    static void GoblinToHobgoblin(EvolutionVFX* vfx);
    static void HobgoblinToOgre(EvolutionVFX* vfx);
    static void OgreToApostleLord(EvolutionVFX* vfx);
    static void ApostleLordToVajrayaksa(EvolutionVFX* vfx);
    
    // Emergency protocol effects
    static void EmergencyEvolution(EvolutionVFX* vfx, RouEvolutionForm targetForm);
    
    // Gauge threshold effects
    static void Gauge25Percent(EvolutionVFX* vfx);
    static void Gauge50Percent(EvolutionVFX* vfx);
    static void Gauge75Percent(EvolutionVFX* vfx);
    static void Gauge100Percent(EvolutionVFX* vfx);
};

} // namespace ArenaFighter