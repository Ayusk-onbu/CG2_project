#include "Hair.hlsli"

struct SDFInfo
{
    float3 gridMin; // SDFバウンディングボックスの最小点
    float pad0;
    float3 gridMax; // SDFバウンディングボックスの最大点
    float pad1;
    uint3 resolution; // テクスチャ解像度 (例: 64, 64, 64)
    uint totalNodes;
};

ConstantBuffer<HairPhysicsConfig> gHairPhysicsConfig : register(b0);
ConstantBuffer<FrameConfig> gFrameConfig : register(b1);
ConstantBuffer<HairConfig> gHairConfig : register(b2);
ConstantBuffer<SDFInfo> g_SDFConfig : register(b3);
StructuredBuffer<GuideInfo> g_GuideInfos : register(t0);
Texture3D<float> g_SDFTexture : register(t1); // SDFManager から渡す SRV
RWStructuredBuffer<ControllerPoint> g_GuideBuffer : register(u0);
SamplerState g_LinearSampler : register(s0);

// -------------------------------------------------------------
// SDF から距離と押し出し法線（勾配）を計算する関数
// -------------------------------------------------------------
bool ResolveSDFCollision(inout float3 position, float hairRadius)
{
    // ワールド座標 P を SDF の 0.0～1.0 (UVW) 空間に変換
    float3 worldPos = position;
    float3 uvw = (worldPos - g_SDFConfig.gridMin) / (g_SDFConfig.gridMax - g_SDFConfig.gridMin);

    // SDF の範囲外なら衝突しない
    if (any(uvw < 0.0f) || any(uvw > 1.0f))
        return false;

    // 現在位置での SDF 値（距離 d）を取得
    float dist = g_SDFTexture.SampleLevel(g_LinearSampler, uvw, 0);

    // 髪の半径を考慮した衝突判定 (dist < hairRadius ならめり込んでいる)
    if (dist < hairRadius)
    {
        // 中心差分（Central Difference）で SDF の勾配（＝押し出し法線）を計算
        float delta = 1.0f / 512.0f; // 差分ステップ（空間解像度に応じて微調整）
        
        float dX1 = g_SDFTexture.SampleLevel(g_LinearSampler, uvw + float3(delta, 0, 0), 0);
        float dX2 = g_SDFTexture.SampleLevel(g_LinearSampler, uvw - float3(delta, 0, 0), 0);
        float dY1 = g_SDFTexture.SampleLevel(g_LinearSampler, uvw + float3(0, delta, 0), 0);
        float dY2 = g_SDFTexture.SampleLevel(g_LinearSampler, uvw - float3(0, delta, 0), 0);
        float dZ1 = g_SDFTexture.SampleLevel(g_LinearSampler, uvw + float3(0, 0, delta), 0);
        float dZ2 = g_SDFTexture.SampleLevel(g_LinearSampler, uvw - float3(0, 0, delta), 0);

        float3 normal = normalize(float3(dX1 - dX2, dY1 - dY2, dZ1 - dZ2));

        // 万が一 gradient がゼロ（真中心など）の場合は上方向に逃がす
        if (length(normal) < 0.001f)
            normal = float3(0, 1, 0);

        // ④ 押し出し量の計算（目標距離 hairRadius まで表面へ押し出す）
        float pushAmount = hairRadius - dist;
        position += normal * pushAmount;

        return true; // 衝突して押し出した
    }

    return false;
}

// -------------------------------------------------------------
// PBD 距離拘束（長さの補正）
// -------------------------------------------------------------
void ApplyConstraintSpring(uint parentIdx, uint childIdx)
{
    float3 posParent = g_GuideBuffer[parentIdx].position;
    float3 posChild = g_GuideBuffer[childIdx].position;
    float targetLength = g_GuideBuffer[parentIdx].nextToLength;

    float3 delta = posChild - posParent;
    float currentLength = length(delta);

    if (currentLength > 0.0001f)
    {
        float invLength = 1.0f / max(currentLength, 0.0001f);
        float diff = (currentLength - targetLength) * invLength;
        float3 correction = delta * diff * 0.5f;

        // 根元側が固定されている場合
        if (g_GuideBuffer[parentIdx].physicsWeight <= 0.0001f)
        {
            g_GuideBuffer[childIdx].position -= correction * 2.0f;
        }
        else
        {
            g_GuideBuffer[parentIdx].position += correction;
            g_GuideBuffer[childIdx].position -= correction;
        }
    }
}

// -------------------------------------------------------------
// PBD 距離拘束（根元固定・毛先押し出し）
// -------------------------------------------------------------
void ApplyConstraint(uint parentIdx, uint childIdx)
{
    float3 posParent = g_GuideBuffer[parentIdx].position;
    float3 posChild = g_GuideBuffer[childIdx].position;
    float targetLength = g_GuideBuffer[parentIdx].nextToLength;

    float3 delta = posChild - posParent;
    float currentLength = length(delta);

    if (currentLength > 0.0001f)
    {
        // 方向ベクトルを求めて、親から正確な targetLength の位置に子を配置する
        float3 dir = delta / currentLength;
        g_GuideBuffer[childIdx].position = posParent + dir * targetLength;
    }
}

