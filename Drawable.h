#pragma once
#include "pch.h"
#include "Bindable.h"
#include "ObjectStore.h"
#include "BoundingBox.h"
#include "StepTimer.h"
#include "MoveLookController.h"
#include "DrawableException.h"
#include "SamplerStateArray.h"

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <filesystem>
#include <algorithm>
#include <tuple>

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
	Drawable(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, std::string name, const aiNode& node, const std::vector<std::shared_ptr<Mesh>>& meshes, const aiMaterial* const* materials);

	void AddBindable(std::string lookupName);
	void AddBindable(std::shared_ptr<Bindable> bindable);
	void SetProjectionMatrix(DirectX::XMMATRIX projection);

	void Draw();

	// Every object should provide how to scale itself
	DirectX::XMMATRIX GetScaleMatrix() { return DirectX::XMMatrixScaling(m_scaling.x, m_scaling.y, m_scaling.z); }
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




	void UpdateModelViewProjectionBuffer(std::shared_ptr<ConstantBuffer> constantBuffer);

	void SetRasterizerState(std::string lookupName, bool recursive = true);
	void SetDepthStencilState(std::string lookupName, bool recursive = true);
		
	void AddSamplerState(SamplerStateBindingLocation bindingLocation, std::string lookupName, bool recursive = true);

	void AddTexture(TextureBindingLocation bindingLocation, std::shared_ptr<Texture> texture, bool recursive = true);
	void AddTexture(TextureBindingLocation bindingLocation, std::string lookupName, bool recursive = true);

	template <typename T>
	void AddConstantBuffer(ConstantBufferBindingLocation bindingLocation, void (Drawable::* updateFunc)(std::shared_ptr<ConstantBuffer>), bool recursive = true);
	template <typename T>
	void AddConstantBuffer(ConstantBufferBindingLocation bindingLocation, void* initialData, bool recursive = true);
	template <typename T>
	void AddConstantBuffer(ConstantBufferBindingLocation bindingLocation, void* initialData, void (Drawable::* updateFunc)(std::shared_ptr<ConstantBuffer>), bool recursive = true);

protected:
	// This Update function is designed to be called during the recursive Update of a Drawable hierarchy.
	void UpdateRenderData(const DirectX::XMMATRIX& parentModelMatrix);
	void UpdateModelViewProjectionConstantBuffer();
	void InitializePipelineConfiguration();
	void LoadMesh(const aiMesh& mesh, const aiMaterial* const* materials, std::vector<std::shared_ptr<Mesh>>& meshes);
	void ConstructFromAiNode(const aiNode& node, const std::vector<std::shared_ptr<Mesh>>& meshes, const aiMaterial* const* materials);
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

	// Rotation about the internal center point
	float m_roll;
	float m_pitch;
	float m_yaw;


	// -------------------------------------------------------------
	DirectX::XMMATRIX m_previousModelViewProjection;

	std::shared_ptr<InputLayout>			m_inputLayout;
	std::shared_ptr<VertexShader>			m_vertexShader;
	std::shared_ptr<PixelShader>			m_pixelShader;
	//ShadingEffect m_shadingEffect;

	std::shared_ptr<RasterizerState>		m_rasterizerState;
	std::shared_ptr<DepthStencilState>		m_depthStencilState;

	std::vector<std::shared_ptr<SamplerStateArray>>		m_samplerStateArrays;
	std::vector<std::shared_ptr<TextureArray>>			m_textureArrays;
	std::vector<std::shared_ptr<ConstantBufferArray>>	m_constantBufferArrays;

	std::vector<std::tuple<std::shared_ptr<ConstantBuffer>, std::function<void(std::shared_ptr<ConstantBuffer>)>>> m_updateFunctions;

	std::vector<std::unique_ptr<Drawable>> m_children;
	std::shared_ptr<Mesh> m_mesh;

	DirectX::XMFLOAT3 m_rotation;
	DirectX::XMFLOAT3 m_translation;
	DirectX::XMFLOAT3 m_scaling;
	DirectX::XMMATRIX m_accumulatedModelMatrix;

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


