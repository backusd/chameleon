#pragma once
#include "pch.h"
#include "ModelNodeException.h"
#include "Mesh.h"
#include "MoveLookController.h"
#include "HLSLStructures.h"

#include <string>
#include <memory>
#include <vector>

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class ModelNode
{
public:
	ModelNode(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController);
	ModelNode(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, const aiNode& node, std::vector<std::shared_ptr<Mesh>> meshes);

	void Draw(DirectX::XMMATRIX parentModelMatrix, DirectX::XMMATRIX projectionMatrix);

	void SetName(std::string name) { m_nodeName = name; }
	void AddMesh(std::shared_ptr<Mesh> mesh) { m_meshes.push_back(mesh); }

	template <typename T, typename A>
	std::shared_ptr<Mesh> CreateMesh(std::vector<T, A>& vertices, std::vector<unsigned short>& indices);

	template <typename T, typename A>
	std::shared_ptr<Mesh> CreateChildNode(std::string nodeName, std::vector<T, A>& vertices, std::vector<unsigned short>& indices);
	std::shared_ptr<Mesh> CreateChildNode(std::string nodeName, std::shared_ptr<Mesh> mesh);

	std::shared_ptr<Mesh> GetMesh(int index) { return m_meshes[index]; }

#ifndef NDEBUG
	void SetMoveLookController(std::shared_ptr<MoveLookController> mlc);
#endif

private:
	void UpdateModelViewProjectionConstantBuffer(DirectX::XMMATRIX parentModelMatrix, DirectX::XMMATRIX projectionMatrix);
	DirectX::XMMATRIX GetModelMatrix();
	DirectX::XMMATRIX GetScaleMatrix() { return DirectX::XMMatrixIdentity(); }

	std::shared_ptr<DeviceResources> m_deviceResources;
	std::shared_ptr<MoveLookController> m_moveLookController;

	std::string m_nodeName;
	std::vector<std::shared_ptr<Mesh>> m_meshes;
	std::vector<std::unique_ptr<ModelNode>> m_childNodes;

	// Rotation about the internal center point
	float m_roll;
	float m_pitch;
	float m_yaw;

	DirectX::XMFLOAT3 m_translation;
	DirectX::XMMATRIX m_accumulatedModelMatrix;


	// DEBUG SPECIFIC --------------------------------------------------------
#ifndef NDEBUG
public:
	void DrawImGui(std::string id);

#endif

};

template <typename T, typename A>
std::shared_ptr<Mesh> ModelNode::CreateMesh(std::vector<T, A>& vertices, std::vector<unsigned short>& indices)
{
	m_meshes.push_back(std::make_shared<Mesh>(m_deviceResources));
	m_meshes.back()->LoadBuffers<T, A>(vertices, indices);

	// Return the newly created mesh so that the loading code can optionally add this mesh to ObjectStore
	return m_meshes.back();
}

template <typename T, typename A>
std::shared_ptr<Mesh> ModelNode::CreateChildNode(std::string nodeName, std::vector<T, A>& vertices, std::vector<unsigned short>& indices)
{
	m_childNodes.push_back(std::make_unique<ModelNode>(m_deviceResources, m_moveLookController));
	m_childNodes.back()->SetName(nodeName);
	m_childNodes.back()->CreateMesh<T, A>(vertices, indices);

	// Return the newly created mesh so that the loading code can optionally add this mesh to ObjectStore
	return m_childNodes.back()->GetMesh(0);
}