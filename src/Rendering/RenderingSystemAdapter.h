#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <memory>
#include "../Characters/CharacterBase.h"
#include "../Combat/SpecialMoveSystem.h"

namespace ArenaFighter {

using Microsoft::WRL::ComPtr;
using namespace DirectX;

/**
 * @brief Adapter to make the provided RenderingSystem work with DFR's characters
 * 
 * Maps between the incorrect Character API and DFR's proper implementation
 */
class RenderingSystemAdapter {
private:
    ComPtr<ID3D11Device> m_device;
    ComPtr<ID3D11DeviceContext> m_deviceContext;
    
public:
    /**
     * @brief Character wrapper that provides the expected API for RenderingSystem
     */
    class CharacterView {
    private:
        const CharacterBase* m_dfrCharacter;
        const SpecialMoveSystem* m_specialMoveSystem;
        XMFLOAT3 m_rotation;
        
    public:
        CharacterView(const CharacterBase* character, const SpecialMoveSystem* sms = nullptr)
            : m_dfrCharacter(character)
            , m_specialMoveSystem(sms)
            , m_rotation(0, 0, 0) {}
        
        // Position and rotation
        XMFLOAT3 GetPosition() const {
            // DFR doesn't track position in CharacterBase, so return arena position
            return XMFLOAT3(0, 0, 0); // Would be tracked by game world
        }
        
        XMFLOAT3 GetRotation() const {
            return m_rotation;
        }
        
        void SetRotation(const XMFLOAT3& rot) {
            m_rotation = rot;
        }
        
        // State mapping
        enum CharacterState {
            Idle,
            Walking,
            Running,
            Attacking,
            UsingSkill,
            Blocking,
            Hit,
            Down,
            Ultimate
        };
        
        CharacterState GetState() const {
            switch (m_dfrCharacter->GetCurrentState()) {
                case ArenaFighter::CharacterState::ExecutingSpecial:
                    return UsingSkill;
                case ArenaFighter::CharacterState::Blocking:
                    return Blocking;
                case ArenaFighter::CharacterState::HitStun:
                    return Hit;
                case ArenaFighter::CharacterState::KnockedDown:
                    return Down;
                default:
                    return Idle;
            }
        }
        
        // Stance mapping
        enum CharacterStance {
            None,
            Light,
            Dark,
            Attack,
            Defense
        };
        
        CharacterStance GetStance() const {
            if (!m_dfrCharacter->HasStanceSystem()) {
                return None;
            }
            
            int stance = m_dfrCharacter->GetCurrentStance();
            if (stance == 0) return Light;
            if (stance == 1) return Dark;
            return None;
        }
        
        // Block state
        bool IsBlockActive() const {
            if (!m_specialMoveSystem) {
                return m_dfrCharacter->IsBlocking();
            }
            
            // Check if block is fully activated (1 second hold)
            auto blockState = m_specialMoveSystem->GetBlockState(
                const_cast<CharacterBase*>(m_dfrCharacter));
            return blockState.isActive;
        }
        
        // Ultimate state
        bool IsInUltimate() const {
            // Check if character has an active transformation
            return m_dfrCharacter->HasTransformationSystem() && 
                   m_dfrCharacter->GetCurrentState() == ArenaFighter::CharacterState::Transformed;
        }
    };
    
    /**
     * @brief Rendering system that uses DFR characters
     */
    class DFRRenderingSystem {
    private:
        ComPtr<ID3D11Device> m_device;
        ComPtr<ID3D11DeviceContext> m_deviceContext;
        
        // Shaders
        ComPtr<ID3D11VertexShader> m_vertexShader;
        ComPtr<ID3D11PixelShader> m_pixelShader;
        ComPtr<ID3D11InputLayout> m_inputLayout;
        
