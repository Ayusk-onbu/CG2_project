#include "Fullscreen.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    float32_t2 correct = input.texcoord * (1.0f - input.texcoord.yx);
    
    float vignette = correct.x * correct.y * 16.0f;// 0.25 * 0.25 = Anser * 16 = 1.0f <= Reason
    
    vignette = saturate(pow(vignette, 0.8f));//This volum is Width
    
    output.color.gb *= vignette;
    
    return output;
}