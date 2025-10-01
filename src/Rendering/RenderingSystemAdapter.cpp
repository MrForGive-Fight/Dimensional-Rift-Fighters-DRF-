#include "RenderingSystemAdapter.h"
#include <algorithm>
#include <cmath>
#include <random>

namespace ArenaFighter {

bool RenderingSystemAdapter::DFRRenderingSystem::Initialize(int screenWidth, int screenHeight) {
    if (!CreateShaders()) return false;
    if (!CreateBuffers()) return false;
    if (!CreateStates()) return false;
    if (!CreateArenaGeometry()) return false;
    if (!CreateCharacterGeometry()) return false;
    
    m_projectionMatrix = XMMatrixPerspectiveFovLH(
        XM_PIDIV4,
        static_cast<float>(screenWidth) / screenHeight,
        0.1f,
        1000.0f
    );
    
    return true;
}

bool RenderingSystemAdapter::DFRRenderingSystem::CreateShaders() {
    // Vertex shader source
    const char* vsSource = R"(
        cbuffer ConstantBuffer : register(b0) {
            matrix World;
            matrix View;
            matrix Projection;
            float4 LightDir;
            float4 LightColor;
            float4 AmbientColor;
            float4 TintColor;
        }
        
        struct VertexInput {
            float3 position : POSITION;
            float3 normal : NORMAL;
            float2 texCoord : TEXCOORD0;
            float4 color : COLOR;
        };
        
        struct PixelInput {
            float4 position : SV_POSITION;
            float3 normal : NORMAL;
            float2 texCoord : TEXCOORD0;
            float4 color : COLOR;
            float3 worldPos : TEXCOORD1;
        };
        
        PixelInput main(VertexInput input) {
            PixelInput output;
            
            float4 worldPos = mul(float4(input.position, 1.0f), World);
            output.worldPos = worldPos.xyz;
            output.position = mul(worldPos, View);
            output.position = mul(output.position, Projection);
            
            output.normal = normalize(mul(input.normal, (float3x3)World));
            output.texCoord = input.texCoord;
            output.color = input.color;
            
            return output;
        }
    )";
    
    const char* psSource = R"(
        cbuffer ConstantBuffer : register(b0) {
            matrix World;
            matrix View;
            matrix Projection;
            float4 LightDir;
            float4 LightColor;
            float4 AmbientColor;
            float4 TintColor;
        }
        
        struct PixelInput {
            float4 position : SV_POSITION;
            float3 normal : NORMAL;
            float2 texCoord : TEXCOORD0;
            float4 color : COLOR;
            float3 worldPos : TEXCOORD1;
        };
        
        float4 main(PixelInput input) : SV_TARGET {
            float3 normal = normalize(input.normal);
            float ndotl = max(0, dot(normal, -LightDir.xyz));
            
            float3 diffuse = LightColor.rgb * ndotl;
            float3 ambient = AmbientColor.rgb;
            
            float4 finalColor = input.color * TintColor;
            finalColor.rgb = finalColor.rgb * (ambient + diffuse);
            
            return finalColor;
        }
    )";
    
    // In production, compile these properly
    // For now, assume success
    return true;
}

bool RenderingSystemAdapter::DFRRenderingSystem::CreateBuffers() {
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.ByteWidth = sizeof(ConstantBuffer);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    
    HRESULT hr = m_device->CreateBuffer(&cbDesc, nullptr, &m_constantBuffer);
    return SUCCEEDED(hr);
}

bool RenderingSystemAdapter::DFRRenderingSystem::CreateStates() {
    // Solid rasterizer
    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.FrontCounterClockwise = FALSE;
    rasterDesc.DepthClipEnable = TRUE;
    
    HRESULT hr = m_device->CreateRasterizerState(&rasterDesc, &m_solidRasterizer);
    if (FAILED(hr)) return false;
    
    // Wireframe rasterizer
    rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
    hr = m_device->CreateRasterizerState(&rasterDesc, &m_wireframeRasterizer);
    if (FAILED(hr)) return false;
    
    // Alpha blend state
    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    
    hr = m_device->CreateBlendState(&blendDesc, &m_alphaBlendState);
    if (FAILED(hr)) return false;
    
    // Depth stencil state
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    
    hr = m_device->CreateDepthStencilState(&dsDesc, &m_depthStencilState);
    return SUCCEEDED(hr);
}

