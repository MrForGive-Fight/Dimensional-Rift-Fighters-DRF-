#include "UIRenderer.h"
#include <algorithm>
#include <sstream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace ArenaFighter {

// Vertex shader source
const char* g_vertexShaderSource = R"(
cbuffer ConstantBuffer : register(b0) {
    matrix Projection;
    float4 TintColor;
};

struct VS_INPUT {
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
    float4 Color : COLOR;
};

struct PS_INPUT {
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float4 Color : COLOR;
};

PS_INPUT main(VS_INPUT input) {
    PS_INPUT output;
    output.Position = mul(float4(input.Position, 1.0f), Projection);
    output.TexCoord = input.TexCoord;
    output.Color = input.Color * TintColor;
    return output;
}
)";

// Pixel shader for solid colors
const char* g_pixelShaderSource = R"(
struct PS_INPUT {
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float4 Color : COLOR;
};

float4 main(PS_INPUT input) : SV_TARGET {
    return input.Color;
}
)";

// Pixel shader for textures
const char* g_texturePixelShaderSource = R"(
Texture2D UITexture : register(t0);
SamplerState UISampler : register(s0);

struct PS_INPUT {
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float4 Color : COLOR;
};

float4 main(PS_INPUT input) : SV_TARGET {
    float4 texColor = UITexture.Sample(UISampler, input.TexCoord);
    return texColor * input.Color;
}
)";

UIRenderer::UIRenderer(ID3D11Device* device, ID3D11DeviceContext* context)
    : m_device(device)
    , m_context(context)
    , m_viewportWidth(1920)
    , m_viewportHeight(1080)
    , m_renderingStarted(false)
    , m_currentVertex(0)
    , m_currentIndex(0) {
    
    createShaders();
    createBuffers();
    createRenderStates();
    createWhiteTexture();
    loadDefaultFont();
    
    // Reserve space for batching
    m_vertices.reserve(10000);
    m_indices.reserve(15000);
}

UIRenderer::~UIRenderer() = default;

void UIRenderer::createShaders() {
    ComPtr<ID3DBlob> vsBlob, psBlob, tpsBlob;
    ComPtr<ID3DBlob> errorBlob;
    
    // Compile vertex shader
    HRESULT hr = D3DCompile(g_vertexShaderSource, strlen(g_vertexShaderSource), nullptr,
                           nullptr, nullptr, "main", "vs_4_0", 0, 0, &vsBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        }
        throw std::runtime_error("Failed to compile vertex shader");
    }
    
    m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
                                nullptr, &m_vertexShader);
    
    // Compile pixel shaders
    hr = D3DCompile(g_pixelShaderSource, strlen(g_pixelShaderSource), nullptr,
                   nullptr, nullptr, "main", "ps_4_0", 0, 0, &psBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        }
        throw std::runtime_error("Failed to compile pixel shader");
    }
    
    m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(),
                               nullptr, &m_pixelShader);
    
    hr = D3DCompile(g_texturePixelShaderSource, strlen(g_texturePixelShaderSource), nullptr,
                   nullptr, nullptr, "main", "ps_4_0", 0, 0, &tpsBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        }
        throw std::runtime_error("Failed to compile texture pixel shader");
    }
    
    m_device->CreatePixelShader(tpsBlob->GetBufferPointer(), tpsBlob->GetBufferSize(),
                               nullptr, &m_texturePixelShader);
    
    // Create input layout
    D3D11_INPUT_ELEMENT_DESC inputLayout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    
    m_device->CreateInputLayout(inputLayout, 3, vsBlob->GetBufferPointer(),
                               vsBlob->GetBufferSize(), &m_inputLayout);
}

void UIRenderer::createBuffers() {
    // Create dynamic vertex buffer
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_DYNAMIC;
    vbDesc.ByteWidth = sizeof(UIVertex) * 10000;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    
    m_device->CreateBuffer(&vbDesc, nullptr, &m_vertexBuffer);
    
    // Create dynamic index buffer
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.Usage = D3D11_USAGE_DYNAMIC;
    ibDesc.ByteWidth = sizeof(uint32_t) * 15000;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    
    m_device->CreateBuffer(&ibDesc, nullptr, &m_indexBuffer);
    
    // Create constant buffer
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.ByteWidth = sizeof(UIConstantBuffer);
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    
    m_device->CreateBuffer(&cbDesc, nullptr, &m_constantBuffer);
}

void UIRenderer::createRenderStates() {
    // Create sampler state
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    
    m_device->CreateSamplerState(&samplerDesc, &m_samplerState);
    
    // Create blend state for alpha blending
    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;
    blendDesc.RenderTarget[0].BlendEnable = true;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    
    m_device->CreateBlendState(&blendDesc, &m_blendState);
    
    // Create rasterizer state
    D3D11_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_NONE;
    rasterizerDesc.FrontCounterClockwise = false;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.SlopeScaledDepthBias = 0.0f;
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.DepthClipEnable = true;
    rasterizerDesc.ScissorEnable = false;
    rasterizerDesc.MultisampleEnable = false;
    rasterizerDesc.AntialiasedLineEnable = false;
    
    m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);
    
    // Create depth stencil state (disable depth testing for UI)
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = false;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = false;
    
    m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
}

