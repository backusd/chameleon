// A constant buffer that stores the three basic column-major matrices for composing geometry.
/*
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};
*/

cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix modelViewProjection;
	matrix inverseTransposeModel;
};


// Per-vertex data used as input to the vertex shader.
/*
struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 color : COLOR0;
};
*/
struct VertexShaderInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};


// Per-pixel color data passed through the pixel shader.
/*
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR0;
};
*/

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

	output.position = mul(modelViewProjection, position);                 // Screen position
	output.positionWS = mul(model, position);                               // World space position
	output.normalWS = mul((float3x3)inverseTransposeModel, input.normal); // compute the world space normal

	return output;
}