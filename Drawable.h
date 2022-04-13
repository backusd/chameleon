#pragma once
#include "pch.h"
#include "Bindable.h"
#include "ObjectStore.h"
//#include "Mesh.h"
#include "Model.h"
#include "StepTimer.h"
#include "MoveLookController.h"

#include <vector>
#include <memory>
#include <string>

class Drawable
{
public:
	Drawable(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController);

	void AddBindable(std::string lookupName) { m_bindables.push_back(ObjectStore::GetBindable(lookupName)); }
	void SetProjectionMatrix(DirectX::XMMATRIX projection) { m_projectionMatrix = projection; }

	// The PreDrawUpdate function will execute immediately prior to performing the actual Draw call
	// within the Draw function. This allows updating of any constant buffers that may have been
	// bound to a shader stage that need to be updated
	virtual void PreDrawUpdate() {}
	
	void Draw();

	// Every object should provide how to scale itself
	virtual DirectX::XMMATRIX GetScaleMatrix() { return DirectX::XMMatrixIdentity(); }
	DirectX::XMMATRIX GetModelMatrix();
	DirectX::XMMATRIX GetProjectionMatrix() { return m_projectionMatrix; }

	virtual void Update(std::shared_ptr<StepTimer> timer) = 0;
	void SetPosition(DirectX::XMFLOAT3 position) { m_position = position; }

protected:
	void UpdateModelViewProjectionConstantBuffer();


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


	// DEBUG SPECIFIC --------------------------------------------------------
#ifndef NDEBUG
public:
	void SetMoveLookController(std::shared_ptr<MoveLookController> mlc) { m_moveLookController = mlc; m_model->SetMoveLookController(mlc); }
	virtual void DrawImGui(std::string id);

protected:
	void DrawImGuiPosition(std::string id);
	void DrawImGuiRollPitchYaw(std::string id);


#endif
};