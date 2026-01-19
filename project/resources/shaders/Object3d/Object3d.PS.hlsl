#include "Object3d.hlsli"


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

struct MultiPointLightData
{
    uint numLights;
    float32_t3 padding;
    PointLight lights[10];
};

ConstantBuffer<MultiPointLightData> gMultiPointLight : register(b3);

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

struct MultiSpotLightData
{
    uint numLights;
    float32_t3 padding;
    SpotLight lights[10];
};

ConstantBuffer<MultiSpotLightData> gMultiSpotLight : register(b4);

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
    float32_t4 textureColor = gTexture.Sample(gSampler, transformUV.xy);
    
    float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    
    // Directional Light
    float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
    float NDotH = dot(normalize(input.normal), halfVector);
    float specularDirectionalLightPow = pow(saturate(NDotH), gMaterial.shininess);
    
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
       
        
        // Point Light
        // [ Total ] 
        float32_t3 totalPointDiffuse = float32_t3(0.0f, 0.0f, 0.0f);
        float32_t3 totalPointSpecular = float32_t3(0.0f, 0.0f, 0.0f);
        
        // [ Calculate ]
        for (uint32_t i = 0; i < gMultiPointLight.numLights; ++i)
        {
            PointLight light = gMultiPointLight.lights[i];
        
            float32_t distance = length(light.position - input.worldPosition);
            float32_t factor = pow(saturate(-distance / light.radius + 1.0f), light.decay);
    
            float32_t3 pointLightDirection = normalize(input.worldPosition - light.position);
            float32_t3 halfVectorPoint = normalize(-pointLightDirection + toEye);
            float NDotHPoint = dot(normalize(input.normal), halfVectorPoint);
            float specularPointLightPow = pow(saturate(NDotHPoint), gMaterial.shininess);
    
            totalPointDiffuse +=
            gMaterial.color.rgb * textureColor.rgb * light.color.rgb * 1.0f/*cos*/ * light.intensity * factor;
        
            totalPointSpecular +=
            light.color.rgb * light.intensity * specularPointLightPow * float32_t3(1.0f, 1.0f, 1.0f) * factor;
        }
        
        // Spot Light
        // [ Total ]
        float32_t3 totalSpotDiffuse = float32_t3(0.0f, 0.0f, 0.0f);
        float32_t3 totalSpotSpecular = float32_t3(0.0f, 0.0f, 0.0f);
        
        for (uint32_t i = 0; i < gMultiSpotLight.numLights; ++i)
        {
            SpotLight light = gMultiSpotLight.lights[i];
            
            float32_t attenuationDistance = length(light.position - input.worldPosition);
            float32_t attenuationFactor = pow(saturate(-attenuationDistance / light.distance + 1.0f), light.decay);
    
            float32_t3 spotLightDirectionOnSurface = normalize(input.worldPosition - light.position);
            float32_t3 halfVectorSpot = normalize(-spotLightDirectionOnSurface + toEye);
            float NDotHSpot = dot(normalize(input.normal), halfVectorSpot);
            float specularSpotLightPow = pow(saturate(NDotHSpot), gMaterial.shininess);
    
            float32_t cosAngle = dot(spotLightDirectionOnSurface, light.direction);
            float32_t falloffFactor = saturate((cosAngle - light.cosAngle) / (light.cosFalloffStart - light.cosAngle));
            
            totalSpotDiffuse +=
            gMaterial.color.rgb * textureColor.rgb * light.color.rgb * cos * light.intensity * attenuationFactor * falloffFactor;
            
            totalSpotSpecular +=
            light.color.rgb * light.intensity * specularSpotLightPow * float32_t3(1.0f, 1.0f, 1.0f) * attenuationFactor * falloffFactor;
        }
        
        output.color.rgb = diffuseDirectionalLight + specularDirectionalLight + totalPointDiffuse + totalPointSpecular + totalSpotDiffuse + totalSpotSpecular;
        output.color.a = gMaterial.color.a * textureColor.a;
    }else{
        output.color = gMaterial.color * textureColor;
    }
        return output;
}