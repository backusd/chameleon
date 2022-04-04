#include "VSInclude.hlsli" // Declares VS constant buffer at slot 0


struct VertexShaderInput
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float4 positionWS : POS_WS;
    float3 normalWS : NORM_WS;
};


// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    float4 position = float4(input.position, 1.0f);

    output.position = mul(modelViewProjection, position); // Screen position
    output.positionWS = mul(model, position); // World space position
    output.normalWS = mul((float3x3) inverseTransposeModel, input.normal); // compute the world space normal

    return output;
}