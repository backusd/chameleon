#pragma once
#include "pch.h"
#include "ModelException.h"
#include "ModelNode.h"
#include "DeviceResources.h"
#include "MoveLookController.h"
#include "ObjectStore.h"

#include <string>
#include <memory>
#include <filesystem>
#include <fstream>


class Model
{
public:
	Model(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, std::string fileName);
	Model(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, std::shared_ptr<Mesh> mesh);
	
	void Draw(DirectX::XMMATRIX parentModelMatrix, DirectX::XMMATRIX projectionMatrix);

	std::shared_ptr<Mesh> GetRootNodeMesh() { return m_rootNode->GetMesh(); }

#ifndef NDEBUG
	void SetMoveLookController(std::shared_ptr<MoveLookController> mlc) { m_moveLookController = mlc; m_rootNode->SetMoveLookController(mlc); }
#endif



private:
	void Load(std::string filename);

	// OBJ specific loading methods
	void OBJLoadFile(std::string filename);
	bool OBJMeshesAlreadyLoaded(std::string filename);
	void OBJLoadMeshesFromObjectStore(std::string filename);
	void OBJGetPositionsTexturesNormals(std::string filename, std::vector<DirectX::XMFLOAT3>& positions, std::vector<DirectX::XMFLOAT2>& textures, std::vector<DirectX::XMFLOAT3>& normals);
	void OBJCreateVertices(std::string filename, std::vector<DirectX::XMFLOAT3>& positions, std::vector<DirectX::XMFLOAT2>& textures, std::vector<DirectX::XMFLOAT3>& normals);

	std::shared_ptr<DeviceResources>	m_deviceResources;
	std::shared_ptr<MoveLookController> m_moveLookController;

	std::unique_ptr<ModelNode> m_rootNode;


	// DEBUG SPECIFIC --------------------------------------------------------
#ifndef NDEBUG
public:
	void DrawImGui(std::string id);

#endif
};