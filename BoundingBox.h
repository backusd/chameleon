#pragma once
#include "pch.h"
#include "DeviceResources.h"
#include "HLSLStructures.h"

#include <vector>
#include <memory>
#include <DirectXCollision.h>

struct SolidColorVertex
{
	DirectX::XMFLOAT4 position;
	DirectX::XMFLOAT4 color;
};

class BoundingBox
{
public:
	BoundingBox(std::shared_ptr<DeviceResources> deviceResources, const std::vector<DirectX::XMVECTOR>& positions);

	bool RayIntersectionTest(DirectX::XMVECTOR rayOrigin, DirectX::XMVECTOR rayDirection, float& distance);
	void GetBoundingBoxPositionsWithTransformation(const DirectX::XMMATRIX& tranformation, std::vector<DirectX::XMVECTOR>& positions);


private:
	std::shared_ptr<DeviceResources> m_deviceResources;

	float m_minX, m_maxX;
	float m_minY, m_maxY;
	float m_minZ, m_maxZ;

	DirectX::XMVECTOR xyz;
	DirectX::XMVECTOR Xyz;
	DirectX::XMVECTOR xYz;
	DirectX::XMVECTOR xyZ;
	DirectX::XMVECTOR XYz;
	DirectX::XMVECTOR XyZ;
	DirectX::XMVECTOR xYZ;
	DirectX::XMVECTOR XYZ;


	// DEBUG SPECIFIC - Used for drawing the bounding box --------------------
#ifndef NDEBUG
public:
	void Draw(const DirectX::XMMATRIX& modelMatrix, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix);

private:
	void LoadBuffers();
	void LoadPosition(const DirectX::XMVECTOR& position, std::vector<SolidColorVertex>& vertices);

	D3D11_PRIMITIVE_TOPOLOGY m_topology;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	unsigned int m_vertexCount;
	unsigned int m_sizeOfVertex;
#endif
};