        // Buffers
        ComPtr<ID3D11Buffer> m_constantBuffer;
        ComPtr<ID3D11Buffer> m_arenaVertexBuffer;
        ComPtr<ID3D11Buffer> m_arenaIndexBuffer;
        ComPtr<ID3D11Buffer> m_characterVertexBuffer;
        ComPtr<ID3D11Buffer> m_characterIndexBuffer;
        ComPtr<ID3D11Buffer> m_particleVertexBuffer;
        
        // States
        ComPtr<ID3D11RasterizerState> m_solidRasterizer;
        ComPtr<ID3D11RasterizerState> m_wireframeRasterizer;
        ComPtr<ID3D11BlendState> m_alphaBlendState;
        ComPtr<ID3D11DepthStencilState> m_depthStencilState;
        
        // Effect tracking
        struct ParticleEffect {
            XMFLOAT3 position;
            XMFLOAT3 velocity;
            XMFLOAT4 color;
            float life;
            float size;
            std::string type;
        };
        
        struct StanceEffect {
            XMFLOAT3 position;
            float timer;
            bool isLightToDark;
            std::vector<ParticleEffect> particles;
        };
        
        std::vector<ParticleEffect> m_particles;
        std::vector<StanceEffect> m_stanceEffects;
        
        // Geometry data
        UINT m_arenaIndexCount;
        UINT m_characterIndexCount;
        
        // Camera and lighting
        XMMATRIX m_viewMatrix;
        XMMATRIX m_projectionMatrix;
        XMFLOAT4 m_lightDirection;
        XMFLOAT4 m_lightColor;
        XMFLOAT4 m_ambientColor;
        
    public:
        DFRRenderingSystem(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
            : m_device(device)
            , m_deviceContext(context)
            , m_arenaIndexCount(0)
            , m_characterIndexCount(0)
            , m_lightDirection(0.577f, -0.577f, 0.577f, 0.0f)
            , m_lightColor(1.0f, 1.0f, 1.0f, 1.0f)
            , m_ambientColor(0.2f, 0.2f, 0.3f, 1.0f) {}
        
        bool Initialize(int screenWidth, int screenHeight);
        
        void UpdateCamera(const XMFLOAT3& position, const XMFLOAT3& target);
        void RenderArena();
        void RenderCharacter(const CharacterBase* character, const XMFLOAT3& position);
        void RenderCharacterWithView(const CharacterView& view, const XMFLOAT3& position);
        
        // Effect creation aligned with DFR's skill system
        void CreateSpecialMoveEffect(const CharacterBase* character, 
                                   InputDirection direction,
                                   const XMFLOAT3& position);
        
        void CreateGearSkillEffect(const CharacterBase* character,
                                 int skillIndex,
                                 const XMFLOAT3& position);
        
        void CreateStanceSwitchEffect(const CharacterBase* character,
                                    int oldStance, int newStance,
                                    const XMFLOAT3& position);
        
        void CreateHitEffect(const XMFLOAT3& position, float damage);
        void CreateBlockEffect(const XMFLOAT3& position);
        
        void UpdateParticles(float deltaTime);
        void RenderParticles();
        
    private:
        bool CreateShaders();
        bool CreateBuffers();
        bool CreateStates();
        bool CreateArenaGeometry();
        bool CreateCharacterGeometry();
        void UpdateConstantBuffer(const XMMATRIX& world, const XMFLOAT4& tint);
        
        // Helper to get character color based on state
        XMFLOAT4 GetCharacterTintColor(const CharacterBase* character) const;
    };
};

// Vertex structures matching the original
struct Vertex {
    XMFLOAT3 position;
    XMFLOAT3 normal;
    XMFLOAT2 texCoord;
    XMFLOAT4 color;
};

struct ConstantBuffer {
    XMMATRIX world;
    XMMATRIX view;
    XMMATRIX projection;
    XMFLOAT4 lightDir;
    XMFLOAT4 lightColor;
    XMFLOAT4 ambientColor;
    XMFLOAT4 tintColor;
};

} // namespace ArenaFighter