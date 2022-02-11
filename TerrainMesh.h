#pragma once
#include "pch.h"
#include "DeviceResources.h"
#include "DeviceResourcesException.h"
#include "TerrainMeshException.h"
#include "Mesh.h"
#include "HLSLStructures.h"

#include <memory>

#include <fstream>
#include <stdio.h>


class TerrainMesh : public Mesh
{
	struct HeightMapType
	{
		float x, y, z;
		float nx, ny, nz;
		float r, g, b;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
		float r, g, b;
	};

	struct VectorType
	{
		float x, y, z;
	};

	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};


public:
	TerrainMesh(std::shared_ptr<DeviceResources> deviceResources);
	TerrainMesh(const TerrainMesh&) = delete;
	TerrainMesh& operator=(const TerrainMesh&) = delete;


private:
	void Tutorial1Setup();
	void Tutorial2Setup(std::string setupFilename);

	void InitializeBuffers();

	void LoadSetupFile(std::string setupFilename);
	void LoadBitmapHeightMap();
	void LoadRawHeightMap();
	void SetTerrainCoordinates();
	void BuildTerrainModel();
	void CalculateNormals();
	void LoadColorMap();

	void CalculateTerrainVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);

	unsigned int m_vertexCount;

	// ------
	int m_terrainHeight, m_terrainWidth;
	float m_heightScale;
	std::string m_terrainFilename;
	std::string m_colorMapFilename;

	std::vector<std::unique_ptr<HeightMapType>> m_heightMapVector;
	std::vector<std::unique_ptr<ModelType>>     m_terrainModelVector;
};