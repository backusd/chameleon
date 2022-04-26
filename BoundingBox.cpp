#include "BoundingBox.h"

using DirectX::XMFLOAT3;
using DirectX::XMVECTOR;
using DirectX::XMMATRIX;

BoundingBox::BoundingBox(std::shared_ptr<DeviceResources> deviceResources, const std::vector<XMVECTOR>& positions) :
	m_deviceResources(deviceResources),
	m_minX(FLT_MAX),
	m_maxX(-FLT_MAX),
	m_minY(FLT_MAX),
	m_maxY(-FLT_MAX),
	m_minZ(FLT_MAX),
	m_maxZ(-FLT_MAX)
#ifndef NDEBUG
	,m_topology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST),
	m_vertexBuffer(nullptr),
	m_vertexCount(0),
	m_sizeOfVertex(0)
#endif
{
	XMFLOAT3 position;
	for (XMVECTOR p : positions)
	{
		DirectX::XMStoreFloat3(&position, p);

		if (position.x > m_maxX)
			m_maxX = position.x;
		else if (position.x < m_minX)
			m_minX = position.x;

		if (position.y > m_maxY)
			m_maxY = position.y;
		else if (position.y < m_minY)
			m_minY = position.y;

		if (position.z > m_maxZ)
			m_maxZ = position.z;
		else if (position.z < m_minZ)
			m_minZ = position.z;
	}

	// Create the 8 vertices
	xyz = DirectX::XMVectorSet(m_minX, m_minY, m_minZ, 1.0f);
	Xyz = DirectX::XMVectorSet(m_maxX, m_minY, m_minZ, 1.0f);
	xYz = DirectX::XMVectorSet(m_minX, m_maxY, m_minZ, 1.0f);
	xyZ = DirectX::XMVectorSet(m_minX, m_minY, m_maxZ, 1.0f);
	XYz = DirectX::XMVectorSet(m_maxX, m_maxY, m_minZ, 1.0f);
	XyZ = DirectX::XMVectorSet(m_maxX, m_minY, m_maxZ, 1.0f);
	xYZ = DirectX::XMVectorSet(m_minX, m_maxY, m_maxZ, 1.0f);
	XYZ = DirectX::XMVectorSet(m_maxX, m_maxY, m_maxZ, 1.0f);

	LoadBuffers();
}

bool BoundingBox::RayIntersectionTest(XMVECTOR rayOrigin, XMVECTOR rayDirection, float& distance)
{
	float dist;
	bool found = false;
	distance = FLT_MAX;

	// Imagine a coordinate system with the positive x-axis going to the right, y-axis going up, and
	// z-axis coming at you. Now imagine the cube located in the positive xyz quadrant with one corner
	// at the origin. There are 6 planes to test, each with two triangles

	// min Z plane 
	if (DirectX::TriangleTests::Intersects(rayOrigin, rayDirection, xyz, Xyz, xYz, dist) ||
		DirectX::TriangleTests::Intersects(rayOrigin, rayDirection, XYz, Xyz, xYz, dist))
	{
		distance = std::min(distance, dist);
		found = true;
	}

	// max Z plane 
	if (DirectX::TriangleTests::Intersects(rayOrigin, rayDirection, xyZ, XyZ, xYZ, dist) ||
		DirectX::TriangleTests::Intersects(rayOrigin, rayDirection, XYZ, XyZ, xYZ, dist))
	{
		distance = std::min(distance, dist);
		found = true;
	}

	// min Y plane 
	if (DirectX::TriangleTests::Intersects(rayOrigin, rayDirection, xyz, xyZ, Xyz, dist) ||
		DirectX::TriangleTests::Intersects(rayOrigin, rayDirection, XyZ, xyZ, Xyz, dist))
	{
		distance = std::min(distance, dist);
		found = true;
	}

	// max Y plane 
	if (DirectX::TriangleTests::Intersects(rayOrigin, rayDirection, xYz, xYZ, XYz, dist) ||
		DirectX::TriangleTests::Intersects(rayOrigin, rayDirection, XYZ, xYZ, XYz, dist))
	{
		distance = std::min(distance, dist);
		found = true;
	}

	// min X plane 
	if (DirectX::TriangleTests::Intersects(rayOrigin, rayDirection, xyz, xyZ, xYz, dist) ||
		DirectX::TriangleTests::Intersects(rayOrigin, rayDirection, xYZ, xyZ, xYz, dist))
	{
		distance = std::min(distance, dist);
		found = true;
	}

	// max X plane 
	if (DirectX::TriangleTests::Intersects(rayOrigin, rayDirection, Xyz, XyZ, XYz, dist) ||
		DirectX::TriangleTests::Intersects(rayOrigin, rayDirection, XYZ, XyZ, XYz, dist))
	{
		distance = std::min(distance, dist);
		found = true;
	}

	return found;
}