bool RenderingSystemAdapter::DFRRenderingSystem::CreateArenaGeometry() {
    std::vector<Vertex> vertices;
    std::vector<UINT> indices;
    
    float arenaSize = 20.0f;
    XMFLOAT4 floorColor(0.3f, 0.3f, 0.35f, 1.0f);
    
    // Main floor
    vertices.push_back({XMFLOAT3(-arenaSize, 0, -arenaSize), XMFLOAT3(0, 1, 0), XMFLOAT2(0, 0), floorColor});
    vertices.push_back({XMFLOAT3(arenaSize, 0, -arenaSize), XMFLOAT3(0, 1, 0), XMFLOAT2(1, 0), floorColor});
    vertices.push_back({XMFLOAT3(arenaSize, 0, arenaSize), XMFLOAT3(0, 1, 0), XMFLOAT2(1, 1), floorColor});
    vertices.push_back({XMFLOAT3(-arenaSize, 0, arenaSize), XMFLOAT3(0, 1, 0), XMFLOAT2(0, 1), floorColor});
    
    indices = {0, 1, 2, 0, 2, 3};
    
    // Grid lines
    XMFLOAT4 lineColor(0.4f, 0.4f, 0.45f, 1.0f);
    int vertexOffset = vertices.size();
    
    for (int i = -10; i <= 10; i++) {
        float pos = i * 2.0f;
        
        // Horizontal lines
        vertices.push_back({XMFLOAT3(-arenaSize, 0.01f, pos), XMFLOAT3(0, 1, 0), XMFLOAT2(0, 0), lineColor});
        vertices.push_back({XMFLOAT3(arenaSize, 0.01f, pos), XMFLOAT3(0, 1, 0), XMFLOAT2(1, 0), lineColor});
        
        // Vertical lines
        vertices.push_back({XMFLOAT3(pos, 0.01f, -arenaSize), XMFLOAT3(0, 1, 0), XMFLOAT2(0, 0), lineColor});
        vertices.push_back({XMFLOAT3(pos, 0.01f, arenaSize), XMFLOAT3(0, 1, 0), XMFLOAT2(1, 0), lineColor});
    }
    
    // Add line indices
    for (int i = 0; i < 21; i++) {
        int base = vertexOffset + i * 4;
        // Horizontal line
        indices.push_back(base);
        indices.push_back(base + 1);
        indices.push_back(base + 1);
        indices.push_back(base);
        indices.push_back(base);
        indices.push_back(base + 1);
        
        // Vertical line
        indices.push_back(base + 2);
        indices.push_back(base + 3);
        indices.push_back(base + 3);
        indices.push_back(base + 2);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
    }
    
    m_arenaIndexCount = indices.size();
    
    // Create vertex buffer
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.ByteWidth = sizeof(Vertex) * vertices.size();
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    
    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = vertices.data();
    
    HRESULT hr = m_device->CreateBuffer(&vbDesc, &vbData, &m_arenaVertexBuffer);
    if (FAILED(hr)) return false;
    
    // Create index buffer
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.ByteWidth = sizeof(UINT) * indices.size();
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    
    D3D11_SUBRESOURCE_DATA ibData = {};
    ibData.pSysMem = indices.data();
    
    hr = m_device->CreateBuffer(&ibDesc, &ibData, &m_arenaIndexBuffer);
    return SUCCEEDED(hr);
}