void UIRenderer::createWhiteTexture() {
    // Create a 1x1 white texture for solid color rendering
    uint32_t white = 0xFFFFFFFF;
    
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = 1;
    texDesc.Height = 1;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = &white;
    initData.SysMemPitch = sizeof(uint32_t);
    
    ComPtr<ID3D11Texture2D> texture;
    m_device->CreateTexture2D(&texDesc, &initData, &texture);
    
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    
    m_device->CreateShaderResourceView(texture.Get(), &srvDesc, &m_whiteTexture);
}

void UIRenderer::loadDefaultFont() {
    // In a real implementation, this would load a font texture atlas
    // For now, we'll use a placeholder
    m_defaultFont.charWidth = 8.0f;
    m_defaultFont.charHeight = 16.0f;
    m_defaultFont.spacing = 1.0f;
    m_defaultFont.texture = m_whiteTexture;  // Placeholder
}

void UIRenderer::begin() {
    if (m_renderingStarted) {
        throw std::runtime_error("UIRenderer::begin() called without matching end()");
    }
    
    m_renderingStarted = true;
    m_vertices.clear();
    m_indices.clear();
    m_currentVertex = 0;
    m_currentIndex = 0;
    
    // Set up render states
    m_context->IASetInputLayout(m_inputLayout.Get());
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    UINT stride = sizeof(UIVertex);
    UINT offset = 0;
    m_context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    m_context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    
    m_context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    m_context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    
    m_context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
    m_context->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    
    m_context->OMSetBlendState(m_blendState.Get(), nullptr, 0xffffffff);
    m_context->RSSetState(m_rasterizerState.Get());
    m_context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);
    
    // Update projection matrix
    XMMATRIX projection = XMMatrixOrthographicOffCenterLH(
        0.0f, static_cast<float>(m_viewportWidth),
        static_cast<float>(m_viewportHeight), 0.0f,
        0.0f, 1.0f
    );
    
    UIConstantBuffer cb;
    cb.projection = XMMatrixTranspose(projection);
    cb.tintColor = XMFLOAT4(1, 1, 1, 1);
    
    m_context->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &cb, 0, 0);
}

void UIRenderer::end() {
    if (!m_renderingStarted) {
        throw std::runtime_error("UIRenderer::end() called without matching begin()");
    }
    
    flushBatch();
    m_renderingStarted = false;
}

void UIRenderer::flushBatch() {
    if (m_vertices.empty()) return;
    
    // Update vertex buffer
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    m_context->Map(m_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy(mappedResource.pData, m_vertices.data(), m_vertices.size() * sizeof(UIVertex));
    m_context->Unmap(m_vertexBuffer.Get(), 0);
    
    // Update index buffer
    m_context->Map(m_indexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy(mappedResource.pData, m_indices.data(), m_indices.size() * sizeof(uint32_t));
    m_context->Unmap(m_indexBuffer.Get(), 0);
    
    // Draw
    m_context->DrawIndexed(static_cast<UINT>(m_indices.size()), 0, 0);
    
    // Clear for next batch
    m_vertices.clear();
    m_indices.clear();
    m_currentVertex = 0;
    m_currentIndex = 0;
}

void UIRenderer::addQuad(const XMFLOAT2& pos, const XMFLOAT2& size, const XMFLOAT4& color,
                        const XMFLOAT2& uvMin, const XMFLOAT2& uvMax) {
    // Check if we need to flush
    if (m_currentVertex + 4 > 10000 || m_currentIndex + 6 > 15000) {
        flushBatch();
    }
    
    uint32_t baseVertex = static_cast<uint32_t>(m_currentVertex);
    
    // Add vertices
    m_vertices.push_back(UIVertex(pos.x, pos.y, 0, uvMin.x, uvMin.y, color));
    m_vertices.push_back(UIVertex(pos.x + size.x, pos.y, 0, uvMax.x, uvMin.y, color));
    m_vertices.push_back(UIVertex(pos.x, pos.y + size.y, 0, uvMin.x, uvMax.y, color));
    m_vertices.push_back(UIVertex(pos.x + size.x, pos.y + size.y, 0, uvMax.x, uvMax.y, color));
    
    // Add indices
    m_indices.push_back(baseVertex + 0);
    m_indices.push_back(baseVertex + 1);
    m_indices.push_back(baseVertex + 2);
    m_indices.push_back(baseVertex + 1);
    m_indices.push_back(baseVertex + 3);
    m_indices.push_back(baseVertex + 2);
    
    m_currentVertex += 4;
    m_currentIndex += 6;
}

void UIRenderer::renderRect(const XMFLOAT2& position, const XMFLOAT2& size, const XMFLOAT4& color) {
    m_context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
    m_context->PSSetShaderResources(0, 1, m_whiteTexture.GetAddressOf());
    addQuad(position, size, color);
}

void UIRenderer::renderRectOutline(const XMFLOAT2& position, const XMFLOAT2& size, const XMFLOAT4& color, float thickness) {
    // Top
    renderRect(position, XMFLOAT2(size.x, thickness), color);
    // Bottom
    renderRect(XMFLOAT2(position.x, position.y + size.y - thickness), XMFLOAT2(size.x, thickness), color);
    // Left
    renderRect(XMFLOAT2(position.x, position.y + thickness), XMFLOAT2(thickness, size.y - 2 * thickness), color);
    // Right
    renderRect(XMFLOAT2(position.x + size.x - thickness, position.y + thickness), 
              XMFLOAT2(thickness, size.y - 2 * thickness), color);
}

void UIRenderer::renderRoundedRect(const XMFLOAT2& position, const XMFLOAT2& size, const XMFLOAT4& color, float radius) {
    // For simplicity, render as normal rect for now
    // A full implementation would use a shader or multiple quads to create rounded corners
    renderRect(position, size, color);
}

void UIRenderer::renderRoundedRectOutline(const XMFLOAT2& position, const XMFLOAT2& size, const XMFLOAT4& color, 
                                         float thickness, float radius) {
    // For simplicity, render as normal rect outline for now
    renderRectOutline(position, size, color, thickness);
}

void UIRenderer::renderTexture(ID3D11ShaderResourceView* texture, const XMFLOAT2& position, 
                              const XMFLOAT2& size, const XMFLOAT4& tint) {
    m_context->PSSetShader(m_texturePixelShader.Get(), nullptr, 0);
    m_context->PSSetShaderResources(0, 1, &texture);
    addQuad(position, size, tint);
}

void UIRenderer::renderTextureRegion(ID3D11ShaderResourceView* texture, const XMFLOAT2& position,
                                    const XMFLOAT2& size, const XMFLOAT2& uvMin, const XMFLOAT2& uvMax,
                                    const XMFLOAT4& tint) {
    m_context->PSSetShader(m_texturePixelShader.Get(), nullptr, 0);
    m_context->PSSetShaderResources(0, 1, &texture);
    addQuad(position, size, tint, uvMin, uvMax);
}

void UIRenderer::renderText(const std::string& text, const XMFLOAT2& position, const XMFLOAT4& color, float fontSize) {
    // Simple text rendering - in a real implementation, this would use a font atlas
    float scale = fontSize / m_defaultFont.charHeight;
    float x = position.x;
    float y = position.y;
    
    for (char c : text) {
        if (c == ' ') {
            x += (m_defaultFont.charWidth + m_defaultFont.spacing) * scale;
            continue;
        }
        
        // For now, just render rectangles for each character
        renderRect(XMFLOAT2(x, y), XMFLOAT2(m_defaultFont.charWidth * scale, fontSize), color);
        x += (m_defaultFont.charWidth + m_defaultFont.spacing) * scale;
    }
}

void UIRenderer::renderTextCentered(const std::string& text, const XMFLOAT2& position, const XMFLOAT4& color, float fontSize) {
    XMFLOAT2 textSize = measureText(text, fontSize);
    XMFLOAT2 centeredPos(position.x - textSize.x * 0.5f, position.y - textSize.y * 0.5f);
    renderText(text, centeredPos, color, fontSize);
}

void UIRenderer::renderTextRightAligned(const std::string& text, const XMFLOAT2& position, const XMFLOAT4& color, float fontSize) {
    XMFLOAT2 textSize = measureText(text, fontSize);
    XMFLOAT2 rightAlignedPos(position.x - textSize.x, position.y);
    renderText(text, rightAlignedPos, color, fontSize);
}

void UIRenderer::renderTextWrapped(const std::string& text, const XMFLOAT2& position, float maxWidth, 
                                  const XMFLOAT4& color, float fontSize) {
    // Simple word wrapping
    std::istringstream stream(text);
    std::string word;
    float x = position.x;
    float y = position.y;
    float lineHeight = fontSize * 1.2f;
    float spaceWidth = (m_defaultFont.charWidth + m_defaultFont.spacing) * (fontSize / m_defaultFont.charHeight);
    
    while (stream >> word) {
        XMFLOAT2 wordSize = measureText(word, fontSize);
        
        if (x + wordSize.x > position.x + maxWidth && x > position.x) {
            x = position.x;
            y += lineHeight;
        }
        
        renderText(word, XMFLOAT2(x, y), color, fontSize);
        x += wordSize.x + spaceWidth;
    }
}

void UIRenderer::setViewportSize(int width, int height) {
    m_viewportWidth = width;
    m_viewportHeight = height;
}

XMFLOAT2 UIRenderer::measureText(const std::string& text, float fontSize) {
    float scale = fontSize / m_defaultFont.charHeight;
    float width = text.length() * (m_defaultFont.charWidth + m_defaultFont.spacing) * scale;
    return XMFLOAT2(width, fontSize);
}

bool UIRenderer::loadFont(const std::string& fontPath) {
    // TODO: Implement font loading from file
    return false;
}

void UIRenderer::setDefaultFont(const FontData& font) {
    m_defaultFont = font;
}

} // namespace ArenaFighter