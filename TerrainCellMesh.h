#pragma once
#include "DeviceResources.h"
#include "DeviceResourcesException.h"
#include "TerrainMeshException.h"
#include "Mesh.h"
#include "HLSLStructures.h"

#include <memory>
#include <vector>

#include <DirectXCollision.h>

struct TerrainModelType
{
	float x, y, z;
	float tu, tv;
	float nx, ny, nz;
	float tx, ty, tz;
	float bx, by, bz;
	float r, g, b;
};

class TerrainCellMesh : public Mesh
{
private:
	struct VectorType
	{
		float x, y, z;
	};

	struct ColorVertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};

public:
	TerrainCellMesh(std::shared_ptr<DeviceResources> deviceResources);
	~TerrainCellMesh();

	void Initialize(TerrainModelType* terrainModel, int nodeIndexX, int nodeIndexY, int cellHeight, int cellWidth, int terrainWidth);
	

	DirectX::XMFLOAT3 GetCenter();
	float GetXLength();
	float GetYLength();
	float GetZLength();

	float GetMinX() { return m_minX; }
	float GetMaxX() { return m_maxX; }
	float GetMinY() { return m_minY; }
	float GetMaxY() { return m_maxY; }
	float GetMinZ() { return m_minZ; }
	float GetMaxZ() { return m_maxZ; }

	bool ContainsPoint(float x, float z);
	float GetHeight(float x, float z);
	bool GetClickLocation(DirectX::XMFLOAT3 origin, DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3& clickLocation, float& distance);



	VectorType* m_vertexList;
	unsigned long* m_indices;

private:
	void InitializeBuffers(int nodeIndexX, int nodeIndexY, int cellHeight, int cellWidth, int terrainWidth, TerrainModelType* terrainModel);
	void CalculateCellDimensions();


	int m_vertexCount;

	float m_maxX, m_maxY, m_maxZ, m_minX, m_minY, m_minZ;
	float m_positionX, m_positionY, m_positionZ;
};