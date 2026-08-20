#include "Character.hlsli"
#include "../../Common.hlsli"

struct PixelShaderOutPut
{
    float32_t4 color : SV_TARGET0;
};

Texture2D<float32_t4> gTextures[] : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutPut main(VertexShaderOutput input)
{
    PixelShaderOutPut output;
    
    // 各テクスチャをバインドレスサンプリング
    float4 albedo = gTextures[input.colorTextureIndex].Sample(gSampler, input.texcoord);
    float3 normalMap = gTextures[input.normalTextureIndex].Sample(gSampler, input.texcoord).rgb;
   
    // 接空間からワールド空間への法線変換 (Common.hlsliの関数を利用)
    float3 worldNormal = CalculateWorldNormal(normalMap, input.worldPosition, input.texcoord, input.normal);
    //float3 worldNormal = input.normal;
    
    // 簡易ライティング（斜め上からの平行光源）
    float3 lightDir = normalize(float3(0.5f, -1.0f, 0.3f));
    float dotNL = saturate(dot(worldNormal, -lightDir));
    
    // 環境光を少し足す
    float3 ambient = float3(0.2f, 0.2f, 0.2f);
    float3 diffuse = dotNL * float3(1.0f, 1.0f, 1.0f);
    
    // 最終カラー計算
    output.color.rgb = albedo.rgb * (diffuse + ambient) * input.color.rgb;
    output.color.a = albedo.a * input.color.a;
    return output;
}