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

// Hit Attribute Store <- Where Hit
struct HairAttribute
{
    float32_t2 uv; // x: 中心軸からの距離比率(0~1), y: 節の始点からの位置比率(0~1)
    float32_t3 normal; // ワールド空間における法線
};

struct Camera 
{
    float32_t4x4 inverseViewProj;
    float32_t3 position;
};

// TLAS
RaytracingAccelerationStructure SceneTLAS : register(t0);
// Hair Vertex Buffer
StructuredBuffer<StrandVertex> HairVertices : register(t1);
// シーンのDepth情報
Texture2D<float32_t> SceneDepth : register(t2);
// Segment Data Buffer
StructuredBuffer<SegmentData> HairSegments : register(t3);

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
    TraceRay(SceneTLAS, RAY_FLAG_NONE, 0xFF, 0, 1, 0, ray, payload);

  // ----------------------------------------
    
    // Drawing
    gOutput[pixelCoord] = float32_t4(payload.color, 1.0f);
}

////////////////////////
//
//  Intersection Shader
//
////////////////////////

struct Ray
{
    float3 Origin;
    float3 Direction;
};

bool RayCapsuleIntersectionTest(Ray ray, float3 p0, float3 p1, float radius, out float thit, out HairAttribute attr)
{
    thit = 0.0f;
    attr = (HairAttribute) 0;

    // 線分の方向と長さを計算
    float3 delta = ray.Origin - p0;
    float3 v = p1 - p0;
    float L = length(v);
    if (L < 0.0001f)
        return false; // 点潰れはスキップ
    float3 W = v / L; // 線分の単位方向ベクトル

    // 2直線の関係を解くための準備
    float dotDW = dot(ray.Direction, W);
    float denom = 1.0f - dotDW * dotDW; // 平行判定用の分母

    float t = 0.0f; // レイ上の距離
    float u = 0.0f; // 線分上の距離

    if (denom < 0.0001f)
    {
        // レイと線分が完全に平行な場合の処理
        t = -dot(delta, ray.Direction);
        u = t * dotDW + dot(delta, W);
    }
    else
    {
        // 通常のねじれの位置にある場合の数式解法
        float dotDD = dot(delta, ray.Direction);
        float dotDW_delta = dot(delta, W);

        t = (dotDW_delta * dotDW - dotDD) / denom;
        u = t * dotDW + dotDW_delta;
    }

    // 無限の線を「有限の長さ」にクリッピング(クランプ)する
    float uClamped = clamp(u, 0.0f, L);

    // 範囲外に飛び出ていたら、丸め込んだ端点を使ってレイ上の衝突位置 t を再計算
    if (uClamped != u)
    {
        u = uClamped;
        t = -dot(delta - u * W, ray.Direction);
    }

    // レイ上の最接近点と、線分上の最接近点のリアルな3D座標を計算
    float3 hitPointRay = ray.Origin + t * ray.Direction;
    float3 hitPointSegment = p0 + u * W;
    
    // 中心軸からのズレ（距離）ベクトル
    float3 distVec = hitPointRay - hitPointSegment;
    float distSq = dot(distVec, distVec);

    // 髪の毛の太さ（半径）の内側を通過しているか判定
    if (distSq <= radius * radius)
    {
        thit = t; // 衝突距離を確定
        
        // ClosestHitへ渡す属性を公式の手順に沿って計算
        attr.uv.x = sqrt(distSq) / radius; // 太さに対する当たった位置の比率
        attr.uv.y = u / L; // 線分の長さに対する当たった位置の比率 (0.0〜1.0)
        
        // ローカル空間での法線（芯から外側へ向かうベクトル）
        float distLengthSq = dot(distVec, distVec);
        if (distLengthSq > 0.000001f)
        {
            attr.normal = distVec / sqrt(distLengthSq); // normalizeと同等
        }
        else
        {
    // 中心を貫いた場合は、カメラ方向に向いた適当な法線を返す
            attr.normal = -ray.Direction;
        }
        
        return true;
    }

    return false;
}

