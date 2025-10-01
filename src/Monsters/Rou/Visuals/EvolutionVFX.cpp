#include "EvolutionVFX.h"
#include <cmath>
#include <algorithm>
#include <random>

namespace ArenaFighter {

static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

EvolutionVFX::EvolutionVFX()
    : m_currentForm(RouEvolutionForm::GOBLIN),
      m_auraIntensity(0.3f),
      m_gaugeGlow(0.0f),
      m_transitionProgress(0.0f),
      m_isTransitioning(false),
      m_evolutionEffectTimer(0.0f),
      m_emergencyFlashTimer(0.0f),
      m_gaugeThresholdTimer(0.0f) {
    
    m_particles.reserve(MAX_PARTICLES);
}

EvolutionVFX::~EvolutionVFX() = default;

void EvolutionVFX::Update(float deltaTime) {
    UpdateParticles(deltaTime);
    
    // Update timers
    if (m_evolutionEffectTimer > 0) {
        m_evolutionEffectTimer -= deltaTime;
    }
    
    if (m_emergencyFlashTimer > 0) {
        m_emergencyFlashTimer -= deltaTime;
    }
    
    if (m_gaugeThresholdTimer > 0) {
        m_gaugeThresholdTimer -= deltaTime;
    }
    
    // Update transition
    if (m_isTransitioning) {
        m_transitionProgress += deltaTime * 2.0f; // 0.5 second transition
        if (m_transitionProgress >= 1.0f) {
            m_transitionProgress = 1.0f;
            m_isTransitioning = false;
        }
    }
    
    // Spawn ambient aura particles
    if (m_auraIntensity > 0.1f) {
        SpawnAuraParticles(m_currentForm);
    }
}

void EvolutionVFX::Render() {
    RenderAura(m_currentForm, m_auraIntensity);
    RenderParticles();
    RenderGaugeGlow(m_gaugeGlow);
}

void EvolutionVFX::PlayEvolutionEffect(RouEvolutionForm fromForm, RouEvolutionForm toForm) {
    m_evolutionEffectTimer = FORM_CONFIGS[static_cast<int>(toForm)].effectDuration;
    m_isTransitioning = true;
    m_transitionProgress = 0.0f;
    
    // Spawn evolution burst
    SpawnEvolutionBurst(toForm, 200);
    
    // Create spiral effect
    CreateSpiralEffect(0.0f, 0.0f, 100);
    
    // Update current form
    m_currentForm = toForm;
    
    // Increase aura intensity
    m_auraIntensity = 1.0f;
}

void EvolutionVFX::PlayEmergencyProtocolEffect() {
    m_emergencyFlashTimer = 3.0f;
    
    // Create emergency flash
    for (int i = 0; i < 300; i++) {
        Particle p;
        p.type = ParticleType::EMERGENCY_FLASH;
        p.x = dis(gen) * 5.0f;
        p.y = dis(gen) * 5.0f;
        p.z = dis(gen) * 5.0f;
        p.vx = dis(gen) * 10.0f;
        p.vy = std::abs(dis(gen)) * 15.0f;
        p.vz = dis(gen) * 10.0f;
        p.lifetime = 2.0f;
        p.size = 1.5f;
        p.alpha = 1.0f;
        p.form = m_currentForm;
        m_particles.push_back(p);
    }
}

void EvolutionVFX::PlayGaugeThresholdEffect(float gaugePercent) {
    m_gaugeThresholdTimer = 1.0f;
    m_gaugeGlow = gaugePercent / 100.0f;
    
    // Pulse effect
    PulseGaugeEffect();
}

void EvolutionVFX::UpdateFormAura(RouEvolutionForm form, float intensity) {
    m_currentForm = form;
    m_auraIntensity = intensity;
}

void EvolutionVFX::PlayFormSpecificEffect(RouEvolutionForm form, const std::string& effectName) {
    // Play specific visual effect based on form and effect name
}

// Goblin special effects
void EvolutionVFX::PlayGoblinPanicJump() {
    // Quick dust cloud
    CreateExplosionEffect(0.0f, -2.0f, 3.0f);
}

void EvolutionVFX::PlayGoblinSurvivalBite() {
    // Green healing particles
    for (int i = 0; i < 20; i++) {
        Particle p;
        p.type = ParticleType::SKILL_TRAIL;
        p.x = dis(gen) * 0.5f;
        p.y = dis(gen) * 0.5f + 1.0f;
        p.z = 0.0f;
        p.vx = 0.0f;
        p.vy = 2.0f;
        p.vz = 0.0f;
        p.lifetime = 1.0f;
        p.size = 0.3f;
        p.alpha = 0.8f;
        p.form = RouEvolutionForm::GOBLIN;
        m_particles.push_back(p);
    }
}

void EvolutionVFX::PlayGoblinRush() {
    // Speed lines
    CreateTrailEffect(-5.0f, 0.0f, 5.0f, 0.0f);
}

// Hobgoblin special effects
void EvolutionVFX::PlayHobgoblinShadowUpper() {
    // Rising shadow trail
    CreatePillarEffect(0.0f, 0.0f, 5.0f);
}

void EvolutionVFX::PlayHobgoblinDarkCounter() {
    // Dark explosion
    CreateExplosionEffect(0.0f, 1.0f, 5.0f);
}

void EvolutionVFX::PlayHobgoblinPhantomStrike() {
    // Teleport particles
    for (int i = 0; i < 50; i++) {
        Particle p;
        p.type = ParticleType::SKILL_TRAIL;
        float angle = (i / 50.0f) * 6.28f;
        p.x = cos(angle) * 2.0f;
        p.y = 1.0f;
        p.z = sin(angle) * 2.0f;
        p.vx = -p.x * 2.0f;
        p.vy = 0.0f;
        p.vz = -p.z * 2.0f;
        p.lifetime = 0.5f;
        p.size = 0.5f;
        p.alpha = 1.0f;
        p.form = RouEvolutionForm::HOBGOBLIN;
        m_particles.push_back(p);
    }
}

// Ogre special effects
void EvolutionVFX::PlayOgreSlam() {
    // Ground impact shockwave
    CreateGroundCrackEffect(0.0f, 0.0f, 10.0f);
    CreateExplosionEffect(0.0f, 0.0f, 8.0f);
}

void EvolutionVFX::PlayOgreGroundQuake() {
    // Earth spikes
    for (int i = 0; i < 5; i++) {
        float x = (i - 2) * 2.0f;
        CreatePillarEffect(x, 0.0f, 3.0f);
    }
}

void EvolutionVFX::PlayOgreBrutalCharge() {
    // Charge trail
    CreateTrailEffect(-8.0f, 0.0f, 8.0f, 0.0f);
}

// Apostle Lord special effects
void EvolutionVFX::PlayApostleDemonAscension() {
    // Rising demon energy
    CreatePillarEffect(0.0f, 0.0f, 10.0f);
    
    // Demon orbs
    for (int i = 0; i < 5; i++) {
        float angle = (i / 5.0f) * 6.28f;
        Particle p;
        p.type = ParticleType::SKILL_TRAIL;
        p.x = cos(angle) * 3.0f;
        p.y = 10.0f;
        p.z = sin(angle) * 3.0f;
        p.vx = 0.0f;
        p.vy = -5.0f;
        p.vz = 0.0f;
        p.lifetime = 2.0f;
        p.size = 1.0f;
        p.alpha = 1.0f;
        p.form = RouEvolutionForm::APOSTLE_LORD;
        m_particles.push_back(p);
    }
}

void EvolutionVFX::PlayApostleLordsTerritory() {
    // Territory field
    CreateSpiralEffect(0.0f, 0.0f, 150);
}

void EvolutionVFX::PlayApostleOrbBarrage() {
    // Homing orbs
    for (int i = 0; i < 3; i++) {
        float angle = (i / 3.0f) * 6.28f;
        CreateTrailEffect(0.0f, 2.0f, cos(angle) * 10.0f, sin(angle) * 10.0f);
    }
}

// Vajrayaksa special effects
void EvolutionVFX::PlayVajrayaksaHeavenSplitter() {
    // Massive energy pillar
    CreatePillarEffect(0.0f, 0.0f, 20.0f);
    CreateExplosionEffect(0.0f, 10.0f, 15.0f);
}

void EvolutionVFX::PlayVajrayaksaOverlordsDecree() {
    // Fear wave
    CreateExplosionEffect(0.0f, 2.0f, 20.0f);
}

void EvolutionVFX::PlayVajrayaksaThousandArms() {
    // Multiple strike effects
    for (int i = 0; i < 20; i++) {
        float x = dis(gen) * 5.0f;
        float y = dis(gen) * 3.0f + 1.0f;
        float z = dis(gen) * 5.0f;
        CreateTrailEffect(x, y, x + dis(gen) * 2.0f, y + dis(gen) * 2.0f);
    }
}

void EvolutionVFX::SetGaugeGlowIntensity(float intensity) {
    m_gaugeGlow = std::clamp(intensity, 0.0f, 1.0f);
}

void EvolutionVFX::PulseGaugeEffect() {
    // Create pulse ring
    for (int i = 0; i < 50; i++) {
        float angle = (i / 50.0f) * 6.28f;
        Particle p;
        p.type = ParticleType::GAUGE_GLOW;
        p.x = cos(angle) * 3.0f;
        p.y = 0.0f;
        p.z = sin(angle) * 3.0f;
        p.vx = p.x * 0.5f;
        p.vy = 0.0f;
        p.vz = p.z * 0.5f;
        p.lifetime = 1.0f;
        p.size = 0.5f;
        p.alpha = m_gaugeGlow;
        p.form = m_currentForm;
        m_particles.push_back(p);
    }
}

void EvolutionVFX::AnimateFormTransition(float fromScale, float toScale, float duration) {
    // Handled by render system with interpolation
}

// Helper functions
void EvolutionVFX::SpawnParticle(ParticleType type, float x, float y, float z) {
    if (m_particles.size() >= MAX_PARTICLES) return;
    
    Particle p;
    p.type = type;
    p.x = x;
    p.y = y;
    p.z = z;
    p.vx = dis(gen) * 2.0f;
    p.vy = dis(gen) * 2.0f;
    p.vz = dis(gen) * 2.0f;
    p.lifetime = 1.0f;
    p.size = 0.5f;
    p.alpha = 1.0f;
    p.form = m_currentForm;
    m_particles.push_back(p);
}

void EvolutionVFX::SpawnEvolutionBurst(RouEvolutionForm form, int count) {
    for (int i = 0; i < count; i++) {
        Particle p;
        p.type = ParticleType::EVOLUTION_BURST;
        p.x = dis(gen) * 0.5f;
        p.y = dis(gen) * 0.5f + 1.0f;
        p.z = dis(gen) * 0.5f;
        
        float speed = 5.0f + std::abs(dis(gen)) * 10.0f;
        float angle = (i / static_cast<float>(count)) * 6.28f;
        float vertAngle = dis(gen) * 1.57f;
        
        p.vx = cos(angle) * sin(vertAngle) * speed;
        p.vy = cos(vertAngle) * speed;
        p.vz = sin(angle) * sin(vertAngle) * speed;
        
        p.lifetime = 2.0f;
        p.size = FORM_CONFIGS[static_cast<int>(form)].particleSize;
        p.alpha = 1.0f;
        p.form = form;
        m_particles.push_back(p);
    }
}

void EvolutionVFX::SpawnAuraParticles(RouEvolutionForm form) {
    // Spawn 1-2 particles per frame for aura
    int count = static_cast<int>(m_auraIntensity * 2.0f);
    for (int i = 0; i < count; i++) {
        Particle p;
        p.type = ParticleType::AURA_PULSE;
        
        float angle = dis(gen) * 6.28f;
        float dist = 2.0f + dis(gen) * 1.0f;
        p.x = cos(angle) * dist;
        p.y = dis(gen) * 3.0f;
        p.z = sin(angle) * dist;
        
        p.vx = -p.x * 0.1f;
        p.vy = 0.5f;
        p.vz = -p.z * 0.1f;
        
        p.lifetime = 2.0f;
        p.size = 0.3f * m_auraIntensity;
        p.alpha = 0.5f * m_auraIntensity;
        p.form = form;
        m_particles.push_back(p);
    }
}

void EvolutionVFX::UpdateParticles(float deltaTime) {
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
        
        // Apply gravity to some particle types
        if (it->type == ParticleType::EVOLUTION_BURST || 
            it->type == ParticleType::SKILL_TRAIL) {
            it->vy -= 9.8f * deltaTime;
        }
        
        // Fade out
        it->alpha = it->lifetime / 2.0f;
        
        ++it;
    }
}

