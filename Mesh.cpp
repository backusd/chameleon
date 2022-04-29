#include "Mesh.h"

using DirectX::XMVECTOR;
using DirectX::XMMATRIX;
using DirectX::XMFLOAT3;

Mesh::Mesh(std::shared_ptr<DeviceResources> deviceResources) : 
	Bindable(deviceResources),
	m_topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_vertexBuffer(nullptr),
	m_indexBuffer(nullptr),
	m_indexCount(0),
	m_vertexCount(0),
	m_sizeOfVertex(0),
	m_indexFormat(DXGI_FORMAT_R16_UINT),
	m_drawIndexed(true),
	m_boundingBox(nullptr)
{
}

void Mesh::Bind()
{
	INFOMAN(m_deviceResources);

	// First, bind any of the attached bindables
	for (std::shared_ptr<Bindable> bindable : m_bindables)
		bindable->Bind();

	// Next, bind the vertex and index buffers
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

bool Mesh::RayIntersectionTest(const XMVECTOR& rayOrigin, const XMVECTOR& rayDirection, float& distance)
{
	// First do a test for the bounding box. If that was not a hit, then just return false
	if (!m_boundingBox->RayIntersectionTest(rayOrigin, rayDirection, distance))
		return false;

	// Loop over each triangle and test for intersection
	float dist;
	bool found = false;
	XMVECTOR v1, v2, v3;
	distance = FLT_MAX;

	assert(m_indices.size() % 3 == 0);
	for (unsigned int iii = 0; iii < m_indices.size(); iii+=3)
	{
		v1 = m_positions[m_indices[iii]];
		v2 = m_positions[m_indices[iii + 1]];
		v3 = m_positions[m_indices[iii + 2]];

		// If there is an intersection, update the shortest distance
		if (DirectX::TriangleTests::Intersects(rayOrigin, rayDirection, v1, v2, v3, dist))
		{
			distance = std::min(distance, dist);
			found = true;
		}
	}

	return found;
}

void Mesh::GetBoundingBoxPositionsWithTransformation(const XMMATRIX& tranformation, std::vector<XMVECTOR>& positions)
{
	m_boundingBox->GetBoundingBoxPositionsWithTransformation(tranformation, positions);
}



#ifndef NDEBUG
void Mesh::DrawBoundingBox(const XMMATRIX& modelMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix)
{
	if (m_boundingBox != nullptr)
		m_boundingBox->Draw(modelMatrix, viewMatrix, projectionMatrix);
}
#endif