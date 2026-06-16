#include "Hair.hlsli"

ConstantBuffer<HairPhysicsConfig> gHairPhysicsConfig : register(b0);
ConstantBuffer<FrameConfig> gFrameConfig : register(b1);
RWStructuredBuffer<ControllerPoint> g_GuideBuffer : register(u0);
ConstantBuffer<HairConfig> gHairConfig : register(b2);

[numthreads(64, 1, 1)]
void main( uint3 dispatchThreadID : SV_DispatchThreadID )
{   
    // このスレッドが担当する「ガイドのインデックス」
    uint guideIdx = dispatchThreadID.x;
 
    if (guideIdx >= gHairConfig.numGuides)
        return;
    
    // ガイドの先頭頂点のバッファインデックスを計算
    uint startIndex = guideIdx * gHairConfig.pointPerGuide;

    // -------------------------------------------------------------
    // ステップ1: 外部の力（重力・風）の適用と、仮の位置更新
    // -------------------------------------------------------------
    // 根元(i=0)は固定なので、i=1（2番目の点）から毛先までループ
    for (uint i = 1; i < gHairConfig.pointPerGuide; ++i)
    {
        uint currentIdx = startIndex + i;
        ControllerPoint p = g_GuideBuffer[currentIdx];
        float3 vel = gFrameConfig.moveDirection;

        // 1. 力の集計（重力 + 簡易的な風のノイズ）
        float3 force = gFrameConfig.gravity * p.physicsWeight;
        
        // 風の計算（時間と座標でサイン波を作ってなびかせる）
        float3 wind = gFrameConfig.windDirection * sin(gFrameConfig.time * 3.0 + p.position.x + p.position.y) * p.physicsWeight;
        force += wind;

        // 2. 初期姿勢（homePosition）に戻ろうとする復元力
        float3 toHome = (p.homePosition - p.position) * gHairPhysicsConfig.restoringForce * (1.0 - p.physicsWeight);
        force += toHome;

        // 3. オイラー法、またはVerlet積分で速度と位置を更新
        vel = (vel + force * gFrameConfig.deltaTime) * gHairPhysicsConfig.damping;
        p.position += vel * gFrameConfig.deltaTime;

        // 一旦バッファに書き戻す
        g_GuideBuffer[currentIdx] = p;
    }

    // -------------------------------------------------------------
    // ステップ2: 長さの拘束条件（Distance Constraint）
    // 髪の毛がゴムのように伸びるのを防ぐ、一番大事な処理！
    // -------------------------------------------------------------
    // 反復回数が多いほど硬い紐になる
    for (uint iteration = 0; iteration < 3; ++iteration)
    {
        for (uint j = 1; j < gHairConfig.pointPerGuide; ++j)
        {
            uint parentIdx = startIndex + j - 1; // 親（根元側）
            uint childIdx = startIndex + j; // 子（毛先側）

            float3 posParent = g_GuideBuffer[parentIdx].position;
            float3 posChild = g_GuideBuffer[childIdx].position;
            float targetLength = g_GuideBuffer[childIdx].nextToLength; // 本来の長さ

            // 現在の2点間のベクトルと距離
            float3 delta = posChild - posParent;
            float currentLength = length(delta);

            if (currentLength > 0.0001)
            {
                // 本来の長さとのズレを計算
                float diff = (currentLength - targetLength) / currentLength;
                float3 correction = delta * diff * 0.5;

                // 根元側(j=1のとき)の親は完全に固定されているので動かさない
                if (j == 1)
                {
                    // 子（毛先側）を100%引っ張って戻す
                    g_GuideBuffer[childIdx].position -= correction * 2.0;
                }
                else
                {
                    // お互いに 50% ずつ引き戻し合う
                    g_GuideBuffer[parentIdx].position += correction;
                    g_GuideBuffer[childIdx].position -= correction;
                }
            }
        }
    }
}