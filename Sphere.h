#pragma once
#include "pch.h"
#include "Drawable.h"

#include "HLSLStructures.h"


class Sphere : public Drawable
{
public:
	Sphere(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController);

	void SetRadius(float radius) { m_scaling = DirectX::XMFLOAT3(radius, radius, radius); }

private:
	void CreateMaterialData();
	void CreateAndAddPSBufferArray(); // This is necessary because the cube will create an immutable constant buffer for material data

	float m_radius;
	PhongMaterialProperties* m_material;

};