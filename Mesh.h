#pragma once
#include "pch.h"
#include "Bindable.h"


class Mesh : public Bindable
{
public:
	Mesh(std::shared_ptr<DeviceResources> deviceResources);
	virtual ~Mesh() = default;

	virtual void Bind() override;
	unsigned int IndexCount() { return m_indexCount; }
	unsigned int VertexCount() { return m_vertexCount; }

	bool DrawIndexed() { return m_drawIndexed; }

protected:
	D3D11_PRIMITIVE_TOPOLOGY m_topology;
	DXGI_FORMAT m_indexFormat;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
	unsigned int m_indexCount;
	unsigned int m_vertexCount;

	unsigned int m_sizeOfVertex;

	bool m_drawIndexed;
};