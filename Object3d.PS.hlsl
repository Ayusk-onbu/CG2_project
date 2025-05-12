//float4 main() : SV_TARGET
//{
//	return float4(1.0f, 1.0f, 1.0f, 1.0f);
//}
#include "Object3d.hlsli"


struct Material
{
    float32_t4 color;
    int32_t enableLighting;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct DirectionalLight
{
    float32_t4 color; // ライトの色
    float32_t3 direction; // ライトの向き
    float32_t intensity; // 輝度
};
ConstantBuffer<DirectionalLight> gDirectionLight : register(b1);

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
    if (gMaterial.enableLighting != 0)
    {
        float cos = saturate(dot(normalize(input.normal), -gDirectionLight.direction));
        output.color = gMaterial.color * textureColor * gDirectionLight.color * cos * gDirectionLight.intensity;
    }else{
        output.color = gMaterial.color * textureColor;
    }
        return output;
}