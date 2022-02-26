
// Model view projection buffer that pretty much every object will need to use
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix modelViewProjection;
	matrix inverseTransposeModel;
};