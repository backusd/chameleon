#include "TerrainCell.h"

TerrainCell::TerrainCell(std::shared_ptr<DeviceResources> deviceResources, 
	std::shared_ptr<MoveLookController> moveLookController,
	std::string meshLookupName) :
		Drawable(deviceResources, moveLookController)
{
	m_model = std::make_unique<Model>(deviceResources, moveLookController, ObjectStore::GetMesh(meshLookupName));
}

bool TerrainCell::ContainsPoint(float x, float z)
{
	std::shared_ptr<Mesh> mesh = m_model->GetRootNodeMesh();
	std::shared_ptr<TerrainCellMesh> cellMesh = std::dynamic_pointer_cast<TerrainCellMesh>(mesh);
	return cellMesh->ContainsPoint(x, z);
}

float TerrainCell::GetHeight(float x, float z)
{
	std::shared_ptr<Mesh> mesh = m_model->GetRootNodeMesh();
	std::shared_ptr<TerrainCellMesh> cellMesh = std::dynamic_pointer_cast<TerrainCellMesh>(mesh);
	return cellMesh->GetHeight(x, z);
}