#include "Mesh.h"

Mesh::Mesh(std::shared_ptr<DeviceResources> deviceResources) : 
	m_deviceResources(deviceResources),
	m_topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_vertexBuffer(nullptr),
	m_indexBuffer(nullptr),
	m_indexCount(0),
	m_sizeOfVertex(0),
	m_indexFormat(DXGI_FORMAT_R16_UINT)
{
}

void Mesh::PreparePipeline()
{
	// Prepare the pipeline by doing the following: (NOTE: This should only be run once before rendering the mesh
	//												 potentially many times)
	//		IASetPrimitiveTopology
	//		IASetVertexBuffers
	//		IASetIndexBuffer

	ID3D11DeviceContext4* context = m_deviceResources->D3DDeviceContext();

	context->IASetPrimitiveTopology(m_topology);

	const UINT stride = m_sizeOfVertex;
	const UINT offset = 0u;
	context->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &stride, &offset);

	context->IASetIndexBuffer(m_indexBuffer.Get(), m_indexFormat, 0u);
}