#include "Mesh.h"

Mesh::Mesh(std::shared_ptr<DeviceResources> deviceResources) : 
	Bindable(deviceResources),
	m_topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_vertexBuffer(nullptr),
	m_indexBuffer(nullptr),
	m_indexCount(0),
	m_vertexCount(0),
	m_sizeOfVertex(0),
	m_indexFormat(DXGI_FORMAT_R16_UINT),
	m_drawIndexed(true)
{
}

void Mesh::Bind()
{
	INFOMAN(m_deviceResources);
	ID3D11DeviceContext4* context = m_deviceResources->D3DDeviceContext();

	GFX_THROW_INFO_ONLY(
		context->IASetPrimitiveTopology(m_topology)
	);

	const UINT stride = m_sizeOfVertex;
	const UINT offset = 0u;
	GFX_THROW_INFO_ONLY(
		context->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &stride, &offset)
	);
	GFX_THROW_INFO_ONLY(
		context->IASetIndexBuffer(m_indexBuffer.Get(), m_indexFormat, 0u)
	);
}