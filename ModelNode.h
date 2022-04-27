#pragma once
#include "pch.h"
#include "ModelNodeException.h"
#include "Mesh.h"
#include "MoveLookController.h"
#include "HLSLStructures.h"

#include <string>
#include <memory>
#include <vector>
#include <ostream>

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class ModelNode
{
public:
	ModelNode(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController);
	ModelNode(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, const aiNode& node, std::vector<std::shared_ptr<Mesh>> meshes);

	void Draw(const DirectX::XMMATRIX& projectionMatrix);
	void Update(const DirectX::XMMATRIX& parentModelMatrix);

	void SetName(std::string name) { m_nodeName = name; }
	void AddMesh(std::shared_ptr<Mesh> mesh) { m_meshes.push_back(mesh); }

	std::shared_ptr<Mesh> GetMesh(int index) { return m_meshes[index]; }

	bool IsMouseHovered(const DirectX::XMVECTOR& clickPointNear, 
						const DirectX::XMVECTOR& clickPointFar, 
						const DirectX::XMMATRIX& projectionMatrix, 
						float& distance);

	void GetBoundingBoxPositionsWithTransformation(const DirectX::XMMATRIX& parentModelMatrix, std::vector<DirectX::XMVECTOR>& positions);


#ifndef NDEBUG
	void SetMoveLookController(std::shared_ptr<MoveLookController> mlc);
#endif

private:
	void UpdateModelViewProjectionConstantBuffer(const DirectX::XMMATRIX& projectionMatrix);
	DirectX::XMMATRIX GetModelMatrix();

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
	DirectX::XMFLOAT3 m_scaling;
	DirectX::XMMATRIX m_accumulatedModelMatrix;


	// DEBUG SPECIFIC --------------------------------------------------------
#ifndef NDEBUG
public:
	void DrawImGui(std::string id);

	void DrawBoundingBox(const DirectX::XMMATRIX& projectionMatrix);
	bool NeedDrawBoundingBox();

private:
	bool m_drawBoundingBox;
#endif

};
