// --- 1. データ構造の定義 (C++側と完全に一致させる) ---
struct StrandVertex
{
    float3 position;
    float radius;
    float3 color;
};

struct RayPayload
{
    float3 color;
};

// --- 2. バッファのバインド ---
// あなたがビルドしたTLAS（世界）
RaytracingAccelerationStructure SceneTLAS : register(t0);

// さっき作った髪の毛の頂点バッファ（SRV）
StructuredBuffer<StrandVertex> HairVertices : register(t1);


// --- 3. 交差シェーダー (Intersection Shader) ---
// 箱（AABB）の中にレイが入ってきた時に、自動的に呼び出される関数
[shader("intersection")]
void HairIntersectionShader()
{
    // 現在処理しているAABBのID = 髪の毛の「セグメント（線分）ID」
    uint segmentId = PrimitiveIndex();
    
    // 1本あたり5点なので、セグメントIDから頂点インデックスを復元
    // ※C++側の「pointsPerStrand = 5」に合わせる
    uint strandsId = segmentId / 4;
    uint localSegId = segmentId % 4;
    uint vertexIdx0 = strandsId * 5 + localSegId;
    uint vertexIdx1 = vertexIdx0 + 1;

    // 線分の始点と終点を取得
    float3 p0 = HairVertices[vertexIdx0].position;
    float3 p1 = HairVertices[vertexIdx1].position;
    float radius = HairVertices[vertexIdx0].radius; // 簡易的に始点の太さを採用

    // レイの情報を取得
    float3 rayOrigin = ObjectRayOrigin();
    float3 rayDir = ObjectRayDirection();

    // --- 【超簡易版】レイと線分（円柱）の交差判定 ---
    // 本来はガチの数学（2次方程式）を解きますが、まずは「一番近い距離」での簡易判定
    float3 v = p1 - p0;
    float3 w = rayOrigin - p0;

    float a = dot(v, v);
    float b = dot(v, rayDir);
    float c = dot(rayDir, rayDir);
    float d = dot(v, w);
    float e = dot(rayDir, w);

    float denom = a * c - b * b;
    if (abs(denom) < 1e-5)
        return; // 平行ならパス

    // レイ上の最も近い位置 t と、線分上の最も近い位置 t_seg を計算
    float t = (a * e - b * d) / denom;
    float t_seg = clamp((b * e - c * d) / denom, 0.0f, 1.0f);

    // 最も近づいた2点間の距離の2乗を計算
    float3 closestOnRay = rayOrigin + t * rayDir;
    float3 closestOnSeg = p0 + t_seg * v;
    float distSq = dot(closestOnRay - closestOnSeg, closestOnRay - closestOnSeg);

    // 半径以内（衝突）かつ、現在のレイの有効範囲内（RayTMin〜RayTMax）にあればヒット報告！
    if (distSq < (radius * radius) && t > RayTCurrent() && t < RayTMax())
    {
        
        // 属性データ（アトリビュート）として、線分のどの位置に当たったか（0.0〜1.0）を保存できる
        float2 attr = float2(t_seg, 0.0f);
        
        // GPUに「当たったぞ！」と報告。これでClosest Hitが呼び出される
        ReportHit(t, 0, attr);
    }
}


// --- 4. 最近接ヒットシェーダー (Closest Hit Shader) ---
// 交差シェーダーが「当たった！」と報告した中で、カメラに一番近い場所で1回だけ呼ばれる
[shader("closesthit")]
void HairClosestHitShader(inout RayPayload payload, in float2 attr : HIT_KIND)
{
    uint segmentId = PrimitiveIndex();
    uint strandsId = segmentId / 4;
    uint vertexIdx0 = strandsId * 5 + (segmentId % 4);
    
    // 始点の色を取得（将来的には attr.x を使ってグラデーションできる）
    float3 hairColor = HairVertices[vertexIdx0].color;

    // 簡易的な陰影（レイの方向と適当に向きを合わせてみる）
    float lighting = saturate(dot(-WorldRayDirection(), float3(0, 1, 0))) * 0.5 + 0.5;

    // 最終的な色をペイロード（戻り値のようなもの）に書き込む
    payload.color = hairColor * lighting;
}