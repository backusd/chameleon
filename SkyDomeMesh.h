#pragma once
#include "pch.h"

#include "DeviceResources.h"
#include "DeviceResourcesException.h"
#include "Mesh.h"
#include "HLSLStructures.h"
#include "SkyDomeException.h"

#include <memory>
#include <fstream>
#include <string>
#include <sstream>


class SkyDomeMesh : public Mesh
{
private:
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	SkyDomeMesh(std::shared_ptr<DeviceResources> deviceResources);
	SkyDomeMesh(const SkyDomeMesh&) = delete;
	SkyDomeMesh& operator=(const SkyDomeMesh&) = delete;


	DirectX::XMFLOAT4 GetApexColor() { return m_apexColor; }
	DirectX::XMFLOAT4 GetCenterColor() { return m_centerColor; }


private:
	void LoadSkyDomeModel(std::string filename);
	void InitializeBuffers();

	std::vector<std::unique_ptr<ModelType>> m_model;
	int m_vertexCount;
	DirectX::XMFLOAT4 m_apexColor, m_centerColor;
};