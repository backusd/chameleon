#pragma once
#include "pch.h"
#include "Bindable.h"
#include "ObjectStore.h"
#include "ModelNode.h"
#include "BoundingBox.h"
#include "StepTimer.h"
#include "MoveLookController.h"
#include "DrawableException.h"

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <filesystem>


enum class BasicModelType
{
	Plane,
	Cube,
	Sphere,
	Cylinder
};

// Forward declare the Terrain class because we will be passing a pointer to Terrain
// to the pure virtual Update function
class Terrain;

class Drawable
{
public:
	Drawable(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, BasicModelType modelType);
	Drawable(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, std::string filename);
	Drawable(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, std::shared_ptr<Mesh> mesh);

	void AddBindable(std::string lookupName) { m_bindables.push_back(ObjectStore::GetBindable(lookupName)); }
	void AddBindable(std::shared_ptr<Bindable> bindable) { m_bindables.push_back(bindable); }
	void SetProjectionMatrix(DirectX::XMMATRIX projection) { m_projectionMatrix = projection; }

	// The PreDrawUpdate function will execute immediately prior to performing the actual Draw call
	// within the Draw function. This allows updating of any constant buffers that may have been
	// bound to a shader stage that need to be updated
	// virtual void PreDrawUpdate() {}
	
	void Draw();

	// Every object should provide how to scale itself
	virtual DirectX::XMMATRIX GetScaleMatrix() { return DirectX::XMMatrixScaling(m_scaleX, m_scaleY, m_scaleZ); }
	DirectX::XMMATRIX GetModelMatrix();
	DirectX::XMMATRIX GetProjectionMatrix() { return m_projectionMatrix; }

	void UpdateHelper(std::shared_ptr<StepTimer> timer, std::shared_ptr<Terrain> terrain);
	void SetPosition(DirectX::XMFLOAT3 position) { m_position = position; }

	float Roll() { return m_roll; }
	float Pitch() { return m_pitch; }
	float Yaw() { return m_yaw; }

	std::string GetName() { return m_name; }
	void SetName(std::string name) { m_name = name; }
	void SetRoll(float roll) { m_roll = roll; }
	void SetPitch(float pitch) { m_pitch = pitch; }
	void SetYaw(float yaw) { m_yaw = yaw; }
	void SetScale(DirectX::XMFLOAT3 scale) { m_scaleX = scale.x; m_scaleY = scale.y; m_scaleZ = scale.z; }
	void SetScale(float x, float y, float z) { m_scaleX = x; m_scaleY = y; m_scaleZ = z; }
	void SetScale(float xyz) { m_scaleX = xyz; m_scaleY = xyz; m_scaleZ = xyz; }
	void SetPhongMaterial(std::unique_ptr<PhongMaterialProperties> material);
	void CreateAndAddPSBufferArray();

	bool IsMouseHovered(float mouseX, float mouseY, float& distance);

	// Functional used for updating buffers, etc., after all bindings and before issuing the draw call
	std::function<void()> PreDrawUpdate;

	// Functional for user interaction events
	std::function<void()> OnMouseHover;
	std::function<void()> OnMouseNotHover;
	std::function<void()> OnMouseClick;
	std::function<void()> OnRightMouseClick;



	// -------------------------------------------------------------
	//std::shared_ptr<RasterizerState> m_rasterizerState;
	//std::vector<std::shared_ptr<SamplerState>> m_samplerStates;
	//ShadingEffect m_shadingEffect;



	std::unique_ptr<ModelNode> m_rootNode;

	// When loading a scene/model via assimp, the meshes are just stored in a flat
	// array. The hierarchy of nodes then just have an index into that array. So, 
	// for our purpose, the model we are building needs to first create a vector
	// of shared pointers to these meshes and then as we build the node hierarchy,
	// we assign out the meshes to the corresponding nodes
	std::vector<std::shared_ptr<Mesh>> m_meshes;

	// BoundingBox to excapsulate the entire Model
	std::unique_ptr<::BoundingBox>	m_boundingBox;


	// -------------------------------------------------------------





protected:
	void UpdateModelViewProjectionConstantBuffer();
	virtual void Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Terrain> terrain) {}
	void LoadMesh(const aiMesh& mesh, const aiMaterial* const* materials);

	std::string m_name;

	std::unique_ptr<PhongMaterialProperties> m_material;
	std::shared_ptr<ConstantBuffer> m_materialConstantBuffer;



	std::shared_ptr<DeviceResources> m_deviceResources;
	std::shared_ptr<MoveLookController> m_moveLookController;

	DirectX::XMMATRIX m_projectionMatrix;

	std::vector<std::shared_ptr<Bindable>> m_bindables;


	DirectX::XMFLOAT3 m_position; // Every object will have a "center point" location

	// Rotation about the internal center point
	float m_roll;
	float m_pitch;
	float m_yaw;

	// Scale Values
	float m_scaleX;
	float m_scaleY;
	float m_scaleZ;


	// DEBUG SPECIFIC --------------------------------------------------------
#ifndef NDEBUG
public:
	void SetMoveLookController(std::shared_ptr<MoveLookController> mlc) { m_moveLookController = mlc; m_rootNode->SetMoveLookController(mlc); }
	virtual void DrawImGuiCollapsable(std::string id);
	virtual void DrawImGuiDetails(std::string id);
	void UpdatePhongMaterial();

protected:
	virtual void DrawImGuiPosition(std::string id);
	void DrawImGuiRollPitchYaw(std::string id);
	void DrawImGuiScale(std::string id);
	virtual void DrawImGuiMaterialSettings(std::string id);

	// Phong material
	bool m_materialNeedsUpdate = false;
	float m_emmissive[4];
	float m_ambient[4];
	float m_diffuse[4];
	float m_specular[4];
	float m_specularPower;

	// Bool on whether or not to sync the scale values together
	bool m_syncScaleValues = true;

	// ------------------------
	void DrawBoundingBox(const DirectX::XMMATRIX& parentModelMatrix, const DirectX::XMMATRIX& projectionMatrix);
	bool NeedDrawBoundingBox();
	bool m_drawBoundingBox;

	// ---------------------------
#endif
};