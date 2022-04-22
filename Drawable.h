#pragma once
#include "pch.h"
#include "Bindable.h"
#include "ObjectStore.h"
#include "Model.h"
#include "StepTimer.h"
#include "MoveLookController.h"

#include <vector>
#include <memory>
#include <string>
#include <functional>

// Forward declare the Terrain class because we will be passing a pointer to Terrain
// to the pure virtual Update function
class Terrain;

class Drawable
{
public:
	Drawable(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController);

	void AddBindable(std::string lookupName) { m_bindables.push_back(ObjectStore::GetBindable(lookupName)); }
	void SetProjectionMatrix(DirectX::XMMATRIX projection) { m_projectionMatrix = projection; }

	// The PreDrawUpdate function will execute immediately prior to performing the actual Draw call
	// within the Draw function. This allows updating of any constant buffers that may have been
	// bound to a shader stage that need to be updated
	// virtual void PreDrawUpdate() {}
	
	void Draw();

	// Every object should provide how to scale itself
	virtual DirectX::XMMATRIX GetScaleMatrix() { return DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z); }
	DirectX::XMMATRIX GetModelMatrix();
	DirectX::XMMATRIX GetProjectionMatrix() { return m_projectionMatrix; }

	virtual void Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Terrain> terrain) {}
	void SetPosition(DirectX::XMFLOAT3 position) { m_position = position; }

	float Roll() { return m_roll; }
	float Pitch() { return m_pitch; }
	float Yaw() { return m_yaw; }

	void SetModel(std::string fileName) { m_model = std::make_unique<Model>(m_deviceResources, m_moveLookController, fileName); }
	void SetScale(DirectX::XMFLOAT3 scale) { m_scale = scale; }
	void SetPhongMaterial(std::unique_ptr<PhongMaterialProperties> material);
	void CreateAndAddPSBufferArray();

	// Functional used for updating buffers, etc., after all bindings and before issuing the draw call
	std::function<void()> PreDrawUpdate;

protected:
	void UpdateModelViewProjectionConstantBuffer();

	std::unique_ptr<PhongMaterialProperties> m_material;
	std::shared_ptr<ConstantBuffer> m_materialConstantBuffer;



	std::shared_ptr<DeviceResources> m_deviceResources;
	std::shared_ptr<MoveLookController> m_moveLookController;

	DirectX::XMMATRIX m_projectionMatrix;

	std::vector<std::shared_ptr<Bindable>> m_bindables;
	std::unique_ptr<Model> m_model;


	DirectX::XMFLOAT3 m_position; // Every object will have a "center point" location

	// Rotation about the internal center point
	float m_roll;
	float m_pitch;
	float m_yaw;

	// Scale Values
	DirectX::XMFLOAT3 m_scale;


	// DEBUG SPECIFIC --------------------------------------------------------
#ifndef NDEBUG
public:
	void SetMoveLookController(std::shared_ptr<MoveLookController> mlc) { m_moveLookController = mlc; m_model->SetMoveLookController(mlc); }
	virtual void DrawImGui(std::string id);
	void UpdatePhongMaterial();

protected:
	virtual void DrawImGuiPosition(std::string id);
	void DrawImGuiRollPitchYaw(std::string id);
	virtual void DrawImGuiMaterialSettings(std::string id);

	// Phong material
	bool m_materialNeedsUpdate = false;
	float m_emmissive[4];
	float m_ambient[4];
	float m_diffuse[4];
	float m_specular[4];
	float m_specularPower;
#endif
};