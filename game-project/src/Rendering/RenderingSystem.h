#pragma once

#include <memory>
#include <vector>
#include <string>
#include <DirectXMath.h>

namespace ArenaFighter {

class Model;
class Animation;
class Camera;

class RenderingSystem {
public:
    RenderingSystem() = default;
    ~RenderingSystem() = default;

    bool Initialize();
    void Shutdown();

    void BeginFrame();
    void EndFrame();

    // Model Management
    std::shared_ptr<Model> LoadModel(const std::string& path);
    void RenderModel(const Model* model, const DirectX::XMMATRIX& worldMatrix);
    
    // Animation
    std::shared_ptr<Animation> LoadAnimation(const std::string& path);
    void PlayAnimation(Model* model, Animation* animation, float time);

    // Camera
    void SetCamera(Camera* camera) { m_activeCamera = camera; }
    Camera* GetCamera() { return m_activeCamera; }

    // Rendering Features
    void EnableShadows(bool enable) { m_shadowsEnabled = enable; }
    void SetAnimeShading(bool enable) { m_animeShading = enable; }

private:
    // D3D12 Resources (implementation details)
    void* m_device = nullptr;
    void* m_commandQueue = nullptr;
    void* m_swapChain = nullptr;
    
    Camera* m_activeCamera = nullptr;
    
    // Render settings
    bool m_shadowsEnabled = true;
    bool m_animeShading = true;  // Anime-style cel shading
    
    // Resource pools
    std::vector<std::shared_ptr<Model>> m_loadedModels;
    std::vector<std::shared_ptr<Animation>> m_loadedAnimations;
};