bool RenderingSystemAdapter::DFRRenderingSystem::CreateCharacterGeometry() {
    std::vector<Vertex> vertices;
    std::vector<UINT> indices;
    
    // Create a simple capsule for character
    int segments = 16;
    float radius = 0.5f;
    float height = 2.0f;
    XMFLOAT4 bodyColor(0.6f, 0.3f, 0.2f, 1.0f);
    
    // Cylinder body
    for (int i = 0; i <= segments; i++) {
        float angle = static_cast<float>(i) / segments * XM_2PI;
        float x = cosf(angle) * radius;
        float z = sinf(angle) * radius;
        
        vertices.push_back({
            XMFLOAT3(x, 0, z),
            XMFLOAT3(x, 0, z),
            XMFLOAT2(static_cast<float>(i) / segments, 0),
            bodyColor
        });
        
        vertices.push_back({
            XMFLOAT3(x, height, z),
            XMFLOAT3(x, 0, z),
            XMFLOAT2(static_cast<float>(i) / segments, 1),
            bodyColor
        });
    }
    
    // Create indices for cylinder
    for (int i = 0; i < segments; i++) {
        int base = i * 2;
        indices.push_back(base);
        indices.push_back(base + 2);
        indices.push_back(base + 1);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
    }
    
    m_characterIndexCount = indices.size();
    
    // Create vertex buffer
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.ByteWidth = sizeof(Vertex) * vertices.size();
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    
    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = vertices.data();
    
    HRESULT hr = m_device->CreateBuffer(&vbDesc, &vbData, &m_characterVertexBuffer);
    if (FAILED(hr)) return false;
    
    // Create index buffer
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.ByteWidth = sizeof(UINT) * indices.size();
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    
    D3D11_SUBRESOURCE_DATA ibData = {};
    ibData.pSysMem = indices.data();
    
    hr = m_device->CreateBuffer(&ibDesc, &ibData, &m_characterIndexBuffer);
    return SUCCEEDED(hr);
}

void RenderingSystemAdapter::DFRRenderingSystem::UpdateCamera(const XMFLOAT3& position, 
                                                              const XMFLOAT3& target) {
    XMVECTOR eyePos = XMLoadFloat3(&position);
    XMVECTOR targetPos = XMLoadFloat3(&target);
    XMVECTOR upDir = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    
    m_viewMatrix = XMMatrixLookAtLH(eyePos, targetPos, upDir);
}

