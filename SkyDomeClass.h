#pragma once
#include "pch.h"

#include "DeviceResources.h"
#include "ChameleonException.h"

#include <memory>
#include <fstream>

class SkyDomeClass
{
private:
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VertexType
	{
		DirectX::XMFLOAT3 position;
	};

public:
	SkyDomeClass(std::shared_ptr<DeviceResources> deviceResources);
	~SkyDomeClass();

	void Shutdown();
	void Render();

	int GetIndexCount();
	DirectX::XMFLOAT4 GetApexColor();
	DirectX::XMFLOAT4 GetCenterColor();

private:
	void LoadSkyDomeModel(char*);
	void ReleaseSkyDomeModel();

	void InitializeBuffers();
	void ReleaseBuffers();
	void RenderBuffers();

private:
	std::shared_ptr<DeviceResources> m_deviceResources;

	ModelType* m_model;
	int m_vertexCount, m_indexCount;
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	DirectX::XMFLOAT4 m_apexColor, m_centerColor;
};