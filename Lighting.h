#pragma once
#include "pch.h"
#include "Drawable.h"

class Lighting : public Drawable
{
public: 
	Lighting(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController);

	void PreDrawUpdate() override;

	void Update(std::shared_ptr<StepTimer> timer);

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

private:
	DirectX::XMFLOAT3 m_positionMax;
	DirectX::XMFLOAT3 m_positionMin;
#endif
};