void EvolutionVFX::CleanupParticles() {
    m_particles.erase(
        std::remove_if(m_particles.begin(), m_particles.end(),
            [](const Particle& p) { return p.lifetime <= 0; }),
        m_particles.end()
    );
}

void EvolutionVFX::RenderParticles() {
    // Rendering implementation depends on graphics API
}

void EvolutionVFX::RenderAura(RouEvolutionForm form, float intensity) {
    // Rendering implementation depends on graphics API
}

void EvolutionVFX::RenderGaugeGlow(float intensity) {
    // Rendering implementation depends on graphics API
}

void EvolutionVFX::CreateSpiralEffect(float centerX, float centerY, int particleCount) {
    for (int i = 0; i < particleCount; i++) {
        float t = i / static_cast<float>(particleCount);
        float angle = t * 12.56f; // 2 full rotations
        float radius = t * 5.0f;
        
        Particle p;
        p.type = ParticleType::SKILL_TRAIL;
        p.x = centerX + cos(angle) * radius;
        p.y = centerY + t * 5.0f;
        p.z = sin(angle) * radius;
        p.vx = 0.0f;
        p.vy = 2.0f;
        p.vz = 0.0f;
        p.lifetime = 1.5f;
        p.size = 0.5f;
        p.alpha = 1.0f;
        p.form = m_currentForm;
        m_particles.push_back(p);
    }
}

