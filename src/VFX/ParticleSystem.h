#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <SpriteBatch.h>
#include <vector>
#include <memory>
#include <random>

namespace ArenaFighter {

using namespace DirectX;
using namespace DirectX::SimpleMath;

/**
 * Particle - Individual particle data
 */
struct Particle {
    Vector3 position;
    Vector3 velocity;
    Vector4 color;          // RGBA
    float lifetime = 1.0f;  // Total lifetime in seconds
    float age = 0.0f;       // Current age in seconds
    float size = 1.0f;      // Particle size
    float rotation = 0.0f;  // Rotation in radians
    bool active = false;

    bool IsAlive() const { return active && age < lifetime; }
};

/**
 * ParticleEmitterConfig - Configuration for particle emission
 */
struct ParticleEmitterConfig {
    // Emission
    int maxParticles = 1000;
    float emissionRate = 100.0f;    // Particles per second
    bool continuous = true;          // Continuous or burst emission

    // Lifetime
    float minLifetime = 0.5f;
    float maxLifetime = 2.0f;

    // Position
    Vector3 spawnPosition = Vector3::Zero;
    Vector3 spawnAreaMin = Vector3(-0.5f, -0.5f, -0.5f);
    Vector3 spawnAreaMax = Vector3(0.5f, 0.5f, 0.5f);

    // Velocity
    Vector3 initialVelocityMin = Vector3(-1.0f, 1.0f, -1.0f);
    Vector3 initialVelocityMax = Vector3(1.0f, 5.0f, 1.0f);
    Vector3 acceleration = Vector3(0.0f, -9.8f, 0.0f);  // Gravity

    // Size
    float minSize = 0.1f;
    float maxSize = 1.0f;
    float sizeOverLifetime = 1.0f;  // Size multiplier at end of life

    // Color
    Vector4 startColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    Vector4 endColor = Vector4(1.0f, 1.0f, 1.0f, 0.0f);  // Fade out

    // Rotation
    float minRotationSpeed = 0.0f;
    float maxRotationSpeed = 0.0f;
};

/**
 * ParticleSystem - GPU-accelerated particle system using DirectXTK
 */
class ParticleSystem {
public:
    ParticleSystem();
    ~ParticleSystem();

    /**
     * Initialize particle system with DirectX device
     */
    bool Initialize(ID3D11Device* device, ID3D11DeviceContext* context);

    /**
     * Load particle texture
     */
    bool LoadTexture(const std::wstring& filepath);

    /**
     * Configure the emitter
     */
    void SetEmitterConfig(const ParticleEmitterConfig& config);

    /**
     * Set emitter world position
     */
    void SetPosition(const Vector3& position);

    /**
     * Start/stop emission
     */
    void Play();
    void Stop();
    void Pause();
    bool IsPlaying() const { return m_isPlaying; }

    /**
     * Emit a burst of particles
     */
    void EmitBurst(int count);

    /**
     * Update particles
     */
    void Update(float deltaTime);

    /**
     * Render particles
     */
    void Render(const Matrix& view, const Matrix& projection);

    /**
     * Get active particle count
     */
    int GetActiveParticleCount() const;

    /**
     * Clear all particles
     */
    void Clear();

    /**
     * Create preset particle effects
     */
    static ParticleEmitterConfig CreateHitSpark();
    static ParticleEmitterConfig CreateFireEffect();
    static ParticleEmitterConfig CreateSmokeEffect();
    static ParticleEmitterConfig CreateAuraEffect();
    static ParticleEmitterConfig CreateExplosion();
    static ParticleEmitterConfig CreateHealEffect();

private:
    // Emit a single particle
    void EmitParticle();

    // Update a single particle
    void UpdateParticle(Particle& particle, float deltaTime);

    // Get random float in range
    float RandomRange(float min, float max);

    // Get random vector in range
    Vector3 RandomVector(const Vector3& min, const Vector3& max);

private:
    // DirectX resources
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;
    std::unique_ptr<SpriteBatch> m_spriteBatch;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

    // Particle data
    std::vector<Particle> m_particles;
    ParticleEmitterConfig m_config;

    // State
    bool m_isPlaying = false;
    float m_emissionAccumulator = 0.0f;

    // Random number generation
    std::random_device m_randomDevice;
    std::mt19937 m_randomEngine;
};

} // namespace ArenaFighter
