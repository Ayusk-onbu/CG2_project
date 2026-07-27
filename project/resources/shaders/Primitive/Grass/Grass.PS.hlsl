#include "Grass.hlsli"

struct PixelShaderOutPut
{
    float32_t4 color : SV_TARGET0;
};

Texture2D<float32_t4> gTextures[] : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutPut main(VertexShaderOutput input)
{
    PixelShaderOutPut output;
    float32_t4 textureColor = gTextures[input.textureIndex].Sample(gSampler, input.texcoord);
    if (textureColor.a < 0.5f)
    {
        discard;
    }
    
    output.color = textureColor * input.color;
    
    return output;
}