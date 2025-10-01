#pragma once
#include <imgui.h>
#include <vector>

namespace ArenaFighter {

class ParticleSystem {
public:
    ParticleSystem();
    ~ParticleSystem();
    
    // Emission methods
    void EmitSparkles(ImVec2 position, int count);
    void EmitLightOrbs(ImVec2 position, int count);
    void EmitCelebration(ImVec2 position);
    void EmitStarBurst(ImVec2 position, int count, ImU32 color);
    void EmitTrail(ImVec2 start, ImVec2 end, int count);
    
    // Update and render
    void Update(float deltaTime);
    void Render();
    
    // Control
    void Clear();
    void SetGravity(float gravity) { m_gravity = gravity; }
    void SetWindForce(ImVec2 wind) { m_windForce = wind; }
    
private:
    enum ParticleType {
        SPARKLE,
        LIGHT_ORB,
        STAR,
        GLOW,
        TRAIL
    };
    
    struct Particle {
        ParticleType type;
        ImVec2 position;
        ImVec2 velocity;
        ImVec2 acceleration;
        float lifetime;
        float maxLifetime;
        float size;
        float rotation;
        float rotationSpeed;
        ImU32 color;
        float alpha;
        float fadeSpeed;
        bool useGravity;
    };
    
    std::vector<Particle> m_particles;
    float m_gravity;
    ImVec2 m_windForce;
    
    // Particle creation helpers
    Particle CreateSparkle(ImVec2 position);
    Particle CreateLightOrb(ImVec2 position);
    Particle CreateStar(ImVec2 position, ImU32 color);
    Particle CreateGlow(ImVec2 position);
    
    // Update methods
    void UpdateParticle(Particle& particle, float deltaTime);
    void ApplyPhysics(Particle& particle, float deltaTime);
    
    // Render methods
    void RenderSparkle(const Particle& particle);
    void RenderLightOrb(const Particle& particle);
    void RenderStar(const Particle& particle);
    void RenderGlow(const Particle& particle);
    void RenderTrail(const Particle& particle);
    
    // Helper functions
    ImVec2 RandomDirection();
    float RandomFloat(float min, float max);
};

// Specialized unlock screen particles
class UnlockParticleSystem : public ParticleSystem {
public:
    UnlockParticleSystem();
    
    // Special effects for unlock screen
    void EmitGlowBurst(ImVec2 center, float radius);
    void EmitRevealSparkles(ImVec2 position, float height);
    void EmitCelebrationBurst(ImVec2 position);
    void EmitEnergyRings(ImVec2 center, float radius);
};

} // namespace ArenaFighter