void RenderingSystemAdapter::DFRRenderingSystem::RenderArena() {
    if (!m_arenaVertexBuffer || !m_arenaIndexBuffer) return;
    
    XMMATRIX worldMatrix = XMMatrixIdentity();
    UpdateConstantBuffer(worldMatrix, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
    
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_deviceContext->IASetVertexBuffers(0, 1, m_arenaVertexBuffer.GetAddressOf(), &stride, &offset);
    m_deviceContext->IASetIndexBuffer(m_arenaIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    m_deviceContext->DrawIndexed(m_arenaIndexCount, 0, 0);
}

void RenderingSystemAdapter::DFRRenderingSystem::RenderCharacter(const CharacterBase* character,
                                                                  const XMFLOAT3& position) {
    if (!character || !m_characterVertexBuffer || !m_characterIndexBuffer) return;
    
    // Default rotation (facing forward)
    XMMATRIX worldMatrix = XMMatrixTranslation(position.x, position.y, position.z);
    
    XMFLOAT4 tintColor = GetCharacterTintColor(character);
    UpdateConstantBuffer(worldMatrix, tintColor);
    
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_deviceContext->IASetVertexBuffers(0, 1, m_characterVertexBuffer.GetAddressOf(), &stride, &offset);
    m_deviceContext->IASetIndexBuffer(m_characterIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    m_deviceContext->DrawIndexed(m_characterIndexCount, 0, 0);
}

void RenderingSystemAdapter::DFRRenderingSystem::RenderCharacterWithView(const CharacterView& view,
                                                                          const XMFLOAT3& position) {
    if (!m_characterVertexBuffer || !m_characterIndexBuffer) return;
    
    const XMFLOAT3& rot = view.GetRotation();
    XMMATRIX worldMatrix = XMMatrixRotationY(rot.y) *
                          XMMatrixTranslation(position.x, position.y, position.z);
    
    // Get tint color based on view state
    XMFLOAT4 tintColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    if (view.IsBlockActive()) {
        tintColor = XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f); // Blue for blocking
    }
    else if (view.GetState() == CharacterView::UsingSkill) {
        auto stance = view.GetStance();
        if (stance == CharacterView::Light) {
            tintColor = XMFLOAT4(0.5f, 0.7f, 1.0f, 1.0f); // Light blue
        } else if (stance == CharacterView::Dark) {
            tintColor = XMFLOAT4(1.0f, 0.3f, 0.3f, 1.0f); // Dark red
        } else {
            tintColor = XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f); // Red for attacking
        }
    }
    else if (view.IsInUltimate()) {
        tintColor = XMFLOAT4(1.0f, 0.8f, 0.0f, 1.0f); // Gold for ultimate
    }
    
    UpdateConstantBuffer(worldMatrix, tintColor);
    
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_deviceContext->IASetVertexBuffers(0, 1, m_characterVertexBuffer.GetAddressOf(), &stride, &offset);
    m_deviceContext->IASetIndexBuffer(m_characterIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    m_deviceContext->DrawIndexed(m_characterIndexCount, 0, 0);
}

void RenderingSystemAdapter::DFRRenderingSystem::UpdateConstantBuffer(const XMMATRIX& world,
                                                                       const XMFLOAT4& tint) {
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = m_deviceContext->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (SUCCEEDED(hr)) {
        ConstantBuffer* cb = static_cast<ConstantBuffer*>(mappedResource.pData);
        cb->world = XMMatrixTranspose(world);
        cb->view = XMMatrixTranspose(m_viewMatrix);
        cb->projection = XMMatrixTranspose(m_projectionMatrix);
        cb->lightDir = m_lightDirection;
        cb->lightColor = m_lightColor;
        cb->ambientColor = m_ambientColor;
        cb->tintColor = tint;
        m_deviceContext->Unmap(m_constantBuffer.Get(), 0);
    }
    
    m_deviceContext->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    m_deviceContext->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
}

XMFLOAT4 RenderingSystemAdapter::DFRRenderingSystem::GetCharacterTintColor(
    const CharacterBase* character) const {
    
    XMFLOAT4 tintColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    // Color based on state
    switch (character->GetCurrentState()) {
        case CharacterState::Blocking:
            tintColor = XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f); // Blue
            break;
            
        case CharacterState::ExecutingSpecial:
            // Color based on category
            switch (character->GetCategory()) {
                case CharacterCategory::System:
                    tintColor = XMFLOAT4(0.8f, 0.8f, 1.0f, 1.0f); // Light purple
                    break;
                case CharacterCategory::GodsHeroes:
                    tintColor = XMFLOAT4(1.0f, 0.9f, 0.5f, 1.0f); // Golden
                    break;
                case CharacterCategory::Murim:
                    if (character->HasStanceSystem()) {
                        int stance = character->GetCurrentStance();
                        tintColor = (stance == 0) ? 
                            XMFLOAT4(0.5f, 0.7f, 1.0f, 1.0f) :  // Light blue
                            XMFLOAT4(1.0f, 0.3f, 0.3f, 1.0f);   // Dark red
                    } else {
                        tintColor = XMFLOAT4(0.9f, 0.7f, 0.5f, 1.0f); // Orange
                    }
                    break;
                case CharacterCategory::Cultivation:
                    tintColor = XMFLOAT4(0.6f, 1.0f, 0.6f, 1.0f); // Jade green
                    break;
                case CharacterCategory::Animal:
                    tintColor = XMFLOAT4(0.8f, 0.6f, 0.4f, 1.0f); // Brown
                    break;
                case CharacterCategory::Monsters:
                    tintColor = XMFLOAT4(0.7f, 0.3f, 0.7f, 1.0f); // Purple
                    break;
                case CharacterCategory::Chaos:
                    tintColor = XMFLOAT4(0.9f, 0.3f, 0.5f, 1.0f); // Crimson
                    break;
            }
            break;
            
        case CharacterState::Transformed:
            tintColor = XMFLOAT4(1.0f, 0.8f, 0.0f, 1.0f); // Gold
            break;
            
        case CharacterState::HitStun:
            tintColor = XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f); // Red tint
            break;
            
        case CharacterState::KnockedDown:
            tintColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f); // Gray
            break;
    }
    
    return tintColor;
}

