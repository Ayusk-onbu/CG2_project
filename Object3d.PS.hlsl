//float4 main() : SV_TARGET
//{
//	return float4(1.0f, 1.0f, 1.0f, 1.0f);
//}

struct PixelShaderOutPut
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutPut main()
{
    PixelShaderOutPut output;
    output.color = float32_t4(1.0, 1.0, 1.0, 1.0);
    return output;
}