[shader("intersection")]
void HairIntersectionShader()
{
    // 現在処理中のレイ情報を取得
    Ray ray;
    ray.Origin = ObjectRayOrigin();
    ray.Direction = ObjectRayDirection();

    // レイトレーシングエンジンが教えてくれる「当たったAABBの通し番号」
    uint aabbIndex = PrimitiveIndex();
    // セグメントバッファから、直接このAABBを構成する頂点インデックスを引く
    SegmentData seg = HairSegments[aabbIndex];
    // 頂点データを直接取得！
    StrandVertex v0 = HairVertices[seg.v0_Index];
    StrandVertex v1 = HairVertices[seg.v1_Index];
    
    float radius = v0.radius;

    float thit;
    HairAttribute attr;

    // 数学テストを実行：ここに改良の余地あり
    if (RayCapsuleIntersectionTest(ray, v0.position, v1.position, radius, thit, attr))
    {
        // 計算された交点 t が、現在のレイトレーシングの有効範囲内（一番手前）にあるか確認
        if (thit > RayTMin() && thit < RayTCurrent())
        {
            // ローカル空間で計算された法線を、世界の傾き（ワールド空間）に変換する
            // ※髪の毛のデータがBLASの時点で配置されている場合は ObjectToWorld で一発で変換
            attr.normal = normalize(mul((float3x3) ObjectToWorld3x4(), attr.normal));

            // GPUへ交差を報告！（これによりClosestHitが起動する）
            ReportHit(thit, 0, attr);
        }
    }
}

////////////////////////
//
//  ClosestHit Shader
//
////////////////////////
[shader("closesthit")]
void HairClosestHitShader(inout RayPayload payload, in HairAttribute attribute)
{
   // レイトレーシングエンジンが教えてくれる「当たったAABBの通し番号」
    uint32_t aabbIndex = PrimitiveIndex();
    // セグメントバッファから、直接このAABBを構成する頂点インデックスを引く
    SegmentData seg = HairSegments[aabbIndex];
    // 頂点データを直接取得
    StrandVertex v0 = HairVertices[seg.v0_Index];
    StrandVertex v1 = HairVertices[seg.v1_Index];
    
    // 当たった場所の比率（attribute.uv.y）を使って、C++からきた頂点カラーを滑らかに補間！
    float32_t3 baseColor = lerp(v0.color, v1.color, attribute.uv.y);

    // 固定値 (0,1,0) を廃止し、この節の「本物の傾きベクトル」を計算して接線 T とする！
    float32_t3 T = normalize(v1.position - v0.position);

    // 2. 各種ベクトルの準備
    float32_t3 L = normalize(float32_t3(1.0f, 1.0f, -1.0f)); // ライト方向
    float32_t3 V = normalize(-WorldRayDirection()); // 視線方向 (レイの逆向き)
    float32_t3 H = normalize(L + V); // ハーフベクトル（LとVの中間）

    // 3. Diffuse (拡散反射) - 既存の処理
    float32_t dotLT = dot(L, T);
    float32_t diffuse = sqrt(max(0.0f, 1.0f - dotLT * dotLT));

    // 4. Specular (鏡面反射: Kajiya-Kayモデル)
    float32_t dotTH = dot(T, H);
    // TとHのなす角のサイン波を求める (ピタゴラスの定理 sin^2 + cos^2 = 1 より)
    float32_t sinTH = sqrt(max(0.0f, 1.0f - dotTH * dotTH));
    
    // ハイライトの鋭さ（数値が大きいほど細く鋭いハイライトになる。例: 20〜100程度）
    float32_t shininess = 60.0f;
    float32_t specularIntensity = pow(sinTH, shininess);

    // ハイライトの色（とりあえず分かりやすいように純白）
    float32_t3 specularColor = float32_t3(1.0f, 1.0f, 1.0f);

    // 5. 環境光と最終合成
    float32_t3 envColor = float32_t3(0.05f, 0.05f, 0.05f);
    
    // DiffuseにSpecularを加算する
    //payload.color = (baseColor * diffuse) + (specularColor * specularIntensity) + envColor;
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
