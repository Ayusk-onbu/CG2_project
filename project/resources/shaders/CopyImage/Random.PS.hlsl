#include "Fullscreen.hlsli"

struct RandomConfig
{
    float32_t time;// しきいち
};

Texture2D<float32_t4> gTexture : register(t0);
ConstantBuffer<RandomConfig> gRandomConfig : register(b0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

float rand2dTo1d(float2 value, float2 dotDir = float2(12.9898, 78.233))
{
    float2 smallValue = sin(value);
    float random = dot(smallValue, dotDir);
    random = frac(sin(random) * 143758.5453);
    return random;
}

// ダメージ食らった演出に使えそう
PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    // 乱数生成
    float32_t random = rand2dTo1d(input.texcoord * gRandomConfig.time);
    
    output.color = gTexture.Sample(gSampler, input.texcoord);
    output.color.rgb *= random;
    return output;
}