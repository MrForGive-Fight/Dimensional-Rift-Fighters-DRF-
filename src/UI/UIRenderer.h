#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <memory>
#include <wrl/client.h>

using namespace DirectX;
using Microsoft::WRL::ComPtr;

namespace ArenaFighter {

// Vertex structure for UI rendering
struct UIVertex {
    XMFLOAT3 position;
    XMFLOAT2 texCoord;
    XMFLOAT4 color;
    
    UIVertex() = default;
    UIVertex(float x, float y, float z, float u, float v, const XMFLOAT4& col)
        : position(x, y, z), texCoord(u, v), color(col) {}
};

// Constant buffer for UI rendering
struct UIConstantBuffer {
    XMMATRIX projection;
    XMFLOAT4 tintColor;
};

// Font data structure
struct FontData {
    ComPtr<ID3D11ShaderResourceView> texture;
    float charWidth;
    float charHeight;
    float spacing;
};

// Batch render command
struct RenderCommand {
    enum Type {
        DrawRect,
        DrawTexture,
        DrawText
    };
    
    Type type;
    XMFLOAT2 position;
    XMFLOAT2 size;
    XMFLOAT4 color;
    ID3D11ShaderResourceView* texture;
    std::string text;
    float fontSize;
    float cornerRadius;
};

// UI Renderer class
class UIRenderer {
private:
    ComPtr<ID3D11Device> m_device;
    ComPtr<ID3D11DeviceContext> m_context;
    ComPtr<ID3D11VertexShader> m_vertexShader;
    ComPtr<ID3D11PixelShader> m_pixelShader;
    ComPtr<ID3D11PixelShader> m_texturePixelShader;
    ComPtr<ID3D11InputLayout> m_inputLayout;
    ComPtr<ID3D11Buffer> m_vertexBuffer;
    ComPtr<ID3D11Buffer> m_indexBuffer;
    ComPtr<ID3D11Buffer> m_constantBuffer;
    ComPtr<ID3D11SamplerState> m_samplerState;
    ComPtr<ID3D11BlendState> m_blendState;
    ComPtr<ID3D11RasterizerState> m_rasterizerState;
    ComPtr<ID3D11DepthStencilState> m_depthStencilState;
    
    // White texture for solid color rendering
    ComPtr<ID3D11ShaderResourceView> m_whiteTexture;
    
    // Font system
    FontData m_defaultFont;
    
    // Batching
    std::vector<UIVertex> m_vertices;
    std::vector<uint32_t> m_indices;
    std::vector<RenderCommand> m_commands;
    size_t m_currentVertex;
    size_t m_currentIndex;
    
    // Viewport
    int m_viewportWidth;
    int m_viewportHeight;
    
    // State tracking
    bool m_renderingStarted;
    
    // Helper methods
    void createShaders();
    void createBuffers();
    void createRenderStates();
    void createWhiteTexture();
    void loadDefaultFont();
    void flushBatch();
    void addQuad(const XMFLOAT2& pos, const XMFLOAT2& size, const XMFLOAT4& color, 
                 const XMFLOAT2& uvMin = XMFLOAT2(0,0), const XMFLOAT2& uvMax = XMFLOAT2(1,1));
    void addRoundedQuad(const XMFLOAT2& pos, const XMFLOAT2& size, const XMFLOAT4& color, float radius);
    
public:
    UIRenderer(ID3D11Device* device, ID3D11DeviceContext* context);
    ~UIRenderer();
    
    // Rendering methods
    void begin();
    void end();
    
    // Basic shapes
    void renderRect(const XMFLOAT2& position, const XMFLOAT2& size, const XMFLOAT4& color);
    void renderRectOutline(const XMFLOAT2& position, const XMFLOAT2& size, const XMFLOAT4& color, float thickness);
    void renderRoundedRect(const XMFLOAT2& position, const XMFLOAT2& size, const XMFLOAT4& color, float radius);
    void renderRoundedRectOutline(const XMFLOAT2& position, const XMFLOAT2& size, const XMFLOAT4& color, float thickness, float radius);
    
    // Texture rendering
    void renderTexture(ID3D11ShaderResourceView* texture, const XMFLOAT2& position, const XMFLOAT2& size, 
                      const XMFLOAT4& tint = XMFLOAT4(1,1,1,1));
    void renderTextureRegion(ID3D11ShaderResourceView* texture, const XMFLOAT2& position, const XMFLOAT2& size,
                            const XMFLOAT2& uvMin, const XMFLOAT2& uvMax, const XMFLOAT4& tint = XMFLOAT4(1,1,1,1));
    
    // Text rendering
    void renderText(const std::string& text, const XMFLOAT2& position, const XMFLOAT4& color, float fontSize);
    void renderTextCentered(const std::string& text, const XMFLOAT2& position, const XMFLOAT4& color, float fontSize);
    void renderTextRightAligned(const std::string& text, const XMFLOAT2& position, const XMFLOAT4& color, float fontSize);
    void renderTextWrapped(const std::string& text, const XMFLOAT2& position, float maxWidth, const XMFLOAT4& color, float fontSize);
    
    // Utility
    void setViewportSize(int width, int height);
    XMFLOAT2 measureText(const std::string& text, float fontSize);
    
    // Font management
    bool loadFont(const std::string& fontPath);
    void setDefaultFont(const FontData& font);
};

} // namespace ArenaFighter