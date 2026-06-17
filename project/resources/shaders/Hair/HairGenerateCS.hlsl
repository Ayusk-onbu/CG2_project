#include "Hair.hlsli"

StructuredBuffer<ControllerPoint> g_InGuideBuffer : register(t0); // 物理演算後のガイド頂点群
StructuredBuffer<ChildStrand> g_InChildStrandBuffer : register(t1); // 子髪の設定バッファ
RWStructuredBuffer<StrandVertex> g_OutVertexBuffer : register(u0);
ConstantBuffer<HairMakeConfig> g_HairMakeConfig : register(b0);
ConstantBuffer<HairConfig> gHairConfig : register(b1);

float3 CatmullRom(float3 p0, float3 p1, float3 p2, float3 p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;

    return 0.5 * (
        (2.0 * p1) +
        (-p0 + p2) * t +
        (2.0 * p0 - 5.0 * p1 + 4.0 * p2 - p3) * t2 +
        (-p0 + 3.0 * p1 - 3.0 * p2 + p3) * t3
    );
}

// Catmull-Romの微分による接線（Tangent）の計算
float3 CatmullRomDerivative(float3 p0, float3 p1, float3 p2, float3 p3, float t)
{
    float t2 = t * t;

    return 0.5 * (
        (-p0 + p2) +
        2.0 * (2.0 * p0 - 5.0 * p1 + 4.0 * p2 - p3) * t +
        3.0 * (-p0 + 3.0 * p1 - 3.0 * p2 + p3) * t2
    );
}

[numthreads(64, 1, 1)] // 64本ずつ並列で一気に生やす！
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint strandIdx = dispatchThreadID.x;
    
     // 総ストランド数
    if (strandIdx >= gHairConfig.numStrands)return;
    
    // この子髪の遺伝子（設定）をロード
    ChildStrand child = g_InChildStrandBuffer[strandIdx];
    
    // この子髪の書き込み先（先頭インデックス）を計算
    uint outStartIndex = strandIdx * gHairConfig.pointPerStrand;
    
    // 代表する親ガイドの先頭インデックス
    uint guideStartIndex = child.parentGuideIds[0] * gHairConfig.pointPerGuide;

    // -------------------------------------------------------------
    // 根元から毛先まで、1本分の全頂点を生やすループ
    // -------------------------------------------------------------
    for (uint i = 0; i < gHairConfig.pointPerStrand; ++i)
    {
       // 1. ストランド全体での進行割合 (0.0 ～ 1.0)
        float progress = (float) i / (float) (gHairConfig.pointPerStrand - 1);

        // 2. ガイド側の仮想インデックス (例: 16頂点なら 0.0 ～ 15.0)
        float guideFloatIdx = progress * (float) (gHairConfig.pointPerGuide - 1);

        // 3. Catmull-Romに必要な 4つのインデックス を計算
        // idx1 と idx2 が現在補間しようとしている区間
        int idx1 = (int) floor(guideFloatIdx);
        int idx2 = min(idx1 + 1, gHairConfig.pointPerGuide - 1);
        
        // idx0 と idx3 は曲線を滑らかにするための前後の点（範囲外に出ないようClamp）
        int idx0 = max(idx1 - 1, 0);
        int idx3 = min(idx2 + 1, gHairConfig.pointPerGuide - 1);

        // 4. 小数点以下を取り出してブレンド率 (t) にする
        float t = guideFloatIdx - (float) idx1;

        // 5. 4つのガイド頂点データを取得
        ControllerPoint gPoint0 = g_InGuideBuffer[guideStartIndex + idx0];
        ControllerPoint gPoint1 = g_InGuideBuffer[guideStartIndex + idx1];
        ControllerPoint gPoint2 = g_InGuideBuffer[guideStartIndex + idx2];
        ControllerPoint gPoint3 = g_InGuideBuffer[guideStartIndex + idx3];

        // 6. Catmull-Romで滑らかな「位置」と「接線」を計算
        float3 currentPos = CatmullRom(gPoint0.position, gPoint1.position, gPoint2.position, gPoint3.position, t);
        
        // 接線は微分関数から求めて正規化するだけ！（非常に正確です）
        float3 tangent = normalize(CatmullRomDerivative(gPoint0.position, gPoint1.position, gPoint2.position, gPoint3.position, t));

        // ※半径や色は直線補間(Lerp)で十分なので処理の軽いこちらを使う
        float currentRadius = lerp(gPoint1.radius, gPoint2.radius, t);
        float3 currentColor = lerp(gPoint1.color, gPoint2.color, t);

        // --- 7. 以降は既存のオフセット計算 ---
        float3 arbitraryUp = float3(0.0, 1.0, 0.0);
        if (abs(dot(tangent, arbitraryUp)) > 0.99)
        {
            arbitraryUp = float3(1.0, 0.0, 0.0);
        }
        
        float3 normal = normalize(cross(tangent, arbitraryUp));
        float3 binormal = cross(tangent, normal);

        float3 offset3D = (normal * child.offset.x) + (binormal * child.offset.y);
        
        float clump = lerp(1.0, (1.0 - child.clumpForce), progress);
        offset3D *= clump;

        // 最終座標
        StrandVertex outVertex;
        outVertex.position = currentPos + offset3D;

        float tipTaper = 1.0 - (progress * 0.5);
        outVertex.radius = currentRadius * tipTaper * g_HairMakeConfig.globalHairThickness;

        outVertex.color = currentColor;
        outVertex.padding = 0.0;

        g_OutVertexBuffer[outStartIndex + i] = outVertex;
    }
}