void EvolutionVFX::CreateExplosionEffect(float centerX, float centerY, float radius) {
    int count = static_cast<int>(radius * 10);
    for (int i = 0; i < count; i++) {
        Particle p;
        p.type = ParticleType::SKILL_TRAIL;
        p.x = centerX;
        p.y = centerY;
        p.z = 0.0f;
        
        float angle = dis(gen) * 6.28f;
        float speed = radius * (0.5f + std::abs(dis(gen)) * 0.5f);
        p.vx = cos(angle) * speed;
        p.vy = std::abs(dis(gen)) * speed;
        p.vz = sin(angle) * speed;
        
        p.lifetime = 0.8f;
        p.size = 0.8f;
        p.alpha = 1.0f;
        p.form = m_currentForm;
        m_particles.push_back(p);
    }
}

void EvolutionVFX::CreateTrailEffect(float startX, float startY, float endX, float endY) {
    float dx = endX - startX;
    float dy = endY - startY;
    float distance = sqrt(dx * dx + dy * dy);
    int count = static_cast<int>(distance * 5);
    
    for (int i = 0; i < count; i++) {
        float t = i / static_cast<float>(count);
        Particle p;
        p.type = ParticleType::SKILL_TRAIL;
        p.x = startX + dx * t;
        p.y = startY + dy * t;
        p.z = 0.0f;
        p.vx = dis(gen) * 0.5f;
        p.vy = dis(gen) * 0.5f;
        p.vz = dis(gen) * 0.5f;
        p.lifetime = 0.5f;
        p.size = 0.4f;
        p.alpha = 1.0f - t;
        p.form = m_currentForm;
        m_particles.push_back(p);
    }
}

