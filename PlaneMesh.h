#pragma once
#include "pch.h"
#include "Mesh.h"
#include "HLSLStructures.h"


class PlaneMesh : public Mesh
{
private:
	struct PlaneVertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
		DirectX::XMFLOAT3 normal;
	};

public:
	PlaneMesh(std::shared_ptr<DeviceResources> deviceResources);

private:
	void LoadVertices();
};