template <typename T>
void Drawable::AddConstantBuffer(ConstantBufferBindingLocation bindingLocation, void* initialData, bool recursive)
{
	// Because we are supplying initial data with no update functional, this buffer will be presumed
	// to be usage default
	std::shared_ptr<ConstantBuffer> buffer = std::make_shared<ConstantBuffer>(m_deviceResources);
	buffer->CreateBuffer<T>(
		D3D11_USAGE_DEFAULT,	// Usage: Read-only by the GPU. Not accessible via CPU. MUST be initialized at buffer creation
		0,						// CPU Access: No CPU access
		0,						// Misc Flags: No miscellaneous flags
		0,						// Structured Byte Stride: Not totally sure, but I don't think this needs to be set because even though it is a structured buffer, there is only a single element
		initialData				// Initial Data: Fill the buffer with config data
	);

	if (m_constantBufferArrays[(int)bindingLocation] == nullptr)
		m_constantBufferArrays[(int)bindingLocation] = std::make_shared<ConstantBufferArray>(m_deviceResources, bindingLocation);

	m_constantBufferArrays[(int)bindingLocation]->AddBuffer(buffer);
	AddBindable(m_constantBufferArrays[(int)bindingLocation]);

	if (recursive)
	{
		for (std::unique_ptr<Drawable>& child : m_children)
			child->AddConstantBuffer<T>(bindingLocation, initialData, true);
	}
}

template <typename T>
void Drawable::AddConstantBuffer(ConstantBufferBindingLocation bindingLocation, void (Drawable::* updateFunc)(std::shared_ptr<ConstantBuffer>), bool recursive)
{
	// Because we are supplying the update functional, we need to allow this buffer to be mappable
	std::shared_ptr<ConstantBuffer> buffer = std::make_shared<ConstantBuffer>(m_deviceResources);
	buffer->CreateBuffer<T>(
		D3D11_USAGE_DYNAMIC,	// Usage: Dynamic
		D3D11_CPU_ACCESS_WRITE,	// CPU Access: CPU will be able to write using Map
		0,						// Misc Flags: No miscellaneous flags
		0						// Structured Byte Stride: Not totally sure, but I don't think this needs to be set because even though it is a structured buffer, there is only a single element
		// No Initial Data
		);

	if (m_constantBufferArrays[(int)bindingLocation] == nullptr)
		m_constantBufferArrays[(int)bindingLocation] = std::make_shared<ConstantBufferArray>(m_deviceResources, bindingLocation);

	m_constantBufferArrays[(int)bindingLocation]->AddBuffer(buffer);
	AddBindable(m_constantBufferArrays[(int)bindingLocation]);

	if (recursive)
	{
		for (std::unique_ptr<Drawable>& child : m_children)
			child->AddConstantBuffer<T>(bindingLocation, updateFunc, true);
	}

	// Add the constant buffer and update functional to the vector of update functionals
	m_updateFunctions.push_back(std::make_tuple(buffer, std::bind(updateFunc, this, std::placeholders::_1)));
}

template <typename T>
void Drawable::AddConstantBuffer(ConstantBufferBindingLocation bindingLocation, void* initialData, void (Drawable::* updateFunc)(std::shared_ptr<ConstantBuffer>), bool recursive)
{
	// Because we are supplying the update functional, we need to allow this buffer to be mappable
	std::shared_ptr<ConstantBuffer> buffer = std::make_shared<ConstantBuffer>(m_deviceResources);
	buffer->CreateBuffer<T>(
		D3D11_USAGE_DYNAMIC,	// Usage: Dynamic
		D3D11_CPU_ACCESS_WRITE,	// CPU Access: CPU will be able to write using Map
		0,						// Misc Flags: No miscellaneous flags
		0,						// Structured Byte Stride: Not totally sure, but I don't think this needs to be set because even though it is a structured buffer, there is only a single element
		initialData				// Initial Data
		);

	if (m_constantBufferArrays[(int)bindingLocation] == nullptr)
		m_constantBufferArrays[(int)bindingLocation] = std::make_shared<ConstantBufferArray>(m_deviceResources, bindingLocation);

	m_constantBufferArrays[(int)bindingLocation]->AddBuffer(buffer);
	AddBindable(m_constantBufferArrays[(int)bindingLocation]);

	if (recursive)
	{
		for (std::unique_ptr<Drawable>& child : m_children)
			child->AddConstantBuffer<T>(bindingLocation, initialData, updateFunc, true);
	}

	// Add the constant buffer and update functional to the vector of update functionals
	m_updateFunctions.push_back(std::make_tuple(buffer, std::bind(updateFunc, this, std::placeholders::_1)));
}