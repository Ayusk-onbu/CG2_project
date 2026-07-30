#include "Bone.hlsli"

struct PixelShaderOutPut
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutPut main(VertexShaderOutput input)
{
    PixelShaderOutPut output;

    // アルファ値が 0 の場合は描画をスキップ[cite: 16]
    if (input.color.a == 0.0)
    {
        discard;
    }

    // 簡易ディレクショナルライティング（右斜め上からの光）
    float32_t3 lightDirection = normalize(float32_t3(1.0, -1.0, 1.0));
    float cos = saturate(dot(input.normal, -lightDirection));

    // 環境光 (0.4) + 拡散光 (0.6 * NdotL)
    float32_t3 lighting = 0.4 + 0.6 * cos;

    // 頂点カラーに陰影を掛け合わせる
    output.color.rgb = input.color.rgb * lighting;
    output.color.a = input.color.a;

    return output;
}