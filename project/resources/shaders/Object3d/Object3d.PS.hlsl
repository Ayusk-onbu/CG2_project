//float4 main() : SV_TARGET
//{
//	return float4(1.0f, 1.0f, 1.0f, 1.0f);
//}
#include "Object3d.hlsli"


struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct DirectionalLight
{
    float32_t4 color; // ライトの色
    float32_t3 direction; // ライトの向き
    float32_t intensity; // 輝度
    int32_t shadowType; // シャドウの種類
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

struct PixelShaderOutPut
{
    float32_t4 color : SV_TARGET0;
};
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
PixelShaderOutPut main(VertexShaderOutput input)
{
    PixelShaderOutPut output;
    float4 transformUV = mul(float32_t4(input.texcoord,0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformUV.xy);
    if (gMaterial.enableLighting != 0)
    {
        float cos = 1.0f;
        //switch (gDirectionalLight.shadowType)
        //{
        //    case 0:
        //        cos = 1.0f;
        //        break;
        //    case 1:
        //        cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        //        break;
        //    case 2:// Half
        //        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        //        cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        //        //output.color = float4(rgbColor, alpha);
        //        //output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
        //        break;
        //}
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        output.color.a = gMaterial.color.a * textureColor.a;
    }else{
        output.color = gMaterial.color * textureColor;
    }
        return output;
}