#pragma once
#include "pch.h"
#include "Drawable.h"
#include "HLSLStructures.h"


class Nanosuit : public Drawable
{
public:
	Nanosuit(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController);

	void PreDrawUpdate() override;
	DirectX::XMMATRIX GetScaleMatrix() override { return DirectX::XMMatrixScaling(m_scaleFactor, m_scaleFactor, m_scaleFactor); }
	DirectX::XMFLOAT3 CenterOfModel();

	void Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Terrain> terrain);
	
	void MoveForward(bool moveForward) { m_movingForward = moveForward; }
	void LookLeft(float angle);
	void LookRight(float angle);


private:
	void CreateMaterialData();
	void CreateAndAddPSBufferArray(); // This is necessary because the cube will create an immutable constant buffer for material data

	PhongMaterialProperties* m_material;
	std::shared_ptr<ConstantBuffer> m_materialConstantBuffer;

	// scale factor for scaling down the size of the model
	float m_scaleFactor;

	float m_movementSpeed;
	bool m_movingForward;
	double m_currentTime;
	double m_previousTime;

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