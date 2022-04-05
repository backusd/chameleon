#pragma once
#include "pch.h"
#include "Drawable.h"

#include "HLSLStructures.h"


class Sphere : public Drawable
{
public:
	Sphere(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController);

	DirectX::XMMATRIX GetScaleMatrix() override { return DirectX::XMMatrixScaling(m_radius, m_radius, m_radius); }
	void SetRadius(float radius) { m_radius = radius; }

	void PreDrawUpdate() override;

	void Update(std::shared_ptr<StepTimer> timer);

private:
	void CreateMaterialData();
	void CreateAndAddPSBufferArray(); // This is necessary because the cube will create an immutable constant buffer for material data

	float m_radius;
	PhongMaterialProperties* m_material;

};