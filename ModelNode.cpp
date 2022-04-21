#include "ModelNode.h"

using DirectX::XMFLOAT3;
using DirectX::XMMATRIX;
using DirectX::operator*;

ModelNode::ModelNode(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController) :
	m_deviceResources(deviceResources),
	m_moveLookController(moveLookController),
	m_roll(0.0f),
	m_pitch(0.0f),
	m_yaw(0.0f),
	m_translation(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_accumulatedModelMatrix(DirectX::XMMatrixIdentity())
{
	// Leave m_mesh as nullptr until CreateMesh is called because vertex loading code 
	// will check to see if m_mesh is nullptr to know if it has loaded the mesh yet
}

ModelNode::ModelNode(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, const aiNode& node, std::vector<std::shared_ptr<Mesh>> meshes) :
	m_deviceResources(deviceResources),
	m_moveLookController(moveLookController),
	m_roll(0.0f),
	m_pitch(0.0f),
	m_yaw(0.0f),
	m_translation(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_accumulatedModelMatrix(DirectX::XMMatrixIdentity())
{
	m_nodeName = std::string(node.mName.C_Str());

	// NOTE: the node is NOT required to have a mesh. In the case of OBJ files, 
	// the root node is basically an empty node that houses all the children nodes
	m_meshes.reserve(node.mNumMeshes);
	for (unsigned int iii = 0; iii < node.mNumMeshes; ++iii)
		m_meshes.push_back(meshes[node.mMeshes[iii]]);		// Reminder: node.mMeshes is just an int array where each int is an index into the all meshes array

	for (unsigned int iii = 0; iii < node.mNumChildren; ++iii)
		m_childNodes.push_back(std::make_unique<ModelNode>(deviceResources, moveLookController, *node.mChildren[iii], meshes));
}

void ModelNode::Draw(XMMATRIX parentModelMatrix, XMMATRIX projectionMatrix)
{
	INFOMAN(m_deviceResources);

	// First thing to do is to update the accumulated model matrix
	// This is important even for when there are no meshes, because the
	// parent model matrix must still make its way down to the children
	m_accumulatedModelMatrix = parentModelMatrix * this->GetModelMatrix();

	// Bind the mesh (vertex and index buffers)
	for (std::shared_ptr<Mesh> mesh : m_meshes)
	{
		mesh->Bind();

		// Update the Model-view-projection constant buffer with the aggregated model matrix
		UpdateModelViewProjectionConstantBuffer(parentModelMatrix, projectionMatrix);

		// Determine the type of draw call from the mesh
		if (mesh->DrawIndexed())
		{
			GFX_THROW_INFO_ONLY(
				m_deviceResources->D3DDeviceContext()->DrawIndexed(mesh->IndexCount(), 0u, 0u)
			);
		}
		else
		{
			GFX_THROW_INFO_ONLY(
				m_deviceResources->D3DDeviceContext()->Draw(mesh->VertexCount(), 0u)
			);
		}
	}


	// NOTE: Must reference the unique_ptr (cannot be copied)
	for (std::unique_ptr<ModelNode>& node : m_childNodes)
		node->Draw(m_accumulatedModelMatrix, projectionMatrix);
}

void ModelNode::UpdateModelViewProjectionConstantBuffer(XMMATRIX parentModelMatrix, XMMATRIX projectionMatrix)
{
	INFOMAN(m_deviceResources);

	ID3D11DeviceContext4* context = m_deviceResources->D3DDeviceContext();
	D3D11_MAPPED_SUBRESOURCE ms;
	ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));

	// Update VS constant buffer with model/view/projection info
	Microsoft::WRL::ComPtr<ID3D11Buffer> vsBuffer;
	GFX_THROW_INFO_ONLY(
		context->VSGetConstantBuffers(0, 1, vsBuffer.ReleaseAndGetAddressOf())
	);

	GFX_THROW_INFO(
		context->Map(vsBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)
	);

	ModelViewProjectionConstantBuffer* mappedBuffer = (ModelViewProjectionConstantBuffer*)ms.pData;

	XMMATRIX viewProjection = m_moveLookController->ViewMatrix() * projectionMatrix;
	DirectX::XMStoreFloat4x4(&(mappedBuffer->model), m_accumulatedModelMatrix);
	DirectX::XMStoreFloat4x4(&(mappedBuffer->modelViewProjection), m_accumulatedModelMatrix * viewProjection);
	DirectX::XMStoreFloat4x4(&(mappedBuffer->inverseTransposeModel), DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, m_accumulatedModelMatrix)));

	GFX_THROW_INFO_ONLY(
		context->Unmap(vsBuffer.Get(), 0)
	);
}

