#include "Hair.hlsli"

ConstantBuffer<HairConfig> gHairConfig : register(b0);
ConstantBuffer<Camera> gCamera : register(b1);
StructuredBuffer<StrandVertex> g_InVertexBuffer : register(t0); // 生成された髪頂点
StructuredBuffer<StrandInfo> g_StrandInfoBuffer : register(t1);
RWStructuredBuffer<StrandVertex> g_OutTriangleBuffer : register(u0);

[numthreads(64, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint strandIdx = dispatchThreadID.x;
    
    if (strandIdx >= gHairConfig.numStrands)return;
    
    // 頂点数とか情報を取得
    StrandInfo info = g_StrandInfoBuffer[strandIdx];
    
    // 頂点が2個未満ならセグメント（線分）を作れないので終了
    if (info.vertexCount < 2)return;
    
    // -------------------------------------------------------------
    // セグメントをループして、それぞれのAABBを計算
    // -------------------------------------------------------------
    for (uint i = 0; i < info.vertexCount - 1; ++i)
    {
        // インデックスを計算
        uint32_t v0_idx = info.vertexStartIndex + i;
        uint32_t v1_idx = info.vertexStartIndex + i + 1;

        // セグメントを構成する「現在の頂点」と「次の頂点」をロード
        StrandVertex v0 = g_InVertexBuffer[v0_idx];
        StrandVertex v1 = g_InVertexBuffer[v1_idx];

        // 💡 髪の毛の方向（接線）
        float3 tangent = normalize(v1.position - v0.position);
        
        // 💡 カメラから見た方向（レイの飛んでくる方向）
        float3 viewDir = normalize(v0.position - gCamera.position);
        
        // 💡 接線とカメラ方向の外積で、髪を広げる「横方向（Right）」を作る！
        float3 right = cross(tangent, viewDir);
        if (length(right) < 0.001f)
        {
            right = cross(tangent, float3(1, 0, 0)); // 特異点対策
        }
        right = normalize(right);

        // 💡 髪の太さ(radius)を使って、線分を「幅のある板」に押し広げる
        StrandVertex p0 = v0;
        p0.position -= right * v0.radius;
        p0.padding = -1.0f; // 左下
        StrandVertex p1 = v0;
        p1.position += right * v0.radius;
        p1.padding = 1.0f; // 右下
        StrandVertex p2 = v1;
        p2.position -= right * v1.radius;
        p2.padding = -1.0f; // 左上
        StrandVertex p3 = v1;
        p3.position += right * v1.radius;
        p3.padding = 1.0f; // 右上
        // 💡 1セグメントにつき、6個の頂点（三角形2枚分）をフラットバッファに書き込む
        uint baseIdx = (info.aabbStartIndex + i) * 6;

        // 1枚目の三角形 (左下, 右下, 左上)
        g_OutTriangleBuffer[baseIdx + 0] = p0;
        g_OutTriangleBuffer[baseIdx + 1] = p1;
        g_OutTriangleBuffer[baseIdx + 2] = p2;
        
        // 2枚目の三角形 (左上, 右下, 右上)
        g_OutTriangleBuffer[baseIdx + 3] = p2;
        g_OutTriangleBuffer[baseIdx + 4] = p1;
        g_OutTriangleBuffer[baseIdx + 5] = p3;
    }
}