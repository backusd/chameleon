#include "TerrainCell.h"

using DirectX::XMFLOAT3;
using DirectX::XMVECTOR;

TerrainCell::TerrainCell(std::shared_ptr<DeviceResources> deviceResources, 
	std::shared_ptr<MoveLookController> moveLookController,
	std::string meshLookupName) :
		Drawable(deviceResources, moveLookController, ObjectStore::GetMesh(meshLookupName))
{
}

bool TerrainCell::ContainsPoint(float x, float z)
{
	std::shared_ptr<TerrainCellMesh> cellMesh = std::dynamic_pointer_cast<TerrainCellMesh>(m_mesh);
	return cellMesh->ContainsPoint(x, z);
}

float TerrainCell::GetHeight(float x, float z)
{
	std::shared_ptr<TerrainCellMesh> cellMesh = std::dynamic_pointer_cast<TerrainCellMesh>(m_mesh);
	return cellMesh->GetHeight(x, z);
}

float TerrainCell::GetMinX()
{
	std::shared_ptr<TerrainCellMesh> cellMesh = std::dynamic_pointer_cast<TerrainCellMesh>(m_mesh);
	return cellMesh->GetMinX();
}

float TerrainCell::GetMaxX()
{
	std::shared_ptr<TerrainCellMesh> cellMesh = std::dynamic_pointer_cast<TerrainCellMesh>(m_mesh);
	return cellMesh->GetMaxX();
}

float TerrainCell::GetMinY()
{
	std::shared_ptr<TerrainCellMesh> cellMesh = std::dynamic_pointer_cast<TerrainCellMesh>(m_mesh);
	return cellMesh->GetMinY();
}

float TerrainCell::GetMaxY()
{
	std::shared_ptr<TerrainCellMesh> cellMesh = std::dynamic_pointer_cast<TerrainCellMesh>(m_mesh);
	return cellMesh->GetMaxY();
}

float TerrainCell::GetMinZ()
{
	std::shared_ptr<TerrainCellMesh> cellMesh = std::dynamic_pointer_cast<TerrainCellMesh>(m_mesh);
	return cellMesh->GetMinZ();
}

float TerrainCell::GetMaxZ()
{
	std::shared_ptr<TerrainCellMesh> cellMesh = std::dynamic_pointer_cast<TerrainCellMesh>(m_mesh);
	return cellMesh->GetMaxZ();
}

bool TerrainCell::GetClickLocation(XMFLOAT3 origin, XMFLOAT3 direction, XMFLOAT3& clickLocation, float& distance)
{
	XMVECTOR o = DirectX::XMLoadFloat3(&origin);
	XMVECTOR d = DirectX::XMLoadFloat3(&direction);

	float minX = GetMinX();
	float maxX = GetMaxX();
	float minY = GetMinY();
	float maxY = GetMaxY();
	float minZ = GetMinZ();
	float maxZ = GetMaxZ();

	XMVECTOR xyz = DirectX::XMVectorSet(minX, minY, minZ, 0.0f);
	XMVECTOR Xyz = DirectX::XMVectorSet(maxX, minY, minZ, 0.0f);
	XMVECTOR xYz = DirectX::XMVectorSet(minX, maxY, minZ, 0.0f);
	XMVECTOR xyZ = DirectX::XMVectorSet(minX, minY, maxZ, 0.0f);
	XMVECTOR XYz = DirectX::XMVectorSet(maxX, maxY, minZ, 0.0f);
	XMVECTOR XyZ = DirectX::XMVectorSet(maxX, minY, maxZ, 0.0f);
	XMVECTOR xYZ = DirectX::XMVectorSet(minX, maxY, maxZ, 0.0f);
	XMVECTOR XYZ = DirectX::XMVectorSet(minX, minY, maxZ, 0.0f);

	// Must test for intersection to all faces of the terrain cell to determine if the clicking ray
	// has the potential to interesect the terrain surface
	if (DirectX::TriangleTests::Intersects(o, d, xyz, Xyz, xYz, distance) || // min z-plane
		DirectX::TriangleTests::Intersects(o, d, XYz, Xyz, xYz, distance) ||
		DirectX::TriangleTests::Intersects(o, d, XYZ, XyZ, xYZ, distance) || // max z-plane
		DirectX::TriangleTests::Intersects(o, d, XYZ, XyZ, xYZ, distance) ||
		DirectX::TriangleTests::Intersects(o, d, xyz, xYz, xyZ, distance) || // min x-plane
		DirectX::TriangleTests::Intersects(o, d, xYZ, xYz, xyZ, distance) ||
		DirectX::TriangleTests::Intersects(o, d, Xyz, XYz, XyZ, distance) || // max x-plane
		DirectX::TriangleTests::Intersects(o, d, XYZ, XYz, XyZ, distance) ||
		DirectX::TriangleTests::Intersects(o, d, xyz, Xyz, xyZ, distance) || // min y-plane
		DirectX::TriangleTests::Intersects(o, d, XyZ, Xyz, xyZ, distance) ||
		DirectX::TriangleTests::Intersects(o, d, xYz, XYz, xYZ, distance) || // max y-plane
		DirectX::TriangleTests::Intersects(o, d, XYZ, XYz, xYZ, distance))
	{
		std::shared_ptr<TerrainCellMesh> cellMesh = std::dynamic_pointer_cast<TerrainCellMesh>(m_mesh);
		return cellMesh->GetClickLocation(origin, direction, clickLocation, distance);
	}

	return false;
}