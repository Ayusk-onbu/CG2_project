#include "Cylinder.hlsli"
struct CylinderForGPU
{
    float32_t4x4 WVP;
    float32_t4x4 World;
    float32_t4x4 uvTransform;
    float32_t4 color;
};
StructuredBuffer<CylinderForGPU> gCylinder : register(t0);

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input,uint32_t instancedId : SV_InstanceID)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gCylinder[instancedId].WVP);
    output.texcoord = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gCylinder[instancedId].uvTransform).xy;
    output.normal = normalize(mul(input.normal, (float32_t3x3) gCylinder[instancedId].World));
    output.color = gCylinder[instancedId].color;
    return output;
}