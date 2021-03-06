#include "BoxMesh.h"

using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;

BoxMesh::BoxMesh(std::shared_ptr<DeviceResources> deviceResources, bool fillCube) :
	Mesh(deviceResources)
{
	if (fillCube)
		LoadTriangleVertices();
	else
		LoadLineListVertices();
}

void BoxMesh::LoadTriangleVertices()
{
	INFOMAN(m_deviceResources);

	// Vertex Buffer =================================================================================
	float x = 0.5f;
	float y = 0.5f;
	float z = 0.5f;

	std::vector<VertexPositionNormal> v(8); // box vertices
	v[0].position = XMFLOAT3(-x, -y, -z);
	v[1].position = XMFLOAT3(x, -y, -z);
	v[2].position = XMFLOAT3(-x, y, -z);
	v[3].position = XMFLOAT3(x, y, -z);
	v[4].position = XMFLOAT3(-x, -y, z);
	v[5].position = XMFLOAT3(x, -y, z);
	v[6].position = XMFLOAT3(-x, y, z);
	v[7].position = XMFLOAT3(x, y, z);

	v[0].normal = XMFLOAT3(-x, -y, -z);
	v[1].normal = XMFLOAT3(x, -y, -z);
	v[2].normal = XMFLOAT3(-x, y, -z);
	v[3].normal = XMFLOAT3(x, y, -z);
	v[4].normal = XMFLOAT3(-x, -y, z);
	v[5].normal = XMFLOAT3(x, -y, z);
	v[6].normal = XMFLOAT3(-x, y, z);
	v[7].normal = XMFLOAT3(x, y, z);

	m_sizeOfVertex = sizeof(VertexPositionNormal);


	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = static_cast<UINT>(v.size()) * m_sizeOfVertex;
	bd.StructureByteStride = m_sizeOfVertex;
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = v.data();
	GFX_THROW_INFO(m_deviceResources->D3DDevice()->CreateBuffer(&bd, &sd, &m_vertexBuffer));


	// Index Buffer =================================================================================
	const unsigned short indices[] =
	{
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
	};

	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(indices);
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;
	GFX_THROW_INFO(m_deviceResources->D3DDevice()->CreateBuffer(&ibd, &isd, &m_indexBuffer));

	m_indexCount = (UINT)std::size(indices);
}

void BoxMesh::LoadLineListVertices()
{
	m_topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

	INFOMAN(m_deviceResources);

	// Vertex Buffer =================================================================================
	float x = 0.5f;
	float y = 0.5f;
	float z = 0.5f;

	std::vector<BoxPositionColorVertex> v(8); // box vertices
	v[0].position = XMFLOAT3(-x, -y, -z);
	v[1].position = XMFLOAT3(x, -y, -z);
	v[2].position = XMFLOAT3(-x, y, -z);
	v[3].position = XMFLOAT3(x, y, -z);
	v[4].position = XMFLOAT3(-x, -y, z);
	v[5].position = XMFLOAT3(x, -y, z);
	v[6].position = XMFLOAT3(-x, y, z);
	v[7].position = XMFLOAT3(x, y, z);

	v[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	v[1].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	v[2].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	v[3].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	v[4].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	v[5].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	v[6].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	v[7].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	m_sizeOfVertex = sizeof(BoxPositionColorVertex);

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = static_cast<UINT>(v.size()) * m_sizeOfVertex;
	bd.StructureByteStride = m_sizeOfVertex;
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = v.data();
	GFX_THROW_INFO(m_deviceResources->D3DDevice()->CreateBuffer(&bd, &sd, &m_vertexBuffer));

	// Index Buffer =================================================================================
	const unsigned short indices[] =
	{
		0, 1, 
		2, 3,
		4, 5,
		6, 7,

		0, 2,
		1, 3,
		4, 6,
		5, 7,

		0, 4,
		1, 5,
		2, 6,
		3, 7
	};

	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(indices);
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;
	GFX_THROW_INFO(m_deviceResources->D3DDevice()->CreateBuffer(&ibd, &isd, &m_indexBuffer));

	m_indexCount = (UINT)std::size(indices);
}