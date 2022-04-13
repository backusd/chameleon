#pragma once
#include "pch.h"
#include "Drawable.h"
#include "HLSLStructures.h"


class Suzanne : public Drawable
{
public:
	Suzanne(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController);

	void PreDrawUpdate() override;

	void Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Terrain> terrain);

private:
	void CreateMaterialData();
	void CreateAndAddPSBufferArray(); // This is necessary because the cube will create an immutable constant buffer for material data

	PhongMaterialProperties* m_material;

};