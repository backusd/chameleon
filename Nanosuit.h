#pragma once
#include "pch.h"
#include "Drawable.h"
#include "HLSLStructures.h"


class Nanosuit : public Drawable
{
public:
	Nanosuit(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController);

	void PreDrawUpdate() override;

	void Update(std::shared_ptr<StepTimer> timer);

private:
	void CreateMaterialData();
	void CreateAndAddPSBufferArray(); // This is necessary because the cube will create an immutable constant buffer for material data

	PhongMaterialProperties* m_material;

	// DEBUG SPECIFIC --------------------------------------------------------
#ifndef NDEBUG
public:
	void DrawImGui(std::string id) override;

#endif

};