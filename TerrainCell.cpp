#include "TerrainCell.h"

TerrainCell::TerrainCell(std::shared_ptr<DeviceResources> deviceResources, 
	std::shared_ptr<MoveLookController> moveLookController,
	std::string meshLookupName) :
		Drawable(deviceResources, moveLookController)
{
	SetMesh(meshLookupName);
}