void EvolutionVFX::CreatePillarEffect(float x, float y, float height) {
    int count = static_cast<int>(height * 10);
    for (int i = 0; i < count; i++) {
        float h = (i / static_cast<float>(count)) * height;
        Particle p;
        p.type = ParticleType::SKILL_TRAIL;
        
        float angle = dis(gen) * 6.28f;
        float radius = 0.5f + dis(gen) * 0.5f;
        p.x = x + cos(angle) * radius;
        p.y = y + h;
        p.z = sin(angle) * radius;
        
        p.vx = cos(angle) * 2.0f;
        p.vy = 0.0f;
        p.vz = sin(angle) * 2.0f;
        
        p.lifetime = 1.0f;
        p.size = 0.6f;
        p.alpha = 1.0f;
        p.form = m_currentForm;
        m_particles.push_back(p);
    }
}

void EvolutionVFX::CreateGroundCrackEffect(float x, float y, float length) {
    int segments = static_cast<int>(length * 2);
    for (int i = 0; i < segments; i++) {
        float t = i / static_cast<float>(segments);
        float crack_x = x + (dis(gen) * 0.5f + t) * length - length/2;
        
        for (int j = 0; j < 5; j++) {
            Particle p;
            p.type = ParticleType::SKILL_TRAIL;
            p.x = crack_x;
            p.y = y;
            p.z = dis(gen) * 0.5f;
            p.vx = 0.0f;
            p.vy = 3.0f + std::abs(dis(gen)) * 2.0f;
            p.vz = 0.0f;
            p.lifetime = 0.8f;
            p.size = 0.5f;
            p.alpha = 1.0f;
            p.form = m_currentForm;
            m_particles.push_back(p);
        }
    }
}

