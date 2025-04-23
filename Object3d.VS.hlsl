//float4 main( float4 pos : POSITION ) : SV_POSITION
//{
//	return pos;
//}
struct TransformationMatrix
{
    float32_t4x4 WVP;
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
struct VertexShaderOutPut
{
    float32_t4 position : SV_POSITION;
};

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
};

VertexShaderOutPut main(VertexShaderInput input)
{
    VertexShaderOutPut output;
    output.position = mul(input.position,gTransformationMatrix.WVP);
    return output;
}