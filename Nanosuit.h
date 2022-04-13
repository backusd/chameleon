#pragma once
#include "pch.h"
#include "Drawable.h"
#include "HLSLStructures.h"


class Nanosuit : public Drawable
{
public:
	Nanosuit(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController);

	void PreDrawUpdate() override;
	DirectX::XMMATRIX GetScaleMatrix() override { return DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f); }

	void Update(std::shared_ptr<StepTimer> timer);

private:
	void CreateMaterialData();
	void CreateAndAddPSBufferArray(); // This is necessary because the cube will create an immutable constant buffer for material data

	PhongMaterialProperties* m_material;
	std::shared_ptr<ConstantBuffer> m_materialConstantBuffer;

	// DEBUG SPECIFIC --------------------------------------------------------
#ifndef NDEBUG
public:
	void DrawImGui(std::string id) override;

private:
	void DrawImGuiMaterialSettings(std::string id);

	bool m_materialNeedsUpdate = false;
	float m_emmissive[4] = { 0.091f, 0.091f, 0.091f, 1.0f };
	float m_ambient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float m_diffuse[4] = { 0.197f, 0.197f, 0.197f, 1.0f };
	float m_specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float m_specularPower = 7.0f;

#endif

};