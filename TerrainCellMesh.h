#pragma once
#include "DeviceResources.h"
#include "DeviceResourcesException.h"
#include "TerrainMeshException.h"
#include "Mesh.h"
#include "HLSLStructures.h"

#include <memory>
#include <vector>

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

	void Initialize(std::vector<std::unique_ptr< TerrainModelType>>& terrainModel, int nodeIndexX, int nodeIndexY, int cellHeight, int cellWidth, int terrainWidth);
	

	DirectX::XMFLOAT3 GetCenter();
	float GetXLength();
	float GetYLength();
	float GetZLength();

	float GetMaxWidth() { return m_maxWidth; }
	float GetMaxHeight() { return m_maxHeight; }
	float GetMaxDepth() { return m_maxDepth; }
	float GetMinWidth() { return m_minWidth; }
	float GetMinHeight() { return m_minHeight; }
	float GetMinDepth() { return m_minDepth; }


	VectorType* m_vertexList;

private:
	void InitializeBuffers(int nodeIndexX, int nodeIndexY, int cellHeight, int cellWidth, int terrainWidth, std::vector<std::unique_ptr< TerrainModelType>>& terrainModel);
	void CalculateCellDimensions();


	int m_vertexCount;

	float m_maxWidth, m_maxHeight, m_maxDepth, m_minWidth, m_minHeight, m_minDepth;
	float m_positionX, m_positionY, m_positionZ;
};