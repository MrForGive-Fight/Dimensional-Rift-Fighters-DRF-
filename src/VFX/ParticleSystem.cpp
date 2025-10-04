#include "ParticleSystem.h"
#include <WICTextureLoader.h>
#include <algorithm>

namespace ArenaFighter {

ParticleSystem::ParticleSystem()
    : m_randomEngine(m_randomDevice()) {
}

ParticleSystem::~ParticleSystem() = default;

bool ParticleSystem::Initialize(ID3D11Device* device, ID3D11DeviceContext* context) {
    if (!device || !context) {
        return false;
    }

    m_device = device;
    m_context = context;

    // Create SpriteBatch for particle rendering
    m_spriteBatch = std::make_unique<SpriteBatch>(context);

    // Reserve particle pool
    m_particles.resize(m_config.maxParticles);

    return true;
}

bool ParticleSystem::LoadTexture(const std::wstring& filepath) {
    if (!m_device) {
        return false;
    }

    HRESULT hr = CreateWICTextureFromFile(
        m_device,
        filepath.c_str(),
        nullptr,
        m_texture.ReleaseAndGetAddressOf()
    );

    return SUCCEEDED(hr);
}

void ParticleSystem::SetEmitterConfig(const ParticleEmitterConfig& config) {
    m_config = config;

    // Resize particle pool if needed
    if (m_particles.size() != static_cast<size_t>(m_config.maxParticles)) {
        m_particles.resize(m_config.maxParticles);
    }
}

void ParticleSystem::SetPosition(const Vector3& position) {
    m_config.spawnPosition = position;
}

void ParticleSystem::Play() {
    m_isPlaying = true;
}

void ParticleSystem::Stop() {
    m_isPlaying = false;
    m_emissionAccumulator = 0.0f;
}

void ParticleSystem::Pause() {
    m_isPlaying = false;
}

void ParticleSystem::EmitBurst(int count) {
    for (int i = 0; i < count; ++i) {
        EmitParticle();
    }
}

void ParticleSystem::Update(float deltaTime) {
    // Update emission
    if (m_isPlaying && m_config.continuous) {
        m_emissionAccumulator += m_config.emissionRate * deltaTime;

        while (m_emissionAccumulator >= 1.0f) {
            EmitParticle();
            m_emissionAccumulator -= 1.0f;
        }
    }

    // Update all particles
    for (auto& particle : m_particles) {
        if (particle.IsAlive()) {
            UpdateParticle(particle, deltaTime);
        }
    }
}

void ParticleSystem::Render(const Matrix& view, const Matrix& projection) {
    if (!m_spriteBatch || !m_texture) {
        return;
    }

    // Begin sprite batch
    m_spriteBatch->Begin(SpriteSortMode_Deferred, nullptr, nullptr, nullptr, nullptr,
        nullptr, view * projection);

    // Render each active particle
    for (const auto& particle : m_particles) {
        if (!particle.IsAlive()) {
            continue;
        }

        // Calculate screen position (billboard)
        Vector2 screenPos(particle.position.x, particle.position.y);

        // Draw particle
        RECT sourceRect = { 0, 0, 64, 64 };  // Assume 64x64 texture
        m_spriteBatch->Draw(
            m_texture.Get(),
            screenPos,
            &sourceRect,
            particle.color,
            particle.rotation,
            Vector2(32.0f, 32.0f),  // Origin at center
            particle.size,
            SpriteEffects_None,
            particle.position.z  // Depth
        );
    }

    // End sprite batch
    m_spriteBatch->End();
}

int ParticleSystem::GetActiveParticleCount() const {
    return static_cast<int>(std::count_if(
        m_particles.begin(),
        m_particles.end(),
        [](const Particle& p) { return p.IsAlive(); }
    ));
}

void ParticleSystem::Clear() {
    for (auto& particle : m_particles) {
        particle.active = false;
        particle.age = particle.lifetime;
    }
}

void ParticleSystem::EmitParticle() {
    // Find an inactive particle
    auto it = std::find_if(
        m_particles.begin(),
        m_particles.end(),
        [](const Particle& p) { return !p.IsAlive(); }
    );

    if (it == m_particles.end()) {
        return;  // No available particles
    }

    // Initialize particle
    it->active = true;
    it->age = 0.0f;
    it->lifetime = RandomRange(m_config.minLifetime, m_config.maxLifetime);
    it->position = m_config.spawnPosition +
        RandomVector(m_config.spawnAreaMin, m_config.spawnAreaMax);
    it->velocity = RandomVector(m_config.initialVelocityMin, m_config.initialVelocityMax);
    it->size = RandomRange(m_config.minSize, m_config.maxSize);
    it->rotation = 0.0f;
    it->color = m_config.startColor;
}

void ParticleSystem::UpdateParticle(Particle& particle, float deltaTime) {
    // Update age
    particle.age += deltaTime;

    // Check if dead
    if (particle.age >= particle.lifetime) {
        particle.active = false;
        return;
    }

    // Update physics
    particle.velocity += m_config.acceleration * deltaTime;
    particle.position += particle.velocity * deltaTime;

    // Update rotation
    particle.rotation += RandomRange(m_config.minRotationSpeed, m_config.maxRotationSpeed) * deltaTime;

    // Update color (lerp between start and end color)
    float t = particle.age / particle.lifetime;
    particle.color = Vector4::Lerp(m_config.startColor, m_config.endColor, t);

    // Update size
    particle.size *= m_config.sizeOverLifetime;
}

float ParticleSystem::RandomRange(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(m_randomEngine);
}

Vector3 ParticleSystem::RandomVector(const Vector3& min, const Vector3& max) {
    return Vector3(
        RandomRange(min.x, max.x),
        RandomRange(min.y, max.y),
        RandomRange(min.z, max.z)
    );
}

// Preset particle effects
ParticleEmitterConfig ParticleSystem::CreateHitSpark() {
    ParticleEmitterConfig config;
    config.maxParticles = 50;
    config.emissionRate = 0.0f;  // Burst only
    config.continuous = false;
    config.minLifetime = 0.1f;
    config.maxLifetime = 0.3f;
    config.spawnAreaMin = Vector3(-0.1f, -0.1f, -0.1f);
    config.spawnAreaMax = Vector3(0.1f, 0.1f, 0.1f);
    config.initialVelocityMin = Vector3(-5.0f, -5.0f, -5.0f);
    config.initialVelocityMax = Vector3(5.0f, 5.0f, 5.0f);
    config.acceleration = Vector3::Zero;
    config.minSize = 0.5f;
    config.maxSize = 1.0f;
    config.startColor = Vector4(1.0f, 0.8f, 0.3f, 1.0f);  // Orange spark
    config.endColor = Vector4(1.0f, 0.3f, 0.0f, 0.0f);    // Fade to red
    return config;
}

ParticleEmitterConfig ParticleSystem::CreateFireEffect() {
    ParticleEmitterConfig config;
    config.maxParticles = 200;
    config.emissionRate = 50.0f;
    config.continuous = true;
    config.minLifetime = 0.5f;
    config.maxLifetime = 1.5f;
    config.spawnAreaMin = Vector3(-0.5f, 0.0f, -0.5f);
    config.spawnAreaMax = Vector3(0.5f, 0.0f, 0.5f);
    config.initialVelocityMin = Vector3(-1.0f, 2.0f, -1.0f);
    config.initialVelocityMax = Vector3(1.0f, 5.0f, 1.0f);
    config.acceleration = Vector3(0.0f, 1.0f, 0.0f);  // Rise up
    config.minSize = 0.5f;
    config.maxSize = 2.0f;
    config.sizeOverLifetime = 0.5f;  // Shrink
    config.startColor = Vector4(1.0f, 0.5f, 0.0f, 1.0f);  // Orange
    config.endColor = Vector4(0.8f, 0.0f, 0.0f, 0.0f);    // Red fade
    return config;
}

ParticleEmitterConfig ParticleSystem::CreateSmokeEffect() {
    ParticleEmitterConfig config;
    config.maxParticles = 100;
    config.emissionRate = 30.0f;
    config.continuous = true;
    config.minLifetime = 1.0f;
    config.maxLifetime = 3.0f;
    config.spawnAreaMin = Vector3(-0.3f, 0.0f, -0.3f);
    config.spawnAreaMax = Vector3(0.3f, 0.0f, 0.3f);
    config.initialVelocityMin = Vector3(-0.5f, 1.0f, -0.5f);
    config.initialVelocityMax = Vector3(0.5f, 3.0f, 0.5f);
    config.acceleration = Vector3(0.0f, 0.5f, 0.0f);  // Slow rise
    config.minSize = 1.0f;
    config.maxSize = 3.0f;
    config.sizeOverLifetime = 1.5f;  // Grow
    config.startColor = Vector4(0.5f, 0.5f, 0.5f, 0.8f);  // Gray
    config.endColor = Vector4(0.3f, 0.3f, 0.3f, 0.0f);    // Fade out
    config.minRotationSpeed = -1.0f;
    config.maxRotationSpeed = 1.0f;
    return config;
}

ParticleEmitterConfig ParticleSystem::CreateAuraEffect() {
    ParticleEmitterConfig config;
    config.maxParticles = 500;
    config.emissionRate = 100.0f;
    config.continuous = true;
    config.minLifetime = 1.0f;
    config.maxLifetime = 2.0f;
    config.spawnAreaMin = Vector3(-1.0f, 0.0f, -1.0f);
    config.spawnAreaMax = Vector3(1.0f, 2.0f, 1.0f);
    config.initialVelocityMin = Vector3(-0.5f, 0.5f, -0.5f);
    config.initialVelocityMax = Vector3(0.5f, 1.5f, 0.5f);
    config.acceleration = Vector3(0.0f, 0.2f, 0.0f);
    config.minSize = 0.3f;
    config.maxSize = 0.8f;
    config.startColor = Vector4(0.3f, 0.5f, 1.0f, 0.8f);  // Blue glow
    config.endColor = Vector4(0.0f, 0.3f, 1.0f, 0.0f);    // Fade
    return config;
}

ParticleEmitterConfig ParticleSystem::CreateExplosion() {
    ParticleEmitterConfig config;
    config.maxParticles = 200;
    config.emissionRate = 0.0f;  // Burst only
    config.continuous = false;
    config.minLifetime = 0.5f;
    config.maxLifetime = 2.0f;
    config.spawnAreaMin = Vector3::Zero;
    config.spawnAreaMax = Vector3::Zero;
    config.initialVelocityMin = Vector3(-10.0f, -10.0f, -10.0f);
    config.initialVelocityMax = Vector3(10.0f, 10.0f, 10.0f);
    config.acceleration = Vector3(0.0f, -5.0f, 0.0f);  // Gravity
    config.minSize = 0.5f;
    config.maxSize = 2.0f;
    config.sizeOverLifetime = 0.3f;  // Shrink fast
    config.startColor = Vector4(1.0f, 1.0f, 0.5f, 1.0f);  // Bright yellow
    config.endColor = Vector4(1.0f, 0.0f, 0.0f, 0.0f);    // Red fade
    return config;
}

ParticleEmitterConfig ParticleSystem::CreateHealEffect() {
    ParticleEmitterConfig config;
    config.maxParticles = 100;
    config.emissionRate = 50.0f;
    config.continuous = true;
    config.minLifetime = 1.0f;
    config.maxLifetime = 2.0f;
    config.spawnAreaMin = Vector3(-0.5f, 0.0f, -0.5f);
    config.spawnAreaMax = Vector3(0.5f, 0.5f, 0.5f);
    config.initialVelocityMin = Vector3(-1.0f, 1.0f, -1.0f);
    config.initialVelocityMax = Vector3(1.0f, 3.0f, 1.0f);
    config.acceleration = Vector3(0.0f, 2.0f, 0.0f);  // Rise up
    config.minSize = 0.3f;
    config.maxSize = 1.0f;
    config.startColor = Vector4(0.3f, 1.0f, 0.3f, 1.0f);  // Green
    config.endColor = Vector4(0.0f, 1.0f, 0.5f, 0.0f);    // Fade
    return config;
}

} // namespace ArenaFighter
