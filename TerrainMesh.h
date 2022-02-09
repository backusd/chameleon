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
	};

	struct ModelType
	{
		float x, y, z;
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
	void SetTerrainCoordinates();
	void BuildTerrainModel();


	unsigned int m_vertexCount;

	// ------
	int m_terrainHeight, m_terrainWidth;
	float m_heightScale;
	std::string m_terrainFilename;

	std::vector<std::unique_ptr<HeightMapType>> m_heightMapVector;
	std::vector<std::unique_ptr<ModelType>> m_terrainModelVector;
};