[numthreads(64, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint guideIdx = dispatchThreadID.x;
    if (guideIdx >= gHairConfig.numGuides)
        return;
    
    GuideInfo info = g_GuideInfos[guideIdx];
    if (info.vertexCount < 2)
        return;

    float dt = gFrameConfig.deltaTime;
    
    // --------------------------------------------------
    // 事前準備: ガイドの全物理長 (totalLength) を計算する
    // --------------------------------------------------
    float totalLength = 0.0f;
    for (uint k = 0; k < info.vertexCount - 1; ++k)
    {
        totalLength += g_GuideBuffer[info.vertexStartIndex + k].nextToLength;
    }
    
    // --------------------------------------------------
    // 各頂点での風の計算
    // --------------------------------------------------
    float currentAccumulatedLength = 0.0f; // 根元からの累積距離

    // 移動量（長さ）と方向を安全に計算
    float moveLen = length(gFrameConfig.moveDirection);

    // 停止時（moveLenがほぼゼロ）のゼロ除算を回避
    float3 moveDir = (moveLen > 0.0001f) ? (gFrameConfig.moveDirection / moveLen) : float3(0, 0, 0);

    // 慣性係数（効き具合の調整用パラメータ）
    float inertiaStrength = 0.125f;

    // スピードに応じた慣性力を設定（過大になりすぎる場合は上限を設定）
    float3 inertia = -moveDir * min(moveLen, 0.5f) * inertiaStrength;
    
    // =========================================================
    // ステップ1: 外力（重力・風・慣性・復元力）の計算と位置更新
    // =========================================================
    for (uint i = 0; i < info.vertexCount; ++i)
    {
        uint currentIdx = info.vertexStartIndex + i;
        ControllerPoint p = g_GuideBuffer[currentIdx];

        // 根元（physicsWeight = 0）は固定のため物理計算をスキップ
        if (p.physicsWeight <= 0.0001f)
            continue;
        
        // 直前の関節の長さを足して「根元から何センチか」を更新
        // この処理の場合はここに書かないと二回分次の長さになってしまう、もしくは一個前の長さになってしまう
        currentAccumulatedLength += g_GuideBuffer[currentIdx - 1].nextToLength;
        
        // 根元(0.0) 〜 毛先(1.0) の相対位置を計算
        // ( vertexCount - 1 で割ることで、毛先がぴったり 1.0 になる )
        float normalizedDepth = (float) i / (float) (info.vertexCount - 1);

        // 重力
        float3 gravity = gFrameConfig.gravity * p.physicsWeight;

        // 風: 根元から毛先に向かって波が伝わるように (i を使用)

        // 正規化された位置を使って風の波（位相）を計算
        // 【実際の距離ベースの風】
        // 例: 1メートル（1.0）あたり 15 ラジアン進む波
        float wavePhase = gFrameConfig.time * 6.0f - currentAccumulatedLength * 15.0f;

        // 風の力（長い髪ほど毛先の影響を強くするなどの調整も totalLength で可能）
        float3 wind = gFrameConfig.windDirection * sin(wavePhase) * p.physicsWeight;
        
        // 復元力: 初期姿勢（homePosition）へ戻ろうとする力
        float3 toHome = (p.homePosition - p.position) * gHairPhysicsConfig.restoringForce * (1.0f - p.physicsWeight);

        // 外力の合算
        float3 totalForce = gravity + wind + toHome;

        // 速度・位置の加算（deltaTimeを反映）
        float3 velocity = (inertia + totalForce * dt) * gHairPhysicsConfig.damping;
        p.position += velocity;

        g_GuideBuffer[currentIdx] = p;
    }

    // =========================================================
    // PBD長さ拘束 & 衝突判定の反復ループ
    // =========================================================
    for (uint iteration = 0; iteration < 3; ++iteration)
    {
        //// --- 順方向パス (根元 -> 毛先) ---
        //for (uint j = 1; j < info.vertexCount; ++j)
        //{
        //    uint pIdx = info.vertexStartIndex + j - 1;
        //    uint cIdx = info.vertexStartIndex + j;

        //    // 長さの拘束（親の位置から正確な距離に子を配置）
        //    ApplyConstraint(pIdx, cIdx);

        //    // 子頂点の SDF 衝突判定 & 押し出し
        //    float3 pos = g_GuideBuffer[cIdx].position;
        //    float hairRadius = g_GuideBuffer[cIdx].radius;

        //    if (ResolveSDFCollision(pos, hairRadius))
        //    {
        //        // 体表へ押し出された位置を即座に確定
        //        g_GuideBuffer[cIdx].position = pos;
        //    }
        //}
        // --- 順方向パス (根元 -> 毛先) のみでOK ---
        for (uint j = 1; j < info.vertexCount; ++j)
        {
            uint pIdx = info.vertexStartIndex + j - 1;
            uint cIdx = info.vertexStartIndex + j;

            ApplyConstraint(pIdx, cIdx);
        }
        
        //// --- 順方向パス (根元 -> 毛先) ---
        //for (uint j = 1; j < info.vertexCount; ++j)
        //{
        //    uint pIdx = info.vertexStartIndex + j - 1;
        //    uint cIdx = info.vertexStartIndex + j;

        //    ApplyConstraint(pIdx, cIdx);
        //}

        //// --- 逆方向パス (毛先 -> 根元) ---
        //for (uint k = info.vertexCount - 1; k > 0; --k)
        //{
        //    uint pIdx = info.vertexStartIndex + k - 1;
        //    uint cIdx = info.vertexStartIndex + k;

        //    ApplyConstraint(pIdx, cIdx);
        //}
        
        // SDF 
        //for (uint c = 1; c < info.vertexCount; ++c) // 根元 (c=0) は固定なので c=1 から
        //{
        //    uint idx = info.vertexStartIndex + c;
        //    float3 pos = g_GuideBuffer[idx].position;

        //    // 髪の毛の太さ
        //    float hairRadius = g_GuideBuffer[idx].radius;

        //    if (ResolveSDFCollision(pos, hairRadius))
        //    {
        //       g_GuideBuffer[idx].position = pos;
        //    }
        //}
    }
}