XMMATRIX ModelNode::GetModelMatrix()
{
	return DirectX::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll) *
		GetScaleMatrix() *
		DirectX::XMMatrixTranslation(m_translation.x, m_translation.y, m_translation.z);
}

std::shared_ptr<Mesh> ModelNode::CreateChildNode(std::string nodeName, std::shared_ptr<Mesh> mesh)
{
	m_childNodes.push_back(std::make_unique<ModelNode>(m_deviceResources, m_moveLookController));
	m_childNodes.back()->SetName(nodeName);
	m_childNodes.back()->AddMesh(mesh);

	// Return the mesh so that the loading code can optionally add this mesh to ObjectStore
	return mesh;
}





#ifndef NDEBUG
void ModelNode::SetMoveLookController(std::shared_ptr<MoveLookController> mlc)
{
	m_moveLookController = mlc;

	// NOTE: Must reference the unique_ptr (cannot be copied)
	for (std::unique_ptr<ModelNode>& node : m_childNodes)
		node->SetMoveLookController(mlc);
}

void ModelNode::DrawImGui(std::string id)
{
	std::string treeNodeName = (m_nodeName == "") ? "Unnamed Node##" + id : m_nodeName + "##" + id;
	if (ImGui::TreeNode(treeNodeName.c_str()))
	{
		ImGui::Text("Position:");
		ImGui::Text("    X: "); ImGui::SameLine(); ImGui::SliderFloat(("##modelNodePositionX" + m_nodeName + id).c_str(), &m_translation.x, -100.0f, 100.0f, "%.3f");
		ImGui::Text("    Y: "); ImGui::SameLine(); ImGui::SliderFloat(("##modelNodePositionY" + m_nodeName + id).c_str(), &m_translation.y, -100.0f, 100.0f, "%.3f");
		ImGui::Text("    Z: "); ImGui::SameLine(); ImGui::SliderFloat(("##modelNodePositionZ" + m_nodeName + id).c_str(), &m_translation.z, -100.0f, 100.0f, "%.3f");

		ImGui::Text("Orientation:");
		ImGui::Text("   Roll:  "); ImGui::SameLine(); ImGui::SliderFloat(("##modelNodeRoll"  + m_nodeName + id).c_str(), &m_roll,  -DirectX::XM_2PI, DirectX::XM_2PI, "%.3f");
		ImGui::Text("   Pitch: "); ImGui::SameLine(); ImGui::SliderFloat(("##modelNodePitch" + m_nodeName + id).c_str(), &m_pitch, -DirectX::XM_2PI, DirectX::XM_2PI, "%.3f");
		ImGui::Text("   Yaw:   "); ImGui::SameLine(); ImGui::SliderFloat(("##modelNodeYaw"   + m_nodeName + id).c_str(), &m_yaw,   -DirectX::XM_2PI, DirectX::XM_2PI, "%.3f");

		// NOTE: Must reference the unique_ptr (cannot be copied)
		for (std::unique_ptr<ModelNode>& node : m_childNodes)
			node->DrawImGui(id);

		ImGui::TreePop();
	}
}
#endif