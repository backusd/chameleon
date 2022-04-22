#pragma once
#include "pch.h"
#include "Drawable.h"

class Lighting : public Drawable
{
public: 
	Lighting(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController);

	void PreDrawUpdate() override;

	void Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Terrain> terrain);
	
	// This function is necessary when in debug mode (and possibly in the future when a new scene
	// is created in memory but not rendered yet. Currently it is used to set the lighting buffer
	// when switching between the normal scene and the center on origin scene
	void Activate();

private:
	void CreateLightProperties();
	void CreateAndBindLightPropertiesBuffer();
	void UpdatePSConstantBuffer();
	void CreateAndAddPSBufferArray();

	LightProperties	m_lightProperties;
	PhongMaterialProperties* m_material;

	std::shared_ptr<ConstantBuffer> m_lightConstantBuffer;



	// DEBUG SPECIFIC --------------------------------------------------------
#ifndef NDEBUG
public:
	void DrawImGui(std::string id) override;


#endif
};