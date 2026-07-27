#include "Grass.hlsli"
struct GrassForGPU
{
    float32_t4x4 WVP;
    float32_t4x4 World;
    float32_t4 color;
    float32_t windPhase;
    uint32_t textureIndex;
    float32_t2 padding;
};

cbuffer cbTime : register(b0)
{
    float32_t g_time;
};

StructuredBuffer<GrassForGPU> gGrass : register(t0);

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input,uint32_t instancedId : SV_InstanceID)
{
    VertexShaderOutput output;
    // 現在のインスタンスデータを取得
    GrassForGPU instData = gGrass[instancedId];
    
    // ローカル座標を取り出す
    float32_t4 localPos = input.position;
    
    // 【風の揺れ計算】
    // input.uv.y を使って「上に行くほど(UVが0に近いほど)揺れる」ようにする
    // (※下をUV=1.0、上をUV=0.0と想定。モデルに合わせて input.pos.y 等に変えてもOK)
    float32_t heightFactor = 1.0f - input.texcoord.y;
    
    if (heightFactor > 0.01f)
    {
        // g_time と windPhase を使ってSin波を作る
        // g_time * 2.0f は風の速さ。適宜調整。
        float32_t wave = sin(g_time * 2.0f + instData.windPhase);
        
        // 高さに対してどれくらい揺らすか（0.3fは揺れの強さ）
        float32_t windOffset = wave * heightFactor * 0.3f;
        
        // Z方向（またはX方向）に頂点をずらす
        localPos.x += windOffset;
        localPos.z += windOffset * 0.5f; // 少し斜めにも揺らす
    }
    
    // 計算したローカル座標を行列で変換
    output.position = mul(localPos, instData.WVP);
    
    output.texcoord = input.texcoord;
    output.color = instData.color;
    output.normal = normalize(mul(input.normal,(float32_t3x3) instData.World));
    
    output.textureIndex = instData.textureIndex;
    
    return output;
}