// VFX Preset implementations
void VFXPresets::GoblinToHobgoblin(EvolutionVFX* vfx) {
    vfx->PlayEvolutionEffect(RouEvolutionForm::GOBLIN, RouEvolutionForm::HOBGOBLIN);
}

void VFXPresets::HobgoblinToOgre(EvolutionVFX* vfx) {
    vfx->PlayEvolutionEffect(RouEvolutionForm::HOBGOBLIN, RouEvolutionForm::OGRE);
}

void VFXPresets::OgreToApostleLord(EvolutionVFX* vfx) {
    vfx->PlayEvolutionEffect(RouEvolutionForm::OGRE, RouEvolutionForm::APOSTLE_LORD);
}

void VFXPresets::ApostleLordToVajrayaksa(EvolutionVFX* vfx) {
    vfx->PlayEvolutionEffect(RouEvolutionForm::APOSTLE_LORD, RouEvolutionForm::VAJRAYAKSA);
}

void VFXPresets::EmergencyEvolution(EvolutionVFX* vfx, RouEvolutionForm targetForm) {
    vfx->PlayEmergencyProtocolEffect();
    vfx->UpdateFormAura(targetForm, 1.0f);
}

void VFXPresets::Gauge25Percent(EvolutionVFX* vfx) {
    vfx->PlayGaugeThresholdEffect(25.0f);
}

void VFXPresets::Gauge50Percent(EvolutionVFX* vfx) {
    vfx->PlayGaugeThresholdEffect(50.0f);
}

void VFXPresets::Gauge75Percent(EvolutionVFX* vfx) {
    vfx->PlayGaugeThresholdEffect(75.0f);
}

void VFXPresets::Gauge100Percent(EvolutionVFX* vfx) {
    vfx->PlayGaugeThresholdEffect(100.0f);
}

} // namespace ArenaFighter