void RenderingSystemAdapter::DFRRenderingSystem::CreateSpecialMoveEffect(
    const CharacterBase* character, 
    InputDirection direction,
    const XMFLOAT3& position) {
    
    if (!character) return;
    
    const SpecialMove* move = character->GetSpecialMove(direction);
    if (!move) return;
    
    // Color based on element type
    XMFLOAT4 effectColor(1.0f, 0.5f, 0.0f, 1.0f); // Default orange
    
    switch (move->element) {
        case ElementType::Fire:
            effectColor = XMFLOAT4(1.0f, 0.3f, 0.0f, 1.0f);
            break;
        case ElementType::Water:
            effectColor = XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f);
            break;
        case ElementType::Earth:
            effectColor = XMFLOAT4(0.6f, 0.4f, 0.2f, 1.0f);
            break;
        case ElementType::Wind:
            effectColor = XMFLOAT4(0.6f, 1.0f, 0.6f, 1.0f);
            break;
        case ElementType::Light:
            effectColor = XMFLOAT4(1.0f, 1.0f, 0.8f, 1.0f);
            break;
        case ElementType::Dark:
            effectColor = XMFLOAT4(0.3f, 0.0f, 0.5f, 1.0f);
            break;
    }
    
    // Create particles
    int particleCount = 20;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 1.0);
    
    for (int i = 0; i < particleCount; i++) {
        ParticleEffect p;
        p.position = position;
        
        // Direction-based velocity
        float spread = 0.5f;
        switch (direction) {
            case InputDirection::Up:
                p.velocity = XMFLOAT3(
                    (dis(gen) - 0.5f) * spread,
                    3.0f + dis(gen) * 2.0f,
                    (dis(gen) - 0.5f) * spread
                );
                break;
            case InputDirection::Down:
                p.velocity = XMFLOAT3(
                    (dis(gen) - 0.5f) * spread * 3.0f,
                    -1.0f,
                    (dis(gen) - 0.5f) * spread * 3.0f
                );
                break;
            case InputDirection::Left:
                p.velocity = XMFLOAT3(
                    -3.0f - dis(gen) * 2.0f,
                    dis(gen) * 2.0f,
                    (dis(gen) - 0.5f) * spread
                );
                break;
            case InputDirection::Right:
                p.velocity = XMFLOAT3(
                    3.0f + dis(gen) * 2.0f,
                    dis(gen) * 2.0f,
                    (dis(gen) - 0.5f) * spread
                );
                break;
        }
        
        p.color = effectColor;
        p.life = 1.0f;
        p.size = 0.1f + dis(gen) * 0.1f;
        p.type = "SpecialMove";
        
        m_particles.push_back(p);
    }
}

void RenderingSystemAdapter::DFRRenderingSystem::CreateGearSkillEffect(
    const CharacterBase* character,
    int skillIndex,
    const XMFLOAT3& position) {
    
    if (!character || skillIndex < 0 || skillIndex >= 8) return;
    
    const auto& skill = character->GetGearSkills()[skillIndex];
    
    // Gear skills are more powerful, so more particles
    int particleCount = 30;
    XMFLOAT4 effectColor(0.7f, 0.3f, 1.0f, 1.0f); // Purple for gear skills
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 1.0);
    
    for (int i = 0; i < particleCount; i++) {
        ParticleEffect p;
        p.position = position;
        
        // Spiral pattern for gear skills
        float angle = static_cast<float>(i) / particleCount * XM_2PI * 2.0f;
        float speed = 2.0f + dis(gen) * 3.0f;
        
        p.velocity = XMFLOAT3(
            speed * cosf(angle),
            dis(gen) * 4.0f,
            speed * sinf(angle)
        );
        
        p.color = effectColor;
        p.life = 1.5f; // Longer lasting
        p.size = 0.15f + dis(gen) * 0.15f;
        p.type = "GearSkill";
        
        m_particles.push_back(p);
    }
}

void RenderingSystemAdapter::DFRRenderingSystem::CreateStanceSwitchEffect(
    const CharacterBase* character,
    int oldStance, int newStance,
    const XMFLOAT3& position) {
    
    StanceEffect effect;
    effect.position = position;
    effect.timer = 1.0f;
    effect.isLightToDark = (oldStance == 0 && newStance == 1);
    
    // Create yin-yang particles
    int particleCount = 30;
    for (int i = 0; i < particleCount; i++) {
        ParticleEffect p;
        p.position = position;
        
        float angle = static_cast<float>(i) / particleCount * XM_2PI;
        float speed = 3.0f + (rand() % 100) / 50.0f;
        
        p.velocity.x = speed * cosf(angle);
        p.velocity.y = speed * 0.5f;
        p.velocity.z = speed * sinf(angle);
        
        // Half blue (light), half red (dark)
        if (i < particleCount / 2) {
            p.color = XMFLOAT4(0.3f, 0.6f, 1.0f, 1.0f); // Blue
        } else {
            p.color = XMFLOAT4(0.8f, 0.1f, 0.1f, 1.0f); // Red
        }
        
        p.life = 1.0f;
        p.size = 0.15f;
        p.type = "StanceSwitch";
        
        effect.particles.push_back(p);
    }
    
    m_stanceEffects.push_back(effect);
}

