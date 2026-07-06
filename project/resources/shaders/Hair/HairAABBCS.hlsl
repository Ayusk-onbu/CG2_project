#include "Hair.hlsli"

ConstantBuffer<HairConfig> gHairConfig : register(b0);
StructuredBuffer<StrandVertex> g_InVertexBuffer : register(t0); // 生成された髪頂点
StructuredBuffer<StrandInfo> g_StrandInfoBuffer : register(t1);
RWStructuredBuffer<RaytracingAABB> g_OutAABBBuffer : register(u0); // DXRが読み込むAABB配列
//StructuredBuffer<SegmentData> g_OutSegmentBuffer : register(t2);

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

        // 2つの頂点の座標から、AABBを生成するために単純な最小値・最大値を求める
        float3 minPos = min(v0.position, v1.position);
        float3 maxPos = max(v0.position, v1.position);

        // 髪の毛には「太さ（半径）」があるため、
        // 半径の分だけAABBの箱を外側に押し広げないと、レイトレの光線がかすった時に貫通
        float maxRadius = max(v0.radius, v1.radius);
        
        minPos -= float3(maxRadius, maxRadius, maxRadius);
        maxPos += float3(maxRadius, maxRadius, maxRadius);

        // ※DXRの仕様上、万が一minとmaxが完全に同じ値（サイズゼロの箱）になると
        // 加速構造のビルドでクラッシュか不具合が起きるため、微小な厚みを持たせる安全弁
        float3 boxSize = maxPos - minPos;
        
        if (boxSize.x < 0.001)
        {

            minPos.x -= 0.0005;

            maxPos.x += 0.0005;

        }

        if (boxSize.y < 0.001)
        {

            minPos.y -= 0.0005;

            maxPos.y += 0.0005;

        }

        if (boxSize.z < 0.001)
        {

            minPos.z -= 0.0005;

            maxPos.z += 0.0005;

        }
        
        //float3 padding = (boxSize < 0.001f) ? float3(0.0005f, 0.0005f, 0.0005f) : float3(0.0f, 0.0f, 0.0f);
        //minPos -= padding;
        //maxPos += padding;
        
        uint32_t aabbWriteIndex = info.aabbStartIndex + i;
        
        // 最終的なAABBデータをバッファへ書き込み
        RaytracingAABB aabb;
        aabb.minPositionX = minPos.x;
        aabb.minPositionY = minPos.y;
        aabb.minPositionZ = minPos.z;
        aabb.maxPositionX = maxPos.x;
        aabb.maxPositionY = maxPos.y;
        aabb.maxPositionZ = maxPos.z;

        g_OutAABBBuffer[aabbWriteIndex] = aabb;
        
        // レイトレの計算を軽くするために
        //SegmentData segData;
        //segData.v0_Index = v0_idx;
        //segData.v1_Index = v1_idx;
        //g_OutSegmentBuffer[aabbWriteIndex] = segData;
    }
}