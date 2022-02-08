#pragma once
#include "pch.h"
#include "DeviceResources.h"
#include "DeviceResourcesException.h"
#include "Mesh.h"
#include "HLSLStructures.h"

#include <memory>




class TerrainMesh : public Mesh
{
public:
	TerrainMesh(std::shared_ptr<DeviceResources> deviceResources);
	TerrainMesh(const TerrainMesh&) = delete;
	TerrainMesh& operator=(const TerrainMesh&) = delete;


private:
	unsigned int m_vertexCount;



};