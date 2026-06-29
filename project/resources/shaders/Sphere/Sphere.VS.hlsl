#include "Sphere.hlsli"
struct SphereForGPU
{
    float32_t4x4 WVP;
    float32_t4x4 World;
    float32_t4 color;
};
StructuredBuffer<SphereForGPU> gSphere : register(t0);

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input,uint32_t instancedId : SV_InstanceID)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gSphere[instancedId].WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float32_t3x3) gSphere[instancedId].World));
    output.color = gSphere[instancedId].color;
    return output;
}