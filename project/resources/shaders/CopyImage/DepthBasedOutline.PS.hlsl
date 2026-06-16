#include "Fullscreen.hlsli"

struct Material
{
    float32_t4x4 projectionInverse;
};

Texture2D<float32_t4> gTexture : register(t0);
Texture2D<float32_t> gDepthTexture : register(t1);
ConstantBuffer<Material> gMaterial : register(b0);
SamplerState gSampler : register(s0);
SamplerState gSamplerPoint : register(s1);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

static const float32_t kPrewittHorizontalKernek[3][3] =
{
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f }
};

static const float32_t kPrewittVerticalKernel[3][3] =
{
    { -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
    { 0.0f, 0.0f, 0.0f },
    { 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f }
};

static const float32_t2 kIndex3x3[3][3] =
{
    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } },
    { { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }
};

PixelShaderOutput main(VertexShaderOutput input)
{
    uint32_t width, height;
    gTexture.GetDimensions(width, height);
    float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));// rcp = reverse
    
    float32_t2 differce = float32_t2(0.0f, 0.0f);
    for (int32_t x = 0; x < 3; ++x)
    {
        for (int32_t y = 0; y < 3; ++y)
        {
            // Get texcoord
            float32_t2 texcoord = input.texcoord + kIndex3x3[x][y] * uvStepSize;
            // Linerのほうが綺麗じゃね
            float32_t ndcDepth = gDepthTexture.Sample(gSamplerPoint, texcoord); // 0.0(手前) ～ 1.0(一番奥)
            float32_t4 viewSpace = mul(float32_t4(0.0f, 0.0f, ndcDepth, 1.0f), gMaterial.projectionInverse);
            float32_t viewZ = viewSpace.z / rcp(viewSpace.w);
            
            differce.x += viewZ * kPrewittHorizontalKernek[x][y];
            differce.y += viewZ * kPrewittVerticalKernel[x][y];
        }
    }
    float32_t weight = length(differce);
    weight = saturate(weight);
    
    PixelShaderOutput output;
    output.color.rgb = (1.0f - weight) * gTexture.Sample(gSampler, input.texcoord).rgb;
    output.color.a = 1.0f;
    
    return output;
}