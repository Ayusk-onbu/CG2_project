#ifndef MY_COMMON_INCLUDED
#define MY_COMMON_INCLUDED

struct PerView
{
    float32_t4x4 viewProjection;
    float32_t4x4 billboardMatrix;
};

struct PerFrame
{
    float32_t time;
    float32_t deltaTime;
};

float rand3dTo1d(float3 value, float3 dotDir = float3(12.9898, 78.233, 37.719))
{
    //make value smaller to avoid artefacts
    float3 smallValue = sin(value);
    //get scalar value from 3d vector
    float random = dot(smallValue, dotDir);
    //make value more random by making it bigger and then taking teh factional part
    random = frac(sin(random) * 143758.5453);
    return random;
}

float3 rand3dTo3d(float3 value)
{
    return float3(
        rand3dTo1d(value, float3(12.989, 78.233, 37.719)),
        rand3dTo1d(value, float3(39.346, 11.135, 83.155)),
        rand3dTo1d(value, float3(73.156, 52.235, 09.151))
    );
}

class RandomGenerator
{

    float32_t3 seed;
    float32_t3 Generate3d()
    {
        seed = rand3dTo3d(seed);
        return seed;
    }

    float32_t Generate1d()
    {
        float32_t result = rand3dTo1d(seed);
        seed.x = result;
        return result;
    }
};



// 【共通関数】法線テクスチャと各種情報から、ワールド空間の法線を計算する
float32_t3 CalculateWorldNormal(float32_t3 normalMapColor, float32_t3 worldPosition, float32_t2 texcoord, float32_t3 vertexNormal)
{
    // 法線マップの色を [-1, 1] のベクトルに変換
    float32_t3 normalMap = normalMapColor * 2.0f - 1.0f;
    
    // ddx/ddy を使ってその場でTangent(接線)を計算
    float32_t3 p_dx = ddx(worldPosition);
    float32_t3 p_dy = ddy(worldPosition);
    float32_t2 tc_dx = ddx(texcoord);
    float32_t2 tc_dy = ddy(texcoord);

    // UVの傾きから接空間の軸を割り出す
    float32_t3 tangent = normalize(p_dx * tc_dy.y - p_dy * tc_dx.y);
    float32_t3 normal = normalize(vertexNormal);
    float32_t3 binormal = normalize(cross(normal, tangent));
    
    // TBN行列（接空間からワールド空間への変換行列）を作成
    float32_t3x3 TBN = float32_t3x3(tangent, binormal, normal);
    
    // ワールド空間へ変換して正規化して返す
    return normalize(mul(normalMap, TBN));
}
#endif// MY_COMMON_INCLUDED
// pragma onceでもいいらしいがかっこいいからこっち