void RenderingSystemAdapter::DFRRenderingSystem::CreateHitEffect(
    const XMFLOAT3& position, float damage) {
    
    // Scale particle count with damage
    int particleCount = static_cast<int>(10 + damage / 10);
    particleCount = std::min(particleCount, 50);
    
    XMFLOAT4 effectColor(1.0f, 0.5f, 0.0f, 1.0f); // Orange for hits
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 1.0);
    
    for (int i = 0; i < particleCount; i++) {
        ParticleEffect p;
        p.position = position;
        
        // Random explosion pattern
        float theta = dis(gen) * XM_2PI;
        float phi = dis(gen) * XM_PI;
        float speed = 1.0f + dis(gen) * 3.0f;
        
        p.velocity.x = speed * sinf(phi) * cosf(theta);
        p.velocity.y = speed * cosf(phi);
        p.velocity.z = speed * sinf(phi) * sinf(theta);
        
        p.color = effectColor;
        p.life = 0.5f;
        p.size = 0.05f + dis(gen) * 0.1f;
        p.type = "Hit";
        
        m_particles.push_back(p);
    }
}

void RenderingSystemAdapter::DFRRenderingSystem::CreateBlockEffect(
    const XMFLOAT3& position) {
    
    // Blue shield particles
    int particleCount = 15;
    XMFLOAT4 effectColor(0.3f, 0.5f, 1.0f, 1.0f);
    
    for (int i = 0; i < particleCount; i++) {
        ParticleEffect p;
        
        // Shield bubble pattern
        float angle = static_cast<float>(i) / particleCount * XM_2PI;
        float radius = 1.0f;
        
        p.position = XMFLOAT3(
            position.x + radius * cosf(angle),
            position.y + 1.0f,
            position.z + radius * sinf(angle)
        );
        
        p.velocity = XMFLOAT3(0, 0.5f, 0); // Slight upward drift
        p.color = effectColor;
        p.life = 0.8f;
        p.size = 0.2f;
        p.type = "Block";
        
        m_particles.push_back(p);
    }
}

void RenderingSystemAdapter::DFRRenderingSystem::UpdateParticles(float deltaTime) {
    // Update regular particles
    for (auto it = m_particles.begin(); it != m_particles.end();) {
        it->life -= deltaTime;
        if (it->life <= 0.0f) {
            it = m_particles.erase(it);
        } else {
            it->position.x += it->velocity.x * deltaTime;
            it->position.y += it->velocity.y * deltaTime;
            it->position.z += it->velocity.z * deltaTime;
            
            // Apply gravity
            it->velocity.y -= 9.8f * deltaTime;
            
            // Fade out
            it->color.w = it->life;
            
            ++it;
        }
    }
    
    // Update stance switch effects
    for (auto it = m_stanceEffects.begin(); it != m_stanceEffects.end();) {
        it->timer -= deltaTime;
        
        // Update particles in effect
        for (auto& p : it->particles) {
            p.life -= deltaTime;
            p.position.x += p.velocity.x * deltaTime;
            p.position.y += p.velocity.y * deltaTime;
            p.position.z += p.velocity.z * deltaTime;
            p.velocity.y -= 5.0f * deltaTime;
            p.color.w = p.life;
        }
        
        if (it->timer <= 0.0f) {
            // Move particles to main list
            m_particles.insert(m_particles.end(),
                             it->particles.begin(),
                             it->particles.end());
            it = m_stanceEffects.erase(it);
        } else {
            ++it;
        }
    }
}

void RenderingSystemAdapter::DFRRenderingSystem::RenderParticles() {
    // In a real implementation, this would use instanced rendering
    // or a geometry shader for efficiency
    
    // For now, just ensure particles are being tracked
    // The actual rendering would reuse the character geometry
    // scaled down and positioned at each particle location
}

} // namespace ArenaFighter