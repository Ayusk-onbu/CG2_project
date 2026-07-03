#include "Line.hlsli"
struct LineForGPU
{
    float32_t4 startPoint;
    float32_t4 endPoint;
    float32_t4 color;
};
StructuredBuffer<LineForGPU> gLine : register(t0);

VertexShaderOutput main(uint32_t vertexId : SV_VertexID,uint32_t instancedId : SV_InstanceID)
{
    VertexShaderOutput output;
    
    // 現在のインスタンスデータを取得
    LineForGPU instData = gLine[instancedId];

    // vertexIdが0なら始点、1なら終点
    float32_t4 worldPos = (vertexId == 0) ? instData.startPoint : instData.endPoint;

    output.position = worldPos;
    output.color = instData.color;

    return output;
}