#pragma once
#include "pch.h"
#include "Drawable.h"

#include <string>


class TerrainCell : public Drawable
{
public:
	TerrainCell(std::shared_ptr<DeviceResources> deviceResources, 
				std::shared_ptr<MoveLookController> moveLookController,
				std::string meshLookupName);

	void PreDrawUpdate() override {}
	void Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Terrain> terrain) override {}

private:

};