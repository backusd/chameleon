#pragma once
#include "pch.h"

#include "DeviceResources.h"
#include "DeviceResourcesException.h"

#include <memory>


class Mesh
{
public:
	Mesh(std::shared_ptr<DeviceResources> deviceResources);
	virtual ~Mesh() = default;

	void PreparePipeline();
	unsigned int IndexCount() { return m_indexCount; }

protected:
	std::shared_ptr<DeviceResources> m_deviceResources;

	D3D11_PRIMITIVE_TOPOLOGY m_topology;
	DXGI_FORMAT m_indexFormat;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
	unsigned int m_indexCount;

	unsigned int m_sizeOfVertex;
};