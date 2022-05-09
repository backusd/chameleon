#pragma once
#include "pch.h"
#include "Bindable.h"
#include "ObjectStore.h"
#include "BoundingBox.h"
#include "StepTimer.h"
#include "MoveLookController.h"
#include "DrawableException.h"

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <filesystem>

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


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
	Drawable(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, std::string name, const aiNode& node, const std::vector<std::shared_ptr<Mesh>>& meshes);


	void AddBindable(std::string lookupName) { m_bindables.push_back(ObjectStore::GetBindable(lookupName)); }
	void AddBindable(std::shared_ptr<Bindable> bindable) { m_bindables.push_back(bindable); }
	void SetProjectionMatrix(DirectX::XMMATRIX projection);

	// The PreDrawUpdate function will execute immediately prior to performing the actual Draw call
	// within the Draw function. This allows updating of any constant buffers that may have been
	// bound to a shader stage that need to be updated
	// virtual void PreDrawUpdate() {}
	
	void Draw();

	// Every object should provide how to scale itself
	virtual DirectX::XMMATRIX GetScaleMatrix() { return DirectX::XMMatrixScaling(m_scaling.x, m_scaling.y, m_scaling.z); }
	DirectX::XMMATRIX GetPreParentTransformModelMatrix();
	DirectX::XMMATRIX GetProjectionMatrix() { return m_projectionMatrix; }

	virtual void UpdatePhysics(std::shared_ptr<StepTimer> timer, std::shared_ptr<Terrain> terrain) {}
	void UpdateRenderData();
	
	void SetPosition(DirectX::XMFLOAT3 position) { m_translation = position; }

	float Roll() { return m_roll; }
	float Pitch() { return m_pitch; }
	float Yaw() { return m_yaw; }

	std::string GetName() { return m_name; }
	void SetName(std::string name) { m_name = name; }
	void SetRoll(float roll) { m_roll = roll; }
	void SetPitch(float pitch) { m_pitch = pitch; }
	void SetYaw(float yaw) { m_yaw = yaw; }
	void SetScale(DirectX::XMFLOAT3 scale) { m_scaling = scale; }
	void SetScale(float x, float y, float z) { m_scaling = DirectX::XMFLOAT3(x, y, z); }
	void SetScale(float xyz) { m_scaling = DirectX::XMFLOAT3(xyz, xyz, xyz); }
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









protected:
	// This Update function is designed to be called during the recursive Update of a Drawable hierarchy.
	void UpdateRenderData(const DirectX::XMMATRIX& parentModelMatrix);
	void UpdateModelViewProjectionConstantBuffer();
	void LoadMesh(const aiMesh& mesh, const aiMaterial* const* materials);
	void ConstructFromAiNode(const aiNode& node, const std::vector<std::shared_ptr<Mesh>>& meshes);
	void GetBoundingBoxPositionsWithTransformation(const DirectX::XMMATRIX& parentModelMatrix, std::vector<DirectX::XMVECTOR>& positions);
	bool IsMouseHovered(const DirectX::XMVECTOR& clickPointNear,
		const DirectX::XMVECTOR& clickPointFar,
		const DirectX::XMMATRIX& projectionMatrix,
		float& distance);

	std::string m_name;			// Name for the object hierarchy as a whole
	std::string m_nodeName;		// Name for this specific drawable node within the hierarchy

	std::unique_ptr<PhongMaterialProperties> m_material;
	std::shared_ptr<ConstantBuffer> m_materialConstantBuffer;



	std::shared_ptr<DeviceResources> m_deviceResources;
	std::shared_ptr<MoveLookController> m_moveLookController;

	DirectX::XMMATRIX m_projectionMatrix;

	std::vector<std::shared_ptr<Bindable>> m_bindables;


	// DirectX::XMFLOAT3 m_position; // Every object will have a "center point" location

	// Rotation about the internal center point
	float m_roll;
	float m_pitch;
	float m_yaw;


	// -------------------------------------------------------------
	//std::shared_ptr<RasterizerState> m_rasterizerState;
	//std::shared_ptr<DepthStencilState> m_depthStencilState;
	//std::vector<std::shared_ptr<SamplerState>> m_samplerStates;
	//ShadingEffect m_shadingEffect;


	std::vector<std::unique_ptr<Drawable>> m_children;
	std::shared_ptr<Mesh> m_mesh;

	DirectX::XMFLOAT3 m_rotation;
	DirectX::XMFLOAT3 m_translation;
	DirectX::XMFLOAT3 m_scaling;
	DirectX::XMMATRIX m_accumulatedModelMatrix;

	// std::unique_ptr<ModelNode> m_rootNode;



	// When loading a scene/model via assimp, the meshes are just stored in a flat
	// array. The hierarchy of nodes then just have an index into that array. So, 
	// for our purpose, the model we are building needs to first create a vector
	// of shared pointers to these meshes and then as we build the node hierarchy,
	// we assign out the meshes to the corresponding nodes
	std::vector<std::shared_ptr<Mesh>> m_meshes;

	// BoundingBox to excapsulate the entire Model
	std::unique_ptr<::BoundingBox>	m_boundingBox;


	// -------------------------------------------------------------


	// DEBUG SPECIFIC --------------------------------------------------------
#ifndef NDEBUG
public:
	void SetMoveLookController(std::shared_ptr<MoveLookController> mlc);
	virtual void DrawImGuiCollapsable(std::string id);
	virtual void DrawImGuiDetails(std::string id);
	void UpdatePhongMaterial();

protected:
	virtual void DrawImGuiPosition(std::string id);
	void DrawImGuiRollPitchYaw(std::string id);
	void DrawImGuiScale(std::string id);
	virtual void DrawImGuiMaterialSettings(std::string id);
	void DrawImGuiNodeHierarchy(std::string id);

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
	void DrawBoundingBox();
	bool NeedDrawBoundingBox();
	bool m_drawBoundingBox;
	bool m_drawWholeBoundingBox;

	// ---------------------------
#endif
};