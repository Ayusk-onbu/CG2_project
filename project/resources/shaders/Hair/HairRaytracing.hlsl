#include "Hair.hlsli"

///////////////
//
// RayGenShader
//
///////////////

// Hit Information Store <- What
struct RayPayload
{
    float32_t3 color;
};

// TLAS
RaytracingAccelerationStructure SceneTLAS : register(t0);
// Hair Vertex Buffer
//StructuredBuffer<StrandVertex> HairVertices : register(t1);
//StructuredBuffer<StrandVertex> HairFlatVertices : register(t1);
StructuredBuffer<StrandVertex> HairTriangleBuffer : register(t1);
// シーンのDepth情報
Texture2D<float32_t> SceneDepth : register(t2);
// Segment Data Buffer
//StructuredBuffer<SegmentData> HairSegments : register(t3);

// Camera Buffer
ConstantBuffer<Camera> gCamera : register(b0);

// UAV
RWTexture2D<float32_t4> gOutput : register(u0);


[shader("raygeneration")]
void MyRayGenShader()
{
  // ----------------------------------------
    
    // Get Pixel Coord and Dimension
    uint32_t2 pixelCoord = DispatchRaysIndex().xy;
    uint32_t2 resolution = DispatchRaysDimensions().xy;
    
    float32_t4 originalColor = gOutput[pixelCoord];
    
    float32_t depth = SceneDepth.Load(int32_t3(pixelCoord, 0)); // 0.0(手前) ～ 1.0(一番奥)
    
    // Transform to NDC -> (0 - 1) -> (0 - 2) -> (-1 - 1)
    float32_t2 ndc = (float32_t2(pixelCoord)) / float32_t2(resolution) * 2.0f - 1.0f;
    // Flip
    ndc.y = -ndc.y;
    
  // ----------------------------------------
    
    // Get World Position
    float32_t4 target = mul(float32_t4(ndc.x, ndc.y, 1.0f, 1.0f), gCamera.inverseViewProj);
    // Perspective Divide
    float32_t3 worldTarget = target.xyz / target.w;

  // ----------------------------------------
    
    // Get Ray Direction (gCamera -> target Vector)
    float32_t3 rayDir = normalize(worldTarget - gCamera.position);

  // ----------------------------------------
    
    float32_t2 uv = ((float32_t2) pixelCoord + 0.5f) / (float32_t2) resolution;
    
    // デプス値(Z)と画面のUV(X,Y)から、不透明オブジェクト（顔など）のワールド座標を復元する
    float32_t4 clipPos;
    clipPos.x = uv.x * 2.0f - 1.0f;
    clipPos.y = (1.0f - uv.y) * 2.0f - 1.0f; // Y軸の上下反転
    clipPos.z = depth; // 読み取った深度値
    clipPos.w = 1.0f;

    // カメラの逆行列を掛けてクリップ空間 ➔ ワールド空間へ
    float32_t4 worldPos = mul(clipPos, gCamera.inverseViewProj);
    worldPos.xyz /= worldPos.w; // w除算で正確なワールド座標の完成

    // カメラから不透明オブジェクトまでの距離を計算する
    float32_t3 viewVec = worldPos.xyz - gCamera.position;
    float32_t maxDistance = length(viewVec); // これがレイを飛ばしていい限界の長さ！
    
  // ----------------------------------------
    
    // Setup Ray
    RayDesc ray;
    ray.Origin = gCamera.position;
    ray.Direction = rayDir;
    ray.TMin = 0.001f; // Collision Distance Min
    //ray.TMax = 10000.0f;// Collision Distance Max
    ray.TMax = maxDistance; // Collision Distance Max
    
  // ----------------------------------------
    
    // Setup Payload
    RayPayload payload = { originalColor.xyz };
    //RayPayload payload = { float32_t3(0.0f, 0.0f, 1.0f) };
  
  // ----------------------------------------
    
    // Ray Trace
    TraceRay(SceneTLAS, RAY_FLAG_FORCE_OPAQUE, 0xFF, 0, 1, 0, ray, payload);

  // ----------------------------------------
    
    // Drawing
    gOutput[pixelCoord] = float32_t4(payload.color, 1.0f);
}

////////////////////////
//
//  ClosestHit Shader
//
////////////////////////
[shader("closesthit")]
void HairClosestHitShader(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attr)
{
   // 当たった三角形の通し番号を取得
    uint triIdx = PrimitiveIndex();
    
    // この三角形を構成する3つの頂点をロード（キャッシュが効くので爆速！）
    StrandVertex v0 = HairTriangleBuffer[triIdx * 3 + 0];
    StrandVertex v1 = HairTriangleBuffer[triIdx * 3 + 1];
    StrandVertex v2 = HairTriangleBuffer[triIdx * 3 + 2];

    // 💡重心座標（barycentrics）を使って、色を滑らかにブレンド
    float3 bary = float3(1.0 - attr.barycentrics.x - attr.barycentrics.y, attr.barycentrics.x, attr.barycentrics.y);
    float3 baseColor = v0.color * bary.x + v1.color * bary.y + v2.color * bary.z;

    // 💡 1. 芯からの距離（-1.0 ～ 1.0）を重心座標で補間して復元！
    float u = v0.padding * bary.x + v1.padding * bary.y + v2.padding * bary.z;

    float3 T = normalize(v2.position - v0.position);
    float3 V = normalize(-WorldRayDirection());

    // 💡 2. 視線(V)と接線(T)から、画面にとって「真横」のベクトル(R)を作る
    float3 R = normalize(cross(T, V));

    // 💡 3. 板ポリゴンなのに「円柱の丸み」を持った法線を生成！
    // 横方向(R)に u の割合、手前方向(V)に円の高さ( ピタゴラスの定理: sqrt(1-u^2) ) の割合を混ぜる
    float z = sqrt(max(0.0f, 1.0f - u * u));
    float3 N = normalize(R * u + V * z); // これが本物の円柱の法線になる！

    // --- ここから下は今までと同じ ---
    float3 L = normalize(float3(1.0f, 1.0f, -1.0f));
    float3 H = normalize(L + V);
    
    // ① Diffuse (拡散反射) 
    float dotLT = dot(L, T);
    float diffuse = saturate(sqrt(max(0.0f, 1.0f - dotLT * dotLT)) * 0.7f + 0.3f);

    // ② Dual-Specular (天使の輪)
    // 💡さっき作った「丸みのある法線(N)」を使うことで、ハイライトが円柱状に美しく乗る！
    float3 T1 = normalize(T + N * 0.1f);
    float3 T2 = normalize(T - N * 0.05f);

    float dotT1H = dot(T1, H);
    float spec1 = pow(sqrt(max(0.0f, 1.0f - dotT1H * dotT1H)), 80.0f);
    float3 specularColor1 = float3(1.0f, 1.0f, 1.0f) * spec1 * 0.6f;

    float dotT2H = dot(T2, H);
    float spec2 = pow(sqrt(max(0.0f, 1.0f - dotT2H * dotT2H)), 30.0f);
    float3 specularColor2 = baseColor * spec2 * 0.4f;

    float3 ambient = baseColor * 0.05f;
    //payload.color = (baseColor * diffuse) + specularColor1 + specularColor2 + ambient;
    payload.color = baseColor;

}

////////////////////////
//
//     Miss Shader
//
////////////////////////
[shader("miss")]
void MyMissShader(inout RayPayload payload)
{
    // 背景はゲームの画像なので特に何もしない
}
