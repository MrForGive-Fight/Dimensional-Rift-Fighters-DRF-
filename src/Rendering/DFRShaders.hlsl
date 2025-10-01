// DFR Vertex Shader
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

PixelInput VSMain(VertexInput input) {
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

// DFR Pixel Shader
float4 PSMain(PixelInput input) : SV_TARGET {
    float3 normal = normalize(input.normal);
    float ndotl = max(0, dot(normal, -LightDir.xyz));
    
    // Rim lighting for special effects
    float3 viewDir = normalize(-input.worldPos);
    float rim = 1.0 - saturate(dot(viewDir, normal));
    rim = pow(rim, 2.0);
    
    float3 diffuse = LightColor.rgb * ndotl;
    float3 ambient = AmbientColor.rgb;
    
    float4 finalColor = input.color * TintColor;
    finalColor.rgb = finalColor.rgb * (ambient + diffuse);
    
    // Add rim light for special states
    if (TintColor.r > 0.9 && TintColor.g > 0.7) {
        // Gold tint (ultimate mode) - add strong rim
        finalColor.rgb += rim * float3(1.0, 0.8, 0.3) * 0.5;
    }
    else if (TintColor.b > 0.8) {
        // Blue tint (blocking) - add subtle rim
        finalColor.rgb += rim * float3(0.3, 0.5, 1.0) * 0.3;
    }
    
    return finalColor;
}

// Particle Vertex Shader
struct ParticleVertexInput {
    float3 position : POSITION;
    float2 texCoord : TEXCOORD0;
    float4 instancePos : TEXCOORD1; // xyz = position, w = size
    float4 instanceColor : TEXCOORD2;
};

struct ParticlePixelInput {
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float4 color : COLOR;
};

ParticlePixelInput VSParticle(ParticleVertexInput input) {
    ParticlePixelInput output;
    
    // Billboard the particle
    float3 worldPos = input.position * input.instancePos.w + input.instancePos.xyz;
    
    // Transform to screen space
    output.position = mul(float4(worldPos, 1.0f), View);
    output.position = mul(output.position, Projection);
    
    output.texCoord = input.texCoord;
    output.color = input.instanceColor;
    
    return output;
}

// Particle Pixel Shader
float4 PSParticle(ParticlePixelInput input) : SV_TARGET {
    // Soft circular particle
    float2 uv = input.texCoord * 2.0 - 1.0;
    float dist = length(uv);
    float alpha = 1.0 - saturate(dist);
    alpha = pow(alpha, 2.0);
    
    float4 color = input.color;
    color.a *= alpha;
    
    return color;
}

// Special Effects Techniques

// Stance Switch Effect - Yin Yang swirl
float4 PSStanceSwitch(ParticlePixelInput input) : SV_TARGET {
    float2 uv = input.texCoord * 2.0 - 1.0;
    float angle = atan2(uv.y, uv.x);
    float dist = length(uv);
    
    // Create yin-yang pattern
    float pattern = sin(angle * 2.0) * 0.5 + 0.5;
    
    float4 color = input.color;
    if (pattern > 0.5) {
        color = float4(0.3, 0.6, 1.0, 1.0); // Light blue
    } else {
        color = float4(0.8, 0.1, 0.1, 1.0); // Dark red
    }
    
    float alpha = 1.0 - saturate(dist);
    color.a *= alpha * input.color.a;
    
    return color;
}

// Ground Impact Effect - Radial waves
float4 PSGroundImpact(PixelInput input) : SV_TARGET {
    float2 uv = input.texCoord * 2.0 - 1.0;
    float dist = length(uv);
    
    // Create wave rings
    float wave = sin(dist * 20.0 - input.worldPos.y * 10.0) * 0.5 + 0.5;
    wave = pow(wave, 3.0);
    
    float4 color = input.color * TintColor;
    color.rgb *= wave;
    color.a *= (1.0 - saturate(dist)) * wave;
    
    return color;
}