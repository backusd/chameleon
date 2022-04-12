#include "TerrainCell.h"

TerrainCell::TerrainCell(std::shared_ptr<DeviceResources> deviceResources, 
	std::shared_ptr<MoveLookController> moveLookController,
	std::string meshLookupName) :
		Drawable(deviceResources, moveLookController)
{
	m_model = std::make_unique<Model>(deviceResources, moveLookController, ObjectStore::GetMesh(meshLookupName));
}
