#include "Hair.hlsli"

StructuredBuffer<StrandVertex> g_InVertexBuffer : register(t0); // 生成された髪頂点
RWStructuredBuffer<RaytracingAABB> g_OutAABBBuffer : register(u0); // DXRが読み込むAABB配列
ConstantBuffer<HairConfig> gHairConfig : register(b0);

[numthreads(64, 1, 1)] // 64本ずつ並列処理
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint strandIdx = dispatchThreadID.x;
    
    if (strandIdx >= gHairConfig.numStrands * gHairConfig.pointPerStrand) // もしくは相当する総ストランド数
    {
        return;
    }
    
    // この髪の毛の頂点の開始位置
    uint vertexStartIndex = strandIdx * gHairConfig.pointPerStrand;
    
    // この髪の毛のAABBの書き込み開始位置（1本につき15個のAABBを出力）
    uint aabbStartIndex = strandIdx * (gHairConfig.pointPerStrand - 1);

    // -------------------------------------------------------------
    // セグメントをループして、それぞれのAABBを計算
    // -------------------------------------------------------------
    for (uint i = 0; i < gHairConfig.pointPerStrand - 1; ++i)
    {
        // セグメントを構成する「現在の頂点」と「次の頂点」をロード
        StrandVertex v0 = g_InVertexBuffer[vertexStartIndex + i];
        StrandVertex v1 = g_InVertexBuffer[vertexStartIndex + i + 1];

        // 2つの頂点の座標から、単純な最小値・最大値を求める
        float3 minPos = min(v0.position, v1.position);
        float3 maxPos = max(v0.position, v1.position);

        // 髪の毛には「太さ（半径）」があるため、
        // 半径の分だけAABBの箱を外側に押し広げないと、レイトレの光線がかすった時に貫通（バグ）します
        float maxRadius = max(v0.radius, v1.radius);
        
        minPos -= float3(maxRadius, maxRadius, maxRadius);
        maxPos += float3(maxRadius, maxRadius, maxRadius);

        // ※DXRの仕様上、万が一minとmaxが完全に同じ値（サイズゼロの箱）になると
        // 加速構造のビルドでクラッシュか不具合が起きるため、微小な厚みを持たせる安全弁
        //float3 boxSize = maxPos - minPos;
        //if (boxSize.x < 0.001)
        //{
        //    minPos.x -= 0.0005;
        //    maxPos.x += 0.0005;
        //}
        //if (boxSize.y < 0.001)
        //{
        //    minPos.y -= 0.0005;
        //    maxPos.y += 0.0005;
        //}
        //if (boxSize.z < 0.001)
        //{
        //    minPos.z -= 0.0005;
        //    maxPos.z += 0.0005;
        //}

        // 最終的なAABBデータをバッファへ書き込み
        RaytracingAABB aabb;
        aabb.minPositionX = minPos.x;
        aabb.minPositionY = minPos.y;
        aabb.minPositionZ = minPos.z;
        aabb.maxPositionX = maxPos.x;
        aabb.maxPositionY = maxPos.y;
        aabb.maxPositionZ = maxPos.z;

        g_OutAABBBuffer[aabbStartIndex + i] = aabb;
    }
}