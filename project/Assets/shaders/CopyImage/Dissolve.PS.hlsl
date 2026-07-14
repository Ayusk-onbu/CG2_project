#include "Fullscreen.hlsli"

struct DissolveConfig
{
    float32_t threshold;// しきいち
    float32_t edgeMin;
    float32_t edgeMax;
};

Texture2D<float32_t4> gTexture : register(t0);
Texture2D<float32_t> gMaskTexture : register(t1);
ConstantBuffer<DissolveConfig> gDissolveConfig : register(b0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

// ダメージ食らった演出に使えそう
PixelShaderOutput main(VertexShaderOutput input)
{
    float32_t mask = gMaskTexture.Sample(gSampler, input.texcoord);
    
    if (mask <= gDissolveConfig.threshold)
    {
        discard;
    }
    
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
    if (gDissolveConfig.threshold > 0.0f)
    {
        float32_t edge = 1.0f - smoothstep(gDissolveConfig.threshold + (gDissolveConfig.edgeMax - gDissolveConfig.edgeMin), gDissolveConfig.threshold, mask);
        output.color.rgb += edge * float32_t3(1.0f, 0.4f, 0.3f);
    }
    return output;
}