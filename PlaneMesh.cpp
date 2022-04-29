#include "PlaneMesh.h"


using DirectX::XMFLOAT2;
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;


PlaneMesh::PlaneMesh(std::shared_ptr<DeviceResources> deviceResources) :
	Mesh(deviceResources)
{
	LoadVertices();
}

void PlaneMesh::LoadVertices()
{
	INFOMAN(m_deviceResources);

	// Vertex Buffer =================================================================================
	// Create a plane in the xy-plane with bottom left at (0, 0) and top right at (1, 1)
	std::vector<PlaneVertex> v(4);
	v[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	v[1].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	v[2].position = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	v[3].position = XMFLOAT3(1.0f, 1.0f, 0.0f);

	v[0].normal = XMFLOAT3(0.0f, 0.0f, 1.0f); // All normals should just be in the z-direction
	v[1].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
	v[2].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
	v[3].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

//	v[0].texture = XMFLOAT2(0.0f, 0.0f);
//	v[1].texture = XMFLOAT2(1.0f, 0.0f);
//	v[2].texture = XMFLOAT2(0.0f, 1.0f);
//	v[3].texture = XMFLOAT2(1.0f, 1.0f);

	v[0].texture = XMFLOAT2(0.0f, 1.0f);
	v[1].texture = XMFLOAT2(1.0f, 1.0f);
	v[2].texture = XMFLOAT2(0.0f, 0.0f);
	v[3].texture = XMFLOAT2(1.0f, 0.0f);

	m_sizeOfVertex = sizeof(PlaneVertex);

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
		0,1,2,  1,3,2
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