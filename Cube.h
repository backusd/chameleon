#pragma once
#include "pch.h"
#include "Drawable.h"
#include "HLSLStructures.h"


class Cube : public Drawable
{
public:
	Cube(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController);

	void SetSideLengths(DirectX::XMFLOAT3 length) { m_xSideLength = length.x; m_ySideLength = length.y; m_zSideLength = length.z; }

	void PreDrawUpdate() override;
	DirectX::XMMATRIX GetScaleMatrix() override;

	void Update(std::shared_ptr<StepTimer> timer);

private:
	void CreateMaterialAndLightData();

	float m_xSideLength;
	float m_ySideLength;
	float m_zSideLength;

	PhongMaterialProperties* m_material;
	LightProperties			 m_lightProperties;

};