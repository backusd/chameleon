#pragma once
#include "pch.h"
#include "ModelException.h"
#include "ModelNode.h"
#include "DeviceResources.h"
#include "MoveLookController.h"
#include "ObjectStore.h"
#include "Mesh.h"
#include "BoundingBox.h"

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <memory>
#include <fstream>
#include <vector>

enum class BasicModelType
{
	Plane,
	Cube,
	Sphere,
	Cylinder
};

class Model
{
public:
	Model(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, std::string fileName);
	Model(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, std::shared_ptr<Mesh> mesh);
	Model(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, BasicModelType basicModelType);
	
	void Draw(const DirectX::XMMATRIX& projectionMatrix);
	void Update(const DirectX::XMMATRIX& parentModelMatrix);

	std::shared_ptr<Mesh> GetRootNodeMesh() { return m_rootNode->GetMesh(0); }

#ifndef NDEBUG
	void SetMoveLookController(std::shared_ptr<MoveLookController> mlc) { m_moveLookController = mlc; m_rootNode->SetMoveLookController(mlc); }
#endif

	bool IsMouseHovered(float mouseX, float mouseY, const DirectX::XMMATRIX& modelMatrix, const DirectX::XMMATRIX& projectionMatrix, float& distance);



private:
	void LoadMesh(const aiMesh& mesh, const aiMaterial* const* materials);


	std::shared_ptr<DeviceResources>	m_deviceResources;
	std::shared_ptr<MoveLookController> m_moveLookController;

	std::unique_ptr<ModelNode> m_rootNode;


	// When loading a scene/model via assimp, the meshes are just stored in a flat
	// array. The hierarchy of nodes then just have an index into that array. So, 
	// for our purpose, the model we are building needs to first create a vector
	// of shared pointers to these meshes and then as we build the node hierarchy,
	// we assign out the meshes to the corresponding nodes
	std::vector<std::shared_ptr<Mesh>> m_meshes;

	// BoundingBox to excapsulate the entire Model
	std::unique_ptr<BoundingBox>	m_boundingBox;



	// DEBUG SPECIFIC --------------------------------------------------------
#ifndef NDEBUG
public:
	void DrawImGui(std::string id);

	void DrawBoundingBox(const DirectX::XMMATRIX& parentModelMatrix, const DirectX::XMMATRIX& projectionMatrix);
	bool NeedDrawBoundingBox();
	bool m_drawBoundingBox;

#endif
};