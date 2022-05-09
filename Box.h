#pragma once
#include "pch.h"
#include "Drawable.h"
#include "HLSLStructures.h"


class Box : public Drawable
{
public:
	Box(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController);

	void SetSideLengths(DirectX::XMFLOAT3 sideLengths) { m_scaling = sideLengths; }

private:
	void CreateMaterialData();
	void CreateAndAddPSBufferArray(); // This is necessary because the cube will create an immutable constant buffer for material data

	PhongMaterialProperties* m_material;

};