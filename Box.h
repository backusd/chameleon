#pragma once
#include "pch.h"
#include "Drawable.h"
#include "HLSLStructures.h"


class Box : public Drawable
{
public:
	Box(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController);

	void SetSideLengths(DirectX::XMFLOAT3 length) { m_xSideLength = length.x; m_ySideLength = length.y; m_zSideLength = length.z; }

	DirectX::XMMATRIX GetScaleMatrix() override;

protected:
	void Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Terrain> terrain) override;

private:
	void CreateMaterialData();
	void CreateAndAddPSBufferArray(); // This is necessary because the cube will create an immutable constant buffer for material data

	float m_xSideLength;
	float m_ySideLength;
	float m_zSideLength;

	PhongMaterialProperties* m_material;

};