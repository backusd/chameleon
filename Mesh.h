#pragma once
#include "pch.h"
#include "Bindable.h"


class Mesh : public Bindable
{
public:
	Mesh(std::shared_ptr<DeviceResources> deviceResources);
	virtual ~Mesh() = default;

	void Bind() override;
	unsigned int IndexCount() { return m_indexCount; }

protected:
	D3D11_PRIMITIVE_TOPOLOGY m_topology;
	DXGI_FORMAT m_indexFormat;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
	unsigned int m_indexCount;

	unsigned int m_sizeOfVertex;
};