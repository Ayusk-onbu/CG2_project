#include "Hair.hlsli"

ConstantBuffer<HairPhysicsConfig> gHairPhysicsConfig : register(b0);
ConstantBuffer<FrameConfig> gFrameConfig : register(b1);
RWStructuredBuffer<ControllerPoint> g_GuideBuffer : register(u0);
ConstantBuffer<HairConfig> gHairConfig : register(b2);

void ApplyConstraint(uint parentIdx, uint childIdx)
{
    float3 posParent = g_GuideBuffer[parentIdx].position;
    float3 posChild = g_GuideBuffer[childIdx].position;
    float targetLength = g_GuideBuffer[childIdx].nextToLength; // 本来の長さ

    // 現在の2点間のベクトルと距離
    float3 delta = posChild - posParent;
    // 大きさを算出
    float currentLength = length(delta);

    if (currentLength > 0.0001)
    {
        // 本来の長さとのズレを計算
        float invLength = 1.0 / max(currentLength, 0.0001);
        float diff = (currentLength - targetLength) * invLength;
                
        float3 correction = delta * diff * 0.5;

        // 根元側(j=1のとき)の親は完全に固定されているので動かさない
        if (parentIdx % gHairConfig.pointPerGuide == 0)
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

[numthreads(64, 1, 1)]
void main( uint3 dispatchThreadID : SV_DispatchThreadID )
{   
    
    // (例)Dispach(1,0,0)：0 ～ 63
    uint guideIdx = dispatchThreadID.x;
    
    // ガイドの総数以内かチェック
    if (guideIdx >= gHairConfig.numGuides)return;
    
    // ガイドの先頭頂点のバッファインデックスを計算
    // (例)Dispach(1,0,0)：0 * 16 = 0：1 -> 16：2 -> 32
    uint startIndex = guideIdx * gHairConfig.pointPerGuide;

    //   ========================================
    // 【 外部の力（重力・風）の適用と、位置更新 】
    //   ========================================
    // 根元(i=0)は固定なので、1 ～ 15までのループ
    for (uint i = 1; i < gHairConfig.pointPerGuide; ++i)
    {
        // 例：(16(n - 1)) =： 0 + (1 ～ 15) = (1 ～ 15)
        uint currentIdx = startIndex + i;
        // 変更するポジションの取得
        ControllerPoint p = g_GuideBuffer[currentIdx];
        // このフレームに動いた移動量を取得
        float3 vel = gFrameConfig.moveDirection;
        
        //   =============
        // 【 Forceの計算 】
        //   =============
        float3 force = float32_t3(0.0f, 0.0f, 0.0f);
        // 重力の計算（重力 + 簡易的な風のノイズ）
        force = gFrameConfig.gravity * p.physicsWeight;
        
        // 風の計算（時間と座標でサイン波を作ってなびかせる）
        float3 wind = gFrameConfig.windDirection * sin(gFrameConfig.time * 3.0 + p.position.x + p.position.y) * p.physicsWeight;
        force += wind;

        // 初期姿勢に戻ろうとする復元力
        float3 toHome = (p.homePosition - p.position) * gHairPhysicsConfig.restoringForce * (1.0 - p.physicsWeight);
        force += toHome;

        // オイラー法、またはVerlet積分で速度と位置を更新
        float32_t3 finalyForce = ((vel + force)/* * gFrameConfig.deltaTime*/) * gHairPhysicsConfig.damping;
        
        //   ==========================
        // 【 最終的な加わった力の計算 】
        //   ==========================
        p.position += finalyForce/* * gFrameConfig.deltaTime*/;
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
        // 順方向パス (根元 -> 毛先)
        for (uint j = 1; j < gHairConfig.pointPerGuide; ++j)
        {
            ApplyConstraint(startIndex + j - 1, startIndex + j);
        }

        // 逆方向パス (毛先 -> 根元)
        for (uint k = gHairConfig.pointPerGuide - 1; k > 0; --k)
        {
            ApplyConstraint(startIndex + k - 1, startIndex + k);
        }
    }
}