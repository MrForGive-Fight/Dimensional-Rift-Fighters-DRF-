#pragma once

#include <vector>
#include <memory>
#include <string>
#include "../HyukWoonSung.h"

namespace ArenaFighter {

// Color enum for VFX
enum class VFXColor {
    BLUE,
    RED,
    GOLD,
    BLACK,
    WHITE
};

// Particle types
enum class StanceParticleType {
    YIN_YANG_SHATTER,
    BLUE_STAR,
    RED_DEMON_SOUL,
    ENERGY_TRAIL,
    EXPLOSION,
    SHOCKWAVE,
    AURA,
    LIGHTNING,
    METEOR_FIRE,
    GHOSTLY_ESSENCE
};

// Base particle structure
struct StanceParticle {
    float x, y, z;
    float vx, vy, vz;
    float lifetime;
    float maxLifetime;
    float size;
    float alpha;
    VFXColor color;
    StanceParticleType type;
    bool hasTrail;
    bool affectedByGravity;
};

class StanceVFX {
public:
    static constexpr VFXColor BLUE = VFXColor::BLUE;
    static constexpr VFXColor RED = VFXColor::RED;
    static constexpr VFXColor GOLD = VFXColor::GOLD;
    
    StanceVFX();
    ~StanceVFX();
    
    // Main update and render
    void Update(float deltaTime);
    void Render();
    
    // Stance switching effects
    void PlayStanceSwitchEffect(StanceType newStance);
    void PlayYinYangShatter();
    void CreateRedSmoke();
    void CreateBlueShards();
    
    // Light stance effects
    void CreateThrustEffect(VFXColor color);
    void CreateStarPattern();
    void CreateBlueStarProjectiles(int count);
    void ChargeEnergy(VFXColor color, float duration);
    void CreateCrescentWave(VFXColor color, bool fullscreen);
    void BrightenScreen(float intensity);
    
    // Dark stance effects
    void CreatePalmStrike(VFXColor color);
    void CreateShockwave(VFXColor color);
    void CreateDoublePalm();
    void CreateExplosion(VFXColor color);
    void CreateSpinningPalm();
    void CreateDarkTrail();
    void CreateDemonFaceProjection();
    
    // S+Direction skill effects
    void CreateSpearSeaEffect();
    void CreateDivineWindEffect();
    void CreateLightningStitchEffect();
    void CreateHeavenlyDemonPowerEffect();
    void CreateBlackNightEffect();
    void CreateMindSplitEffect();
    
    // Gear skill effects
    void CreateGlassyRainEffect();
    void CreateSpearAuraEffect();
    void CreateDivineDragonFlow();
    void CreateThunderSpearEffect();
    void CreateWorldDestructionEffect();
    void CreateIntimidationEffect();
    void CreateDarkFlowerEffect();
    void CreateSkyPortal(bool massive);
    void CreateBigMeteor();
    
    // Ultimate effects
    void PlayUltimateTransformation();
    void PlayMastersVengeanceEffect();
    void CreateSixBondsEffect();
    void CreateDeathMoonEffect();
    void CreateBlueOceanEffect();
    void CreateFingerWindEffect();
    
    // General effect creators
    void CreateProjectile(VFXColor color, float speed, float size);
    void CreateExplosion(float x, float y, VFXColor color, float radius);
    void CreateBeam(float startX, float startY, float endX, float endY, VFXColor color);
    void CreateAura(VFXColor color, float intensity);
    void CreateTrail(VFXColor color, bool hasScreamingFaces = false);
    
    // Screen effects
    void FlashScreen(VFXColor color, float intensity, float duration);
    void ShakeScreen(float intensity, float duration);
    void ZoomEffect(float zoomLevel, float duration);
    
private:
    // Particle system
    std::vector<StanceParticle> m_particles;
    static constexpr int MAX_PARTICLES = 2000;
    
    // Effect state
    StanceType m_currentStance;
    float m_stanceTransitionTimer;
    bool m_isTransitioning;
    
    // Screen effects
    float m_screenFlashTimer;
    VFXColor m_flashColor;
    float m_flashIntensity;
    
    float m_screenShakeTimer;
    float m_shakeIntensity;
    
    // Aura effects
    float m_auraIntensity;
    VFXColor m_auraColor;
    float m_auraPulseTimer;
    
    // Special effects
    bool m_yinYangActive;
    float m_yinYangTimer;
    
    bool m_bigMeteorActive;
    float m_meteorX, m_meteorY, m_meteorZ;
    float m_meteorSpeed;
    
    // Color palettes
    struct ColorRGBA {
        float r, g, b, a;
    };
    
    static constexpr ColorRGBA LIGHT_BLUE = {0.3f, 0.6f, 1.0f, 0.8f};
    static constexpr ColorRGBA CELESTIAL_BLUE = {0.5f, 0.8f, 1.0f, 0.9f};
    static constexpr ColorRGBA DARK_RED = {0.8f, 0.1f, 0.1f, 0.9f};
    static constexpr ColorRGBA DEMON_BLACK = {0.2f, 0.0f, 0.0f, 0.8f};
    static constexpr ColorRGBA GOLD = {1.0f, 0.8f, 0.2f, 1.0f};
    static constexpr ColorRGBA WHITE = {1.0f, 1.0f, 1.0f, 0.8f};
    
    // Helper functions
    void SpawnParticle(StanceParticleType type, float x, float y, float z);
    void SpawnParticles(StanceParticleType type, int count, float radius);
    void UpdateParticles(float deltaTime);
    void CleanupParticles();
    
    // Specific effect generators
    void GenerateYinYangShatter();
    void GenerateBlueStars(int count);
    void GenerateDemonSouls(int count);
    void GenerateEnergyBeam(VFXColor color, float length);
    void GenerateExplosionParticles(VFXColor color, float radius);
    void GenerateAuraParticles(VFXColor color, float intensity);
    void GenerateLightningBolt(float length);
    void GenerateMeteorTrail();
    
    // Rendering helpers
    void RenderParticles();
    void RenderScreenEffects();
    void RenderAura();
    void RenderYinYang();
    void RenderMeteor();
    
    // Audio integration
    void PlayAudio(const std::string& audioFile);
    void StopAudio(const std::string& audioFile);
    
    // Utility functions
    ColorRGBA GetColorFromEnum(VFXColor color) const;
    float RandomFloat(float min, float max) const;
    void SetParticleTrajectory(StanceParticle& particle, float angle, float speed);
};

// VFX presets for specific techniques
namespace StanceVFXPresets {
    // Light stance presets
    void SevenStars(StanceVFX* vfx);
    void DivineSpearEndingNight(StanceVFX* vfx);
    void TravelOfGale(StanceVFX* vfx);
    void AerialSpearDance(StanceVFX* vfx);
    
    // Dark stance presets
    void HeavenlyDemonPalm(StanceVFX* vfx);
    void RedSoulCharge(StanceVFX* vfx);
    void HeavenlyDemonStep(StanceVFX* vfx);
    void HeavenlyDemonSpiral(StanceVFX* vfx);
    
    // Ultimate presets
    void DivineArtsTransformation(StanceVFX* vfx);
    void MastersVengeance(StanceVFX* vfx);
    void SixBondsUnited(StanceVFX* vfx);
    
    // THE BIG METEOR
    void BigMeteorEffect(StanceVFX* vfx);
};

} // namespace ArenaFighter