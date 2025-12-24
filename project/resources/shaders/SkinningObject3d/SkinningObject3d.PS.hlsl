#include "SkinningObject3d.hlsli"

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
    float32_t shininess;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct DirectionalLight
{
    float32_t4 color; // ライトの色
    float32_t3 direction; // ライトの向き
    float32_t intensity; // 輝度
    int32_t shadowType; // シャドウの種類
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);


struct Camera
{
    float32_t3 worldPosition;
};
ConstantBuffer<Camera> gCamera : register(b2);

struct PointLight
{
    float32_t4 color;
    float32_t3 position;
    float intensity;
    float radius;
    float decay;
};
ConstantBuffer<PointLight> gPointLight : register(b3);

struct SpotLight
{
    float32_t4 color;
    float32_t3 position;
    float intensity;
    float32_t3 direction;
    float distance;
    float decay;
    float cosAngle;
    float cosFalloffStart;
};
ConstantBuffer<SpotLight> gSpotLight : register(b4);

struct PixelShaderOutPut
{
    float32_t4 color : SV_TARGET0;
};
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutPut main(VertexShaderOutput input)
{
    PixelShaderOutPut output;
    float4 transformUV = mul(float32_t4(input.texcoord,0.0f, 1.0f), gMaterial.uvTransform);
    //float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
    //float RdotE = dot(reflectLight, toEye);
    
    float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    
    // Directional Light
    float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
    float NDotH = dot(normalize(input.normal), halfVector);
    float specularDirectionalLightPow = pow(saturate(NDotH), gMaterial.shininess);
    
    // Point Light
    float32_t distance = length(gPointLight.position - input.worldPosition);
    float32_t factor = pow(saturate(-distance / gPointLight.radius + 1.0f), gPointLight.decay);
    
    float32_t3 pointLightDirection = normalize(input.worldPosition - gPointLight.position);
    float32_t3 halfVectorPoint = normalize(-pointLightDirection + toEye);
    float NDotHPoint = dot(normalize(input.normal), halfVectorPoint);
    float specularPointLightPow = pow(saturate(NDotHPoint), gMaterial.shininess);
    
    // Spot Light
    float32_t attenuationDistance = length(gSpotLight.position - input.worldPosition);
    float32_t attenuationFactor = pow(saturate(-attenuationDistance / gSpotLight.distance + 1.0f), gSpotLight.decay);
    
    float32_t3 spotLightDirectionOnSurface = normalize(input.worldPosition - gSpotLight.position);
    float32_t3 halfVectorSpot = normalize(-spotLightDirectionOnSurface + toEye);
    float NDotHSpot = dot(normalize(input.normal), halfVectorSpot);
    float specularSpotLightPow = pow(saturate(NDotHSpot), gMaterial.shininess);
    
    float32_t cosAngle = dot(spotLightDirectionOnSurface, gSpotLight.direction);
    float32_t falloffFactor = saturate((cosAngle - gSpotLight.cosAngle) / (gSpotLight.cosFalloffStart - gSpotLight.cosAngle));
    
    float32_t4 textureColor = gTexture.Sample(gSampler, transformUV.xy);
    
    if (gMaterial.enableLighting != 0)
    {
        float cos = 1.0f;
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        // 
        float32_t3 diffuseDirectionalLight =
        gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        //
        float32_t3 specularDirectionalLight =
        gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularDirectionalLightPow * float32_t3(1.0f, 1.0f, 1.0f);
       
        //
        float32_t3 diffusePointLight =
        gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * cos * gPointLight.intensity * factor;
        //
        float32_t3 specularPointLight =
        gPointLight.color.rgb * gPointLight.intensity * specularPointLightPow * float32_t3(1.0f, 1.0f, 1.0f) * factor;
        
        //
        float32_t3 diffuseSpotLight =
        gMaterial.color.rgb * textureColor.rgb * gSpotLight.color.rgb * cos * gSpotLight.intensity * attenuationFactor * falloffFactor;
        //
        float32_t3 specularSpotLight =
        gSpotLight.color.rgb * gSpotLight.intensity * specularSpotLightPow * float32_t3(1.0f, 1.0f, 1.0f) * attenuationFactor * falloffFactor;
        
        output.color.rgb = diffuseDirectionalLight + specularDirectionalLight + diffusePointLight + specularPointLight + diffuseSpotLight + specularSpotLight;
        output.color.a = gMaterial.color.a * textureColor.a;
    }else{
        output.color = gMaterial.color * textureColor;
    }
        return output;
}