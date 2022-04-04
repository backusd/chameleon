#pragma once
#include "pch.h"

#include "Drawable.h"
#include "HLSLStructures.h"


class Suzanne : public Drawable
{
public:
	Suzanne(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController);

	void SetSideLengths(DirectX::XMFLOAT3 length) { m_xSideLength = length.x; m_ySideLength = length.y; m_zSideLength = length.z; }

	void PreDrawUpdate() override;

	void Update(std::shared_ptr<StepTimer> timer);

private:
	void CreateMaterialData();
	void CreateAndAddPSBufferArray(); // This is necessary because the cube will create an immutable constant buffer for material data

	float m_xSideLength;
	float m_ySideLength;
	float m_zSideLength;

	PhongMaterialProperties* m_material;

};