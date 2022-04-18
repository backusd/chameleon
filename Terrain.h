#pragma once
#include "pch.h"
#include "DeviceResources.h"
#include "DeviceResourcesException.h"
#include "MoveLookController.h"
#include "TerrainCell.h"
#include "TerrainMesh.h"
#include "Frustum.h"
#include "Bindable.h"

#include <memory>
#include <vector>



class Terrain
{
public:
	Terrain(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController);
	Terrain(const Terrain&) = delete;
	Terrain& operator=(const Terrain&) = delete;

	// Don't update the terrain - leave it static
	void Update(std::shared_ptr<StepTimer> timer);

	void SetProjectionMatrix(DirectX::XMMATRIX matrix);
	void Draw();

	void AddBindable(std::string lookupName) { m_bindables.push_back(ObjectStore::GetBindable(lookupName)); }

	float GetHeight(float x, float z);
	float GetMinX() { return m_minX; }
	float GetMaxX() { return m_maxX; }
	float GetMinY() { return m_minY; }
	float GetMaxY() { return m_maxY; }
	float GetMinZ() { return m_minZ; }
	float GetMaxZ() { return m_maxZ; }
	bool GetClickLocation(DirectX::XMFLOAT3 origin, DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3& clickLocation);

	// If we are in DEBUG, then the move look controller may change, so allow it to be updated
#ifndef NDEBUG
	void SetMoveLookController(std::shared_ptr<MoveLookController> mlc);
#endif

private:
	void UpdateBindings();

	// Min max values for storing the min/max coordinate values
	float m_minX, m_maxX, m_minY, m_maxY, m_minZ, m_maxZ;

	std::shared_ptr<DeviceResources>	m_deviceResources;
	std::shared_ptr<MoveLookController> m_moveLookController;

	std::vector<std::shared_ptr<TerrainCell>>	m_terrainCells;
	std::vector<bool>							m_terrainCellVisibility;

	std::shared_ptr<TerrainMesh>	m_terrainMesh;
	std::shared_ptr<Frustum>		m_frustum;

	// Can bind everything once that will be the same for each cell
	std::vector<std::shared_ptr<Bindable>> m_bindables;
};