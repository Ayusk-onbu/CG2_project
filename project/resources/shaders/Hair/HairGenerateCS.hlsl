#include "Hair.hlsli"

StructuredBuffer<ControllerPoint> g_InGuideBuffer : register(t0); // 物理演算後のガイド頂点群
StructuredBuffer<ChildStrand> g_InChildStrandBuffer : register(t1); // 子髪の設定バッファ
RWStructuredBuffer<StrandVertex> g_OutVertexBuffer : register(u0);
ConstantBuffer<HairMakeConfig> g_HairConfig : register(b0);
ConstantBuffer<HairConfig> gHairConfig : register(b1);

[numthreads(64, 1, 1)] // 64本ずつ並列で一気に生やす！
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint strandIdx = dispatchThreadID.x;
    
    if (strandIdx >= gHairConfig.numStrands) // もしくは相当する総ストランド数
    {
        return;
    }
    
    // バッファオーバーラン防止（念のため）
    // C++側からディスパッチされた総ストランド数を超えないようにします
    // ※今回は簡易的にスレッドインデックスをそのまま使います
    
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
        // 対応する親ガイドの頂点を取得
        uint currentGuideIdx = guideStartIndex + i;
        ControllerPoint gPoint = g_InGuideBuffer[currentGuideIdx];

        // 1. 接線（Tangent）の計算
        // 前後の頂点を使って、今髪の毛がどちらに向かって伸びているかのベクトルを求める
        float3 tangent = float3(0.0, 1.0, 0.0);
        if (i == 0)
        {
            // 根元なら、1番目の点へのベクトル
            tangent = normalize(g_InGuideBuffer[currentGuideIdx + 1].position - gPoint.position);
        }
        else
        {
            // それ以外なら、1つ前の点からのベクトル
            tangent = normalize(gPoint.position - g_InGuideBuffer[currentGuideIdx - 1].position);
        }

        // 2. 接線に垂直な、髪のローカル座標系（法線・従法線）を作る
        // C++側の Strands.cpp のロジックを忠実に再現
        float3 arbitraryUp = float3(0.0, 1.0, 0.0);
        if (abs(dot(tangent, arbitraryUp)) > 0.99)
        {
            arbitraryUp = float3(1.0, 0.0, 0.0); // 真上を向いていたらX軸に逃げる
        }
        
        float3 normal = normalize(cross(tangent, arbitraryUp)); // X軸方向
        float3 binormal = cross(tangent, normal); // Y軸方向

        // 3. 2Dオフセットを3D空間のベクトルに変換
        float3 offset3D = (normal * child.offset.x) + (binormal * child.offset.y);
        
        // 毛先に行くほどガイドに収束させる（クランプ効果）
        // 根元はオフセット通りに広がり、毛先(i=15)に近づくほど細くなる
        float progress = (float) i / (float) (gHairConfig.pointPerStrand - 1);
        float clump = lerp(1.0, (1.0 - child.clumpForce), progress);
        offset3D *= clump;

        // 4. 最終座標の決定（ガイドの座標 + オフセット）
        StrandVertex outVertex;
        outVertex.position = gPoint.position + offset3D;

        // 5. 太さ（半径）の計算（毛先テーパリング）
        // 毛先にいくほど髪の毛を細くする
        float tipTaper = 1.0 - (progress * 0.5); // 毛先は元の50%の太さに
        outVertex.radius = gPoint.radius * tipTaper * g_HairConfig.globalHairThickness;

        // 6. 色とパディング
        outVertex.color = gPoint.color;
        outVertex.padding = 0.0;

        // 7. 最終的なグローバルバッファ（UAV）へ書き込み！
        g_OutVertexBuffer[outStartIndex + i] = outVertex;
    }
}