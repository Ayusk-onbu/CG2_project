#include "Fullscreen.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

static const float32_t PI = 3.141592653589f;

float gausu(float x, float y, float sigma){
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(denominator);
    // exp = e^exponent
}

static const float32_t2 kIndex3x3[3][3] =
{
    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } },
    { { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }
};

PixelShaderOutput main(VertexShaderOutput input)
{
    float32_t weight = 0.0f;
    float32_t kernel3x3[3][3];
    for (int32_t x = 0; x < 3; ++x){
        for (int32_t y = 0; y < 3; ++y){
            kernel3x3[x][y] = gausu(kIndex3x3[x][y].x, kIndex3x3[x][y].y, 2.0f);
            weight += kernel3x3[x][y];
        }
    }
    // Get dimensions of texture
    uint32_t width, height;
    gTexture.GetDimensions(width, height);
    float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height)); // rcp = reverse
    
    PixelShaderOutput output;
    // Initialize output color
    output.color.rgb = float32_t3(0.0f, 0.0f, 0.0f);
    output.color.a = 1.0f;
    // Drawing with 3x3 kernel
    for (int32_t x = 0; x < 3; ++x){
        for (int32_t y = 0; y < 3; ++y){
            // Get texcoord
            float32_t2 texcoord = input.texcoord + kIndex3x3[x][y] * uvStepSize;
            // Get Color
            float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
            // color multiply 1/9 after add
            output.color.rgb += fetchColor * kernel3x3[x][y];
        }
    }
    
    // Normalize output Color
    output.color.rgb *= rcp(weight);
    
    return output;
}