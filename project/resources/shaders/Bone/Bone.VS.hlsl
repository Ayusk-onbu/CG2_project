#include "Bone.hlsli"
struct BoneForGPU
{
    float32_t4x4 WVP;
    float32_t4x4 World;
    float32_t4 color;
};

StructuredBuffer<BoneForGPU> gBone : register(t0);

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input, uint32_t instancedId : SV_InstanceID)
{
    VertexShaderOutput output;
    // ローカル座標からスクリーン座標へ変換
    output.position = mul(input.position, gBone[instancedId].WVP);
    output.texcoord = input.texcoord;
    // 法線のワールド空間への変換
    output.normal = normalize(mul(input.normal, (float32_t3x3) gBone[instancedId].World));
    // インスタンスカラーの設定
    output.color = gBone[instancedId].color;
    return output;
}