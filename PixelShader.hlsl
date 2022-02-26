#include "PSInclude.hlsli" // Declares PS constant buffer at slot 0

cbuffer CBuf : register(b1)
{
	float4 face_colors[6];
};

float4 main(uint tid : SV_PrimitiveID) : SV_Target
{
	return face_colors[tid / 2];
}