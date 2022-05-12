#pragma once
#include "pch.h"
#include "Bindable.h"
#include "BoundingBox.h"

#include <vector>

struct OBJVertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texture;
	DirectX::XMFLOAT3 normal;
};

class Mesh : public Bindable
{
public:
	Mesh(std::shared_ptr<DeviceResources> deviceResources);
	// virtual ~Mesh() = default;

	template <typename T, typename A>
	void LoadBuffers(std::vector<T, A>& vertices, std::vector<unsigned short>& indices);

	// void AddBindable(std::shared_ptr<Bindable> bindable) { m_bindables.push_back(bindable); }
	virtual void Bind() override;
	unsigned int IndexCount() { return m_indexCount; }
	unsigned int VertexCount() { return m_vertexCount; }

	bool DrawIndexed() { return m_drawIndexed; }

	bool RayIntersectionTest(const DirectX::XMVECTOR& rayOrigin, const DirectX::XMVECTOR& rayDirection, float& distance);
	void GetBoundingBoxPositionsWithTransformation(const DirectX::XMMATRIX& tranformation, std::vector<DirectX::XMVECTOR>& positions);

	void SetMaterialIndex(unsigned int index) { m_materialIndex = index; }
	unsigned int GetMaterialIndex() { return m_materialIndex; }

protected:
	std::string m_name;
	unsigned int m_materialIndex;

	D3D11_PRIMITIVE_TOPOLOGY m_topology;
	DXGI_FORMAT m_indexFormat;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
	unsigned int m_indexCount;
	unsigned int m_vertexCount;

	unsigned int m_sizeOfVertex;

	bool m_drawIndexed;

	// Data used for collision detection
	std::unique_ptr<BoundingBox>	m_boundingBox;
	std::vector<DirectX::XMVECTOR>	m_positions;
	std::vector<unsigned short>		m_indices;

	// Bindables for drawing the mesh (ex. Texture and Sampler)
	// std::vector<std::shared_ptr<Bindable>> m_bindables;

	// DEBUG SPECIFIC --------------------------------------------------------
#ifndef NDEBUG
public:
	void DrawBoundingBox(const DirectX::XMMATRIX& modelMatrix, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix);

#endif
};

template <typename T, typename A>
void Mesh::LoadBuffers(std::vector<T, A>& vertices, std::vector<unsigned short>& indices)
{
	INFOMAN(m_deviceResources);

	m_sizeOfVertex = sizeof(T);
	m_vertexCount = static_cast<unsigned int>(vertices.size());

	// Vertex Buffer
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = static_cast<UINT>(vertices.size() * m_sizeOfVertex);
	bd.StructureByteStride = m_sizeOfVertex;
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices.data();
	GFX_THROW_INFO(m_deviceResources->D3DDevice()->CreateBuffer(&bd, &sd, &m_vertexBuffer));

	// Index Buffer
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = static_cast<UINT>(indices.size() * sizeof(unsigned short));
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices.data();
	GFX_THROW_INFO(m_deviceResources->D3DDevice()->CreateBuffer(&ibd, &isd, &m_indexBuffer));

	m_indexCount = static_cast<unsigned int>(indices.size());

	// If this worked, copy over the position data and create the BoundingBox
	// NOTE: HUGE ASSUMPTION that the vertex has a XMFLOAT3 position member variable
	for (T vertex : vertices)
		m_positions.push_back(DirectX::XMLoadFloat3(&vertex.position));

	for (unsigned short index : indices)
		m_indices.push_back(index);

	m_boundingBox = std::make_unique<BoundingBox>(m_deviceResources, m_positions);
}