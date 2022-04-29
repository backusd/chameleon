#include "PSInclude.hlsli" // Declares PS constant buffer at slot 0

Texture2D diffuseTexture : register(t0);
Texture2D specularTexture : register(t1);

SamplerState splr : register(s0);

// PS INPUT =================================================================================================
struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float4 positionWS : POS_WS;
    float3 normalWS : NORM_WS;
    float2 tex : TEXCOORD0;
};

// Material data =============================================================================================
struct _MyMaterial
{
    float4 Emissive; // 16 bytes
    //----------------------------------- (16 byte boundary)
    float4 Ambient; // 16 bytes
    //------------------------------------(16 byte boundary)
    float4 Diffuse; // 16 bytes
    //----------------------------------- (16 byte boundary)
    float4 Specular; // 16 bytes
    //----------------------------------- (16 byte boundary)
    float SpecularPower; // 4 bytes
    bool UseTexture; // 4 bytes
    float2 Padding; // 8 bytes
    //----------------------------------- (16 byte boundary)
}; // Total:               // 80 bytes ( 5 * 16 )

//cbuffer MyMaterialProperties : register(b1)
//{
//   _MyMaterial Material;
//};

cbuffer SpecularData : register(b1)
{
    float specularIntensity;
    float specularPower;
    float padding[2];
};


struct LightingResult
{
    float4 Diffuse;
    float4 Specular;
};

float4 DoDiffuse(MyLight light, float3 L, float3 N)
{
    float NdotL = max(0, dot(N, L));
    return light.Color * NdotL;
}

float4 DoSpecular(MyLight light, float3 V, float3 L, float3 N, float2 textureCoords)
{
    // Phong lighting.
    float3 R = normalize(reflect(-L, N));
    float RdotV = max(0, dot(R, V));

    // Blinn-Phong lighting
    float3 H = normalize(L + V);
    float NdotH = max(0, dot(N, H));

    
    
    float4 specularSample = specularTexture.Sample(splr, textureCoords);
    float specularPower = pow(2.0f, specularSample.a * 13.0f);
    return light.Color * pow(RdotV, specularPower);
    //return light.Color * pow(RdotV, Material.SpecularPower);
}

LightingResult DoDirectionalLight(MyLight light, float3 V, float4 P, float3 N, float2 textureCoords)
{
    LightingResult result;

    float3 L = -light.Direction.xyz;

    result.Diffuse = DoDiffuse(light, L, N);
    result.Specular = DoSpecular(light, V, L, N, textureCoords);

    return result;
}

float DoAttenuation(MyLight light, float d)
{
    return 1.0f / (light.ConstantAttenuation + light.LinearAttenuation * d + light.QuadraticAttenuation * d * d);
}

LightingResult DoPointLight(MyLight light, float3 V, float4 P, float3 N, float2 textureCoords)
{
    LightingResult result;

    float3 L = (light.Position - P).xyz;
    float distance = length(L);
    L = L / distance;

    float attenuation = DoAttenuation(light, distance);

    result.Diffuse = DoDiffuse(light, L, N) * attenuation;
    result.Specular = DoSpecular(light, V, L, N, textureCoords) * attenuation;

    return result;
}

float DoSpotCone(MyLight light, float3 L)
{
    float minCos = cos(light.SpotAngle);
    float maxCos = (minCos + 1.0f) / 2.0f;
    float cosAngle = dot(light.Direction.xyz, -L);
    return smoothstep(minCos, maxCos, cosAngle);
}

LightingResult DoSpotLight(MyLight light, float3 V, float4 P, float3 N, float2 textureCoords)
{
    LightingResult result;

    float3 L = (light.Position - P).xyz;
    float distance = length(L);
    L = L / distance;

    float attenuation = DoAttenuation(light, distance);
    float spotIntensity = DoSpotCone(light, L);

    result.Diffuse = DoDiffuse(light, L, N) * attenuation * spotIntensity;
    result.Specular = DoSpecular(light, V, L, N, textureCoords) * attenuation * spotIntensity;

    return result;
}

LightingResult ComputeLighting(float4 P, float3 N, float2 textureCoords)
{
    float3 V = normalize(EyePosition - P).xyz;

    LightingResult totalResult = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

    [unroll]
    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        LightingResult result = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

        if (!Lights[i].Enabled)
            continue;

        switch (Lights[i].LightType)
        {
            case DIRECTIONAL_LIGHT:
        {
                    result = DoDirectionalLight(Lights[i], V, P, N, textureCoords);
                }
                break;
            case POINT_LIGHT:
        {
                    result = DoPointLight(Lights[i], V, P, N, textureCoords);
                }
                break;
            case SPOT_LIGHT:
        {
                    result = DoSpotLight(Lights[i], V, P, N, textureCoords);
                }
                break;
        }
        totalResult.Diffuse += result.Diffuse;
        totalResult.Specular += result.Specular;
    }

    totalResult.Diffuse = saturate(totalResult.Diffuse);
    totalResult.Specular = saturate(totalResult.Specular);

    return totalResult;
}

// Pixel Shader main function
float4 main(PixelShaderInput input) : SV_TARGET
{
    
    
    LightingResult lit = ComputeLighting(input.positionWS, normalize(input.normalWS), input.tex);

    //float4 emissive = Material.Emissive;
    //float4 ambient = Material.Ambient * GlobalAmbient;
    //float4 diffuse = Material.Diffuse * lit.Diffuse;
    //float4 specular = Material.Specular * lit.Specular;
    //return emissive + ambient + diffuse + specular;
   
    float4 specular = specularIntensity * lit.Specular;
    //float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuseAmbient = diffuseTexture.Sample(splr, input.tex) * (lit.Diffuse + GlobalAmbient);
    return saturate(diffuseAmbient + specular);
}