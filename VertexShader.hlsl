#include "VSInclude.hlsli" // Declares VS constant buffer at slot 0

cbuffer CBuf : register(b1)
{
	matrix transform;
};

float4 main( float3 pos : POSITION ) : SV_POSITION
{
	return mul(float4(pos,1.0f),transform);
}