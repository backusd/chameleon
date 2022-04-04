#pragma once
#include "pch.h"
#include "Mesh.h"
#include "ModelMeshException.h"

#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>


class ModelMesh : public Mesh
{
private:
	struct OBJVertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
		DirectX::XMFLOAT3 normal;
	};

public:
	ModelMesh(std::shared_ptr<DeviceResources> deviceResources, std::string filename);

private:
	void LoadOBJ(std::string filename);

};