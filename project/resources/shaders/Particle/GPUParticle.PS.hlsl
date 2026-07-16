#include "Particle.hlsli"

struct PixelShaderOutPut
{
    float32_t4 color : SV_TARGET0;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutPut main(VertexShaderOutput input)
{
    PixelShaderOutPut output;
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    // input.color * texture.color
    output.color = textureColor * input.color;
    if (output.color.a == 0.0)
    {
        discard;
    }
    return output;
}