void BoundingBox::GetBoundingBoxPositionsWithTransformation(const XMMATRIX& tranformation, std::vector<XMVECTOR>& positions)
{
	XMFLOAT3 position;

	positions.push_back(xyz);
	positions.push_back(Xyz);
	positions.push_back(xYz);
	positions.push_back(xyZ);
	positions.push_back(XYz);
	positions.push_back(XyZ);
	positions.push_back(xYZ);
	positions.push_back(XYZ);
}

#ifndef NDEBUG
void BoundingBox::LoadBuffers()
{
	INFOMAN(m_deviceResources);

	std::vector<SolidColorVertex> vertices;

	// x -> X ========================================
	// xyz -> Xyz
	LoadPosition(xyz, vertices);
	LoadPosition(Xyz, vertices);

	// xyZ -> XyZ
	LoadPosition(xyZ, vertices);
	LoadPosition(XyZ, vertices);

	// xYZ -> XYZ
	LoadPosition(xYZ, vertices);
	LoadPosition(XYZ, vertices);

	// xYz -> XYz
	LoadPosition(xYz, vertices);
	LoadPosition(XYz, vertices);

	// Remaining little x ===============================
	// xyz -> xyZ
	LoadPosition(xyz, vertices);
	LoadPosition(xyZ, vertices);

	// xyZ -> xYZ
	LoadPosition(xyZ, vertices);
	LoadPosition(xYZ, vertices);

	// xYZ -> xYz
	LoadPosition(xYZ, vertices);
	LoadPosition(xYz, vertices);

	// xYz -> xyz
	LoadPosition(xYz, vertices);
	LoadPosition(xyz, vertices);

	// Remaining Big X ===============================
	// Xyz -> XyZ
	LoadPosition(Xyz, vertices);
	LoadPosition(XyZ, vertices);

	// XyZ -> XYZ
	LoadPosition(XyZ, vertices);
	LoadPosition(XYZ, vertices);

	// XYZ -> XYz
	LoadPosition(XYZ, vertices);
	LoadPosition(XYz, vertices);

	// XYz -> Xyz
	LoadPosition(XYz, vertices);
	LoadPosition(Xyz, vertices);

	m_vertexCount = vertices.size();
	m_sizeOfVertex = sizeof(SolidColorVertex);

	// Vertex Buffer
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = static_cast<UINT>(m_vertexCount * m_sizeOfVertex);
	bd.StructureByteStride = m_sizeOfVertex;
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices.data();
	GFX_THROW_INFO(m_deviceResources->D3DDevice()->CreateBuffer(&bd, &sd, &m_vertexBuffer));
}

void BoundingBox::LoadPosition(const XMVECTOR& position, std::vector<SolidColorVertex>& vertices)
{
	SolidColorVertex v;
	v.color = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f); // Start with BLACK

	DirectX::XMStoreFloat4(&v.position, position);
	v.position.w = 1.0f;
	vertices.push_back(v);
}

void BoundingBox::Draw(const XMMATRIX& parentModelMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix)
{
	INFOMAN(m_deviceResources);
	ID3D11DeviceContext4* context = m_deviceResources->D3DDeviceContext();

	// Set topology (should be line list)
	GFX_THROW_INFO_ONLY(
		context->IASetPrimitiveTopology(m_topology)
	);

	// Set vertex buffers
	const UINT stride = m_sizeOfVertex;
	const UINT offset = 0u;
	GFX_THROW_INFO_ONLY(
		context->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &stride, &offset)
	);

	// Update the Model/View/Projection constant buffer that is bound to slot 0 in the vertex shader
	D3D11_MAPPED_SUBRESOURCE ms;
	ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));

	Microsoft::WRL::ComPtr<ID3D11Buffer> vsBuffer;
	GFX_THROW_INFO_ONLY(
		context->VSGetConstantBuffers(0, 1, vsBuffer.ReleaseAndGetAddressOf())
	);

	GFX_THROW_INFO(
		context->Map(vsBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)
	);

	ModelViewProjectionConstantBuffer* mappedBuffer = (ModelViewProjectionConstantBuffer*)ms.pData;
	XMMATRIX viewProjection = viewMatrix * projectionMatrix;
	DirectX::XMStoreFloat4x4(&(mappedBuffer->model), parentModelMatrix);
	DirectX::XMStoreFloat4x4(&(mappedBuffer->modelViewProjection), parentModelMatrix * viewProjection);
	DirectX::XMStoreFloat4x4(&(mappedBuffer->inverseTransposeModel), DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, parentModelMatrix)));

	GFX_THROW_INFO_ONLY(
		context->Unmap(vsBuffer.Get(), 0)
	);


	// Issue the Draw call
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->Draw(m_vertexCount, 0u)
	);
}
#endif