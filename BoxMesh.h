#pragma once
#include "pch.h"
#include "Mesh.h"
#include "HLSLStructures.h"


class BoxMesh : public Mesh
{
private:
	struct BoxPositionColorVertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};

public:
	BoxMesh(std::shared_ptr<DeviceResources> deviceResources, bool fillCube = true);

private:
	void LoadTriangleVertices(); // For rendering a solid cube
	void LoadLineListVertices(); // For rendering the cube outline
};