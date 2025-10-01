#include "StanceVFX.h"
#include <cmath>
#include <algorithm>
#include <random>

namespace ArenaFighter {

static std::random_device rd;
static std::mt19937 gen(rd());

StanceVFX::StanceVFX()
    : m_currentStance(StanceType::LIGHT_STANCE),
      m_stanceTransitionTimer(0.0f),
      m_isTransitioning(false),
      m_screenFlashTimer(0.0f),
      m_flashColor(VFXColor::WHITE),
      m_flashIntensity(0.0f),
      m_screenShakeTimer(0.0f),
      m_shakeIntensity(0.0f),
      m_auraIntensity(0.5f),
      m_auraColor(VFXColor::BLUE),
      m_auraPulseTimer(0.0f),
      m_yinYangActive(false),
      m_yinYangTimer(0.0f),
      m_bigMeteorActive(false),
      m_meteorX(0.0f), m_meteorY(20.0f), m_meteorZ(0.0f),
      m_meteorSpeed(5.0f) {
    
    m_particles.reserve(MAX_PARTICLES);
}

StanceVFX::~StanceVFX() = default;

void StanceVFX::Update(float deltaTime) {
    UpdateParticles(deltaTime);
    
    // Update timers
    if (m_screenFlashTimer > 0) {
        m_screenFlashTimer -= deltaTime;
    }
    
    if (m_screenShakeTimer > 0) {
        m_screenShakeTimer -= deltaTime;
    }
    
    if (m_yinYangActive) {
        m_yinYangTimer -= deltaTime;
        if (m_yinYangTimer <= 0) {
            m_yinYangActive = false;
        }
    }
    
    if (m_bigMeteorActive) {
        m_meteorY -= m_meteorSpeed * deltaTime;
        GenerateMeteorTrail();
        
        if (m_meteorY <= 0.0f) {
            CreateExplosion(m_meteorX, 0.0f, VFXColor::RED, 15.0f);
            m_bigMeteorActive = false;
        }
    }
    
    // Update aura pulse
    m_auraPulseTimer += deltaTime;
    float pulse = 0.7f + 0.3f * sin(m_auraPulseTimer * 3.0f);
    m_auraIntensity = pulse;
    
    // Spawn ambient aura particles
    if (m_auraIntensity > 0.1f) {
        GenerateAuraParticles(m_auraColor, m_auraIntensity);
    }
}

void StanceVFX::Render() {
    RenderParticles();
    RenderScreenEffects();
    RenderAura();
    
    if (m_yinYangActive) {
        RenderYinYang();
    }
    
    if (m_bigMeteorActive) {
        RenderMeteor();
    }
}

// Stance switching effects
void StanceVFX::PlayStanceSwitchEffect(StanceType newStance) {
    m_currentStance = newStance;
    m_isTransitioning = true;
    m_stanceTransitionTimer = 1.0f;
    
    PlayYinYangShatter();
    
    if (newStance == StanceType::LIGHT_STANCE) {
        CreateBlueShards();
        m_auraColor = VFXColor::BLUE;
    } else {
        CreateRedSmoke();
        m_auraColor = VFXColor::RED;
    }
    
    PlayAudio("stance_switch.wav");
}

void StanceVFX::PlayYinYangShatter() {
    m_yinYangActive = true;
    m_yinYangTimer = 0.8f;
    
    GenerateYinYangShatter();
    FlashScreen(VFXColor::WHITE, 0.3f, 0.2f);
    ShakeScreen(0.2f, 0.3f);
}

void StanceVFX::CreateRedSmoke() {
    SpawnParticles(StanceParticleType::RED_DEMON_SOUL, 100, 3.0f);
}

void StanceVFX::CreateBlueShards() {
    SpawnParticles(StanceParticleType::BLUE_STAR, 80, 2.5f);
}

// Light stance effects
void StanceVFX::CreateThrustEffect(VFXColor color) {
    GenerateEnergyBeam(color, 5.0f);
    CreateTrail(color);
}

void StanceVFX::CreateStarPattern() {
    GenerateBlueStars(7);
}

void StanceVFX::CreateBlueStarProjectiles(int count) {
    for (int i = 0; i < count; i++) {
        float angle = (i / static_cast<float>(count)) * 6.28f;
        SpawnParticle(StanceParticleType::BLUE_STAR, 
                     cos(angle) * 2.0f, 1.0f, sin(angle) * 2.0f);
    }
}

void StanceVFX::ChargeEnergy(VFXColor color, float duration) {
    for (int i = 0; i < 50; i++) {
        SpawnParticle(StanceParticleType::ENERGY_TRAIL, 0.0f, 2.0f, 0.0f);
    }
    
    CreateAura(color, 0.8f);
}

void StanceVFX::CreateCrescentWave(VFXColor color, bool fullscreen) {
    float range = fullscreen ? 20.0f : 10.0f;
    GenerateEnergyBeam(color, range);
    
    if (fullscreen) {
        BrightenScreen(0.5f);
    }
}

void StanceVFX::BrightenScreen(float intensity) {
    FlashScreen(VFXColor::WHITE, intensity, 0.3f);
}

// Dark stance effects
void StanceVFX::CreatePalmStrike(VFXColor color) {
    CreateShockwave(color);
    GenerateDemonSouls(20);
}

void StanceVFX::CreateShockwave(VFXColor color) {
    SpawnParticles(StanceParticleType::SHOCKWAVE, 50, 5.0f);
}

void StanceVFX::CreateDoublePalm() {
    CreateExplosion(VFXColor::RED);
    GenerateDemonSouls(30);
}

void StanceVFX::CreateExplosion(VFXColor color) {
    CreateExplosion(0.0f, 1.0f, color, 4.0f);
}

void StanceVFX::CreateSpinningPalm() {
    CreateDarkTrail();
    GenerateDemonSouls(15);
}

void StanceVFX::CreateDarkTrail() {
    CreateTrail(VFXColor::RED, true); // with screaming faces
}

void StanceVFX::CreateDemonFaceProjection() {
    SpawnParticles(StanceParticleType::RED_DEMON_SOUL, 80, 6.0f);
    
    // Create demon face visual
    for (int i = 0; i < 20; i++) {
        float angle = (i / 20.0f) * 6.28f;
        SpawnParticle(StanceParticleType::RED_DEMON_SOUL,
                     cos(angle) * 3.0f, 2.0f, sin(angle) * 3.0f);
    }
}

// S+Direction skill effects
void StanceVFX::CreateSpearSeaEffect() {
    // Massive spear projection with stars
    GenerateBlueStars(50);
    GenerateEnergyBeam(VFXColor::BLUE, 15.0f);
    FlashScreen(VFXColor::BLUE, 0.4f, 0.5f);
}

void StanceVFX::CreateDivineWindEffect() {
    // Wind spiral with blue particles
    for (int i = 0; i < 100; i++) {
        float t = i / 100.0f;
        float angle = t * 12.56f; // 2 rotations
        float radius = t * 8.0f;
        
        SpawnParticle(StanceParticleType::BLUE_STAR,
                     cos(angle) * radius, t * 5.0f, sin(angle) * radius);
    }
}

void StanceVFX::CreateLightningStitchEffect() {
    GenerateLightningBolt(8.0f);
    GenerateBlueStars(25);
}

void StanceVFX::CreateHeavenlyDemonPowerEffect() {
    GenerateDemonSouls(60);
    CreateExplosion(0.0f, 2.0f, VFXColor::RED, 8.0f);
    FlashScreen(VFXColor::RED, 0.5f, 0.4f);
}

void StanceVFX::CreateBlackNightEffect() {
    // Moon silhouette effect
    CreateAura(VFXColor::BLACK, 1.0f);
    GenerateDemonSouls(40);
}

void StanceVFX::CreateMindSplitEffect() {
    // Double will effect
    GenerateDemonSouls(30);
    CreateTrail(VFXColor::RED, true);
}

// THE BIG METEOR implementation
void StanceVFX::CreateSkyPortal(bool massive) {
    float portalSize = massive ? 20.0f : 8.0f;
    
    // Create portal particles in circle
    for (int i = 0; i < 100; i++) {
        float angle = (i / 100.0f) * 6.28f;
        SpawnParticle(StanceParticleType::RED_DEMON_SOUL,
                     cos(angle) * portalSize, 15.0f, sin(angle) * portalSize);
    }
    
    FlashScreen(VFXColor::RED, 0.6f, 1.0f);
}

void StanceVFX::CreateBigMeteor() {
    m_bigMeteorActive = true;
    m_meteorX = 0.0f;
    m_meteorY = 18.0f;
    m_meteorZ = 0.0f;
    m_meteorSpeed = 3.0f; // Slow descent for dramatic effect
    
    // Create demon face on meteor
    for (int i = 0; i < 50; i++) {
        SpawnParticle(StanceParticleType::RED_DEMON_SOUL, 
                     m_meteorX, m_meteorY, m_meteorZ);
    }
}

// Ultimate effects
void StanceVFX::PlayUltimateTransformation() {
    // Gold and black pillar
    for (int i = 0; i < 200; i++) {
        float height = (i / 200.0f) * 15.0f;
        SpawnParticle(StanceParticleType::ENERGY_TRAIL, 0.0f, height, 0.0f);
    }
    
    FlashScreen(VFXColor::GOLD, 0.8f, 2.0f);
    ShakeScreen(0.3f, 1.5f);
}

void StanceVFX::PlayMastersVengeanceEffect() {
    // Ghostly master images
    CreateAura(VFXColor::GOLD, 1.0f);
    
    // Eyes glow effect
    SpawnParticles(StanceParticleType::ENERGY_TRAIL, 20, 1.0f);
    
    FlashScreen(VFXColor::GOLD, 0.6f, 1.0f);
}

void StanceVFX::CreateSixBondsEffect() {
    // Ultimate technique with combined elements
    GenerateBlueStars(100);
    GenerateDemonSouls(100);
    CreateExplosion(0.0f, 2.0f, VFXColor::GOLD, 12.0f);
}

void StanceVFX::CreateDeathMoonEffect() {
    // 4th Bond - Death Moon
    CreateAura(VFXColor::BLACK, 1.0f);
    GenerateDemonSouls(80);
}

void StanceVFX::CreateBlueOceanEffect() {
    // 6th Bond - Blue Ocean
    GenerateBlueStars(150);
    CreateAura(VFXColor::BLUE, 1.0f);
}

void StanceVFX::CreateFingerWindEffect() {
    // Precise finger technique
    GenerateLightningBolt(6.0f);
    GenerateEnergyBeam(VFXColor::WHITE, 8.0f);
}

// Helper implementations
void StanceVFX::SpawnParticle(StanceParticleType type, float x, float y, float z) {
    if (m_particles.size() >= MAX_PARTICLES) return;
    
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
    
    StanceParticle p;
    p.type = type;
    p.x = x;
    p.y = y;
    p.z = z;
    p.vx = dis(gen) * 3.0f;
    p.vy = dis(gen) * 3.0f;
    p.vz = dis(gen) * 3.0f;
    p.lifetime = 2.0f;
    p.maxLifetime = 2.0f;
    p.size = 0.5f;
    p.alpha = 1.0f;
    p.hasTrail = false;
    p.affectedByGravity = true;
    
    // Set color based on type and stance
    switch (type) {
        case StanceParticleType::BLUE_STAR:
            p.color = VFXColor::BLUE;
            break;
        case StanceParticleType::RED_DEMON_SOUL:
            p.color = VFXColor::RED;
            break;
        case StanceParticleType::YIN_YANG_SHATTER:
            p.color = VFXColor::WHITE;
            break;
        default:
            p.color = m_currentStance == StanceType::LIGHT_STANCE ? VFXColor::BLUE : VFXColor::RED;
            break;
    }
    
    m_particles.push_back(p);
}

void StanceVFX::SpawnParticles(StanceParticleType type, int count, float radius) {
    for (int i = 0; i < count; i++) {
        float angle = (i / static_cast<float>(count)) * 6.28f;
        float r = RandomFloat(0.5f, 1.0f) * radius;
        
        SpawnParticle(type, 
                     cos(angle) * r,
                     RandomFloat(0.0f, 2.0f),
                     sin(angle) * r);
    }
}

void StanceVFX::UpdateParticles(float deltaTime) {
    for (auto it = m_particles.begin(); it != m_particles.end();) {
        it->lifetime -= deltaTime;
        
        if (it->lifetime <= 0) {
            it = m_particles.erase(it);
            continue;
        }
        
        // Update position
        it->x += it->vx * deltaTime;
        it->y += it->vy * deltaTime;
        it->z += it->vz * deltaTime;
        
        // Apply gravity
        if (it->affectedByGravity) {
            it->vy -= 9.8f * deltaTime;
        }
        
        // Update alpha based on lifetime
        it->alpha = it->lifetime / it->maxLifetime;
        
        ++it;
    }
}

void StanceVFX::CleanupParticles() {
    m_particles.erase(
        std::remove_if(m_particles.begin(), m_particles.end(),
            [](const StanceParticle& p) { return p.lifetime <= 0; }),
        m_particles.end()
    );
}

// Specific effect generators
void StanceVFX::GenerateYinYangShatter() {
    // Create yin-yang symbol that shatters
    for (int i = 0; i < 60; i++) {
        float angle = (i / 60.0f) * 6.28f;
        bool isYin = i < 30;
        
        StanceParticle p;
        p.type = StanceParticleType::YIN_YANG_SHATTER;
        p.x = cos(angle) * 2.0f;
        p.y = 1.5f;
        p.z = sin(angle) * 2.0f;
        
        float speed = 8.0f;
        p.vx = cos(angle) * speed;
        p.vy = RandomFloat(0.0f, 5.0f);
        p.vz = sin(angle) * speed;
        
        p.lifetime = 1.5f;
        p.maxLifetime = 1.5f;
        p.size = 0.3f;
        p.alpha = 1.0f;
        p.color = isYin ? VFXColor::WHITE : VFXColor::BLACK;
        p.hasTrail = true;
        p.affectedByGravity = true;
        
        m_particles.push_back(p);
    }
}

void StanceVFX::GenerateBlueStars(int count) {
    for (int i = 0; i < count; i++) {
        SpawnParticle(StanceParticleType::BLUE_STAR,
                     RandomFloat(-3.0f, 3.0f),
                     RandomFloat(0.0f, 3.0f),
                     RandomFloat(-3.0f, 3.0f));
    }
}

void StanceVFX::GenerateDemonSouls(int count) {
    for (int i = 0; i < count; i++) {
        SpawnParticle(StanceParticleType::RED_DEMON_SOUL,
                     RandomFloat(-4.0f, 4.0f),
                     RandomFloat(0.0f, 4.0f),
                     RandomFloat(-4.0f, 4.0f));
    }
}

void StanceVFX::GenerateEnergyBeam(VFXColor color, float length) {
    int particleCount = static_cast<int>(length * 10);
    for (int i = 0; i < particleCount; i++) {
        float t = i / static_cast<float>(particleCount);
        SpawnParticle(StanceParticleType::ENERGY_TRAIL,
                     t * length, 1.0f, 0.0f);
    }
}

void StanceVFX::GenerateExplosionParticles(VFXColor color, float radius) {
    int count = static_cast<int>(radius * 20);
    for (int i = 0; i < count; i++) {
        float angle = RandomFloat(0.0f, 6.28f);
        float speed = RandomFloat(3.0f, 12.0f);
        
        StanceParticle p;
        p.type = StanceParticleType::EXPLOSION;
        p.x = 0.0f;
        p.y = 1.0f;
        p.z = 0.0f;
        p.vx = cos(angle) * speed;
        p.vy = RandomFloat(0.0f, 8.0f);
        p.vz = sin(angle) * speed;
        p.lifetime = 1.0f;
        p.maxLifetime = 1.0f;
        p.size = 0.8f;
        p.alpha = 1.0f;
        p.color = color;
        p.hasTrail = false;
        p.affectedByGravity = true;
        
        m_particles.push_back(p);
    }
}

void StanceVFX::GenerateAuraParticles(VFXColor color, float intensity) {
    int count = static_cast<int>(intensity * 5);
    for (int i = 0; i < count; i++) {
        float angle = RandomFloat(0.0f, 6.28f);
        float dist = RandomFloat(1.5f, 3.0f);
        
        SpawnParticle(StanceParticleType::AURA,
                     cos(angle) * dist,
                     RandomFloat(0.0f, 3.0f),
                     sin(angle) * dist);
    }
}

void StanceVFX::GenerateLightningBolt(float length) {
    // Jagged lightning path
    int segments = 20;
    float segmentLength = length / segments;
    
    for (int i = 0; i < segments; i++) {
        float t = i / static_cast<float>(segments);
        float offset = RandomFloat(-0.5f, 0.5f);
        
        SpawnParticle(StanceParticleType::LIGHTNING,
                     t * length + offset,
                     2.0f + RandomFloat(-0.3f, 0.3f),
                     offset * 0.5f);
    }
}

void StanceVFX::GenerateMeteorTrail() {
    // Fire trail behind meteor
    for (int i = 0; i < 10; i++) {
        SpawnParticle(StanceParticleType::METEOR_FIRE,
                     m_meteorX + RandomFloat(-0.5f, 0.5f),
                     m_meteorY + RandomFloat(0.0f, 2.0f),
                     m_meteorZ + RandomFloat(-0.5f, 0.5f));
    }
}

// Screen and render effects
void StanceVFX::FlashScreen(VFXColor color, float intensity, float duration) {
    m_flashColor = color;
    m_flashIntensity = intensity;
    m_screenFlashTimer = duration;
}

void StanceVFX::ShakeScreen(float intensity, float duration) {
    m_shakeIntensity = intensity;
    m_screenShakeTimer = duration;
}

void StanceVFX::CreateAura(VFXColor color, float intensity) {
    m_auraColor = color;
    m_auraIntensity = intensity;
}

void StanceVFX::CreateTrail(VFXColor color, bool hasScreamingFaces) {
    SpawnParticles(StanceParticleType::ENERGY_TRAIL, 30, 1.0f);
}

void StanceVFX::CreateExplosion(float x, float y, VFXColor color, float radius) {
    GenerateExplosionParticles(color, radius);
    ShakeScreen(0.2f, 0.5f);
}

// Rendering functions (interface with graphics system)
void StanceVFX::RenderParticles() {
    // Rendering handled by graphics system
}

void StanceVFX::RenderScreenEffects() {
    // Screen flash and shake handled by renderer
}

void StanceVFX::RenderAura() {
    // Aura rendering handled by graphics system
}

void StanceVFX::RenderYinYang() {
    // Yin-yang symbol rendering
}

void StanceVFX::RenderMeteor() {
    // Big meteor rendering with demon face
}

// Utility functions
StanceVFX::ColorRGBA StanceVFX::GetColorFromEnum(VFXColor color) const {
    switch (color) {
        case VFXColor::BLUE: return LIGHT_BLUE;
        case VFXColor::RED: return DARK_RED;
        case VFXColor::GOLD: return GOLD;
        case VFXColor::BLACK: return DEMON_BLACK;
        case VFXColor::WHITE: return WHITE;
        default: return WHITE;
    }
}

float StanceVFX::RandomFloat(float min, float max) const {
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

void StanceVFX::SetParticleTrajectory(StanceParticle& particle, float angle, float speed) {
    particle.vx = cos(angle) * speed;
    particle.vy = sin(angle) * speed * 0.5f; // Slight upward arc
    particle.vz = sin(angle * 2.0f) * speed * 0.3f; // Some Z variation
}

void StanceVFX::PlayAudio(const std::string& audioFile) {
    // Audio system integration
}

void StanceVFX::StopAudio(const std::string& audioFile) {
    // Audio system integration
}

// VFX Preset implementations
void StanceVFXPresets::SevenStars(StanceVFX* vfx) {
    vfx->GenerateBlueStars(7);
    vfx->CreateCrescentWave(VFXColor::BLUE, false);
}

void StanceVFXPresets::DivineSpearEndingNight(StanceVFX* vfx) {
    vfx->ChargeEnergy(VFXColor::BLUE, 1.0f);
    vfx->CreateCrescentWave(VFXColor::BLUE, true);
    vfx->BrightenScreen(0.5f);
}

void StanceVFXPresets::HeavenlyDemonPalm(StanceVFX* vfx) {
    vfx->CreatePalmStrike(VFXColor::RED);
    vfx->CreateDemonFaceProjection();
}

void StanceVFXPresets::BigMeteorEffect(StanceVFX* vfx) {
    vfx->CreateSkyPortal(true);
    vfx->CreateBigMeteor();
    vfx->FlashScreen(VFXColor::RED, 0.8f, 2.0f);
}

void StanceVFXPresets::DivineArtsTransformation(StanceVFX* vfx) {
    vfx->PlayUltimateTransformation();
}

void StanceVFXPresets::MastersVengeance(StanceVFX* vfx) {
    vfx->PlayMastersVengeanceEffect();
}

void StanceVFXPresets::SixBondsUnited(StanceVFX* vfx) {
    vfx->CreateSixBondsEffect();
}

} // namespace ArenaFighter