//float4 main() : SV_TARGET
//{
//	return float4(1.0f, 1.0f, 1.0f, 1.0f);
//}



struct Material
{
    float32_t4 color;
};

ConstantBuffer<Material> gMaterial : register(b0);
struct PixelShaderOutPut
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutPut main()
{
    PixelShaderOutPut output;
    output.color = gMaterial.color;
    return output;
}