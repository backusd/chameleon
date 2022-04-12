#pragma once
#include "pch.h"
#include "Mesh.h"
#include "MoveLookController.h"
#include "HLSLStructures.h"

#include <string>
#include <memory>
#include <vector>




class ModelNode
{
public:
	ModelNode(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController);

	void Draw(DirectX::XMMATRIX modelMatrix, DirectX::XMMATRIX projectionMatrix);

	void SetName(std::string name) { m_nodeName = name; }
	void SetMesh(std::shared_ptr<Mesh> mesh) { m_mesh = mesh; }

	void CreateMesh(std::vector<OBJVertex>& vertices, std::vector<unsigned short>& indices);
	std::shared_ptr<Mesh> GetMesh() { return m_mesh; }

	void CreateChildNode(std::string nodeName, std::vector<OBJVertex>& vertices, std::vector<unsigned short>& indices);


#ifndef NDEBUG
	void SetMoveLookController(std::shared_ptr<MoveLookController> mlc);
#endif

private:
	void UpdateModelViewProjectionConstantBuffer(DirectX::XMMATRIX parentModelMatrix, DirectX::XMMATRIX projectionMatrix);
	XMMATRIX GetModelMatrix();
	XMMATRIX GetScaleMatrix() { return DirectX::XMMatrixIdentity(); }

	std::shared_ptr<DeviceResources> m_deviceResources;
	std::shared_ptr<MoveLookController> m_moveLookController;

	std::string m_nodeName;
	std::shared_ptr<Mesh> m_mesh;
	std::vector<std::unique_ptr<ModelNode>> m_childNodes;

	// Rotation about the internal center point
	float m_roll;
	float m_pitch;
	float m_yaw;

	DirectX::XMFLOAT3 m_translation;
	DirectX::XMMATRIX m_accumulatedModelMatrix;

};