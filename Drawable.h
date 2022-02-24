#pragma once
#include "pch.h"
#include "Bindable.h"
#include "ObjectStore.h"
#include "Mesh.h"
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
	void SetMesh(std::string lookupName) { m_mesh = ObjectStore::GetMesh(lookupName); }
	void SetProjectionMatrix(DirectX::XMMATRIX projection) { m_projectionMatrix = projection; }

	// The PreDrawUpdate function will execute immediately prior to performing the actual Draw call
	// within the Draw function. This allows updating of any constant buffers that may have been
	// bound to a shader stage that need to be updated
	virtual void PreDrawUpdate() {}
	void Draw();

	// Every object should provide how to scale itself
	virtual DirectX::XMMATRIX GetScaleMatrix() { return DirectX::XMMatrixIdentity(); }
	DirectX::XMMATRIX GetModelMatrix();

	virtual void Update(std::shared_ptr<StepTimer> timer) = 0;
	void SetPosition(DirectX::XMFLOAT3 position) { m_position = position; }

	// If we are in DEBUG, then the move look controller may change, so allow it to be updated
#ifndef NDEBUG
	void SetMoveLookController(std::shared_ptr<MoveLookController> mlc) { m_moveLookController = mlc; }
#endif


protected:
	std::shared_ptr<DeviceResources> m_deviceResources;
	std::shared_ptr<MoveLookController> m_moveLookController;

	DirectX::XMMATRIX m_projectionMatrix;

	std::vector<std::shared_ptr<Bindable>> m_bindables;
	std::shared_ptr<Mesh> m_mesh;


	DirectX::XMFLOAT3 m_position; // Every object will have a "center point" location

	// Rotation about the internal center point
	float m_roll;
	float m_pitch;
	float m_yaw;
};