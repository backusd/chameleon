#include "ModelNode.h"

using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4X4;
using DirectX::XMMATRIX;
using DirectX::XMVECTOR;
using DirectX::operator*;

ModelNode::ModelNode(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController) :
	m_deviceResources(deviceResources),
	m_moveLookController(moveLookController),
	m_roll(0.0f),
	m_pitch(0.0f),
	m_yaw(0.0f),
	m_translation(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_scaling(XMFLOAT3(1.0f, 1.0f, 1.0f)),
	m_accumulatedModelMatrix(DirectX::XMMatrixIdentity())
#ifndef NDEBUG
	, m_drawBoundingBox(false)
#endif
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
	m_scaling(XMFLOAT3(1.0f, 1.0f, 1.0f)),
	m_accumulatedModelMatrix(DirectX::XMMatrixIdentity())
#ifndef NDEBUG
	, m_drawBoundingBox(false)
#endif
{
	m_nodeName = std::string(node.mName.C_Str());

	// Decompose the transformation to get the individual components
	XMMATRIX transform = DirectX::XMMatrixTranspose(
		DirectX::XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&node.mTransformation))
	);
	XMVECTOR scale, rotation, translation;
	DirectX::XMMatrixDecompose(&scale, &rotation, &translation, transform);

	XMFLOAT3 _rotation;
	DirectX::XMStoreFloat3(&m_scaling, scale);
	DirectX::XMStoreFloat3(&_rotation, rotation);
	DirectX::XMStoreFloat3(&m_translation, translation);

	if (_rotation.x != 0.0f || _rotation.y != 0.0f || _rotation.z != 0.0f)
	{
		std::ostringstream oss;
		oss << "Model node '" << m_nodeName << "' has non-unity rotation transform:" << std::endl;
		oss << "   " << _rotation.x << ", " << _rotation.y << ", " << _rotation.z << std::endl;
		oss << "This is not yet supported";
		throw ModelNodeException(__LINE__, __FILE__, oss.str());
	}

	// NOTE: the node is NOT required to have a mesh. In the case of OBJ files, 
	// the root node is basically an empty node that houses all the children nodes
	m_meshes.reserve(node.mNumMeshes);
	for (unsigned int iii = 0; iii < node.mNumMeshes; ++iii)
		m_meshes.push_back(meshes[node.mMeshes[iii]]);		// Reminder: node.mMeshes is just an int array where each int is an index into the all meshes array

	for (unsigned int iii = 0; iii < node.mNumChildren; ++iii)
		m_childNodes.push_back(std::make_unique<ModelNode>(deviceResources, moveLookController, *node.mChildren[iii], meshes));
}

void ModelNode::Draw(const XMMATRIX& parentModelMatrix, const XMMATRIX& projectionMatrix)
{
	INFOMAN(m_deviceResources);

	// First thing to do is to update the accumulated model matrix
	// This is important even for when there are no meshes, because the
	// parent model matrix must still make its way down to the children
	// NOTE: Must post-multiply the parent transform
	m_accumulatedModelMatrix = this->GetModelMatrix() * parentModelMatrix;

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

void ModelNode::UpdateModelViewProjectionConstantBuffer(const XMMATRIX& parentModelMatrix, const XMMATRIX& projectionMatrix)
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
		DirectX::XMMatrixScaling(m_scaling.x, m_scaling.y, m_scaling.z) *
		DirectX::XMMatrixTranslation(m_translation.x, m_translation.y, m_translation.z);
}

bool ModelNode::IsMouseHovered(const XMVECTOR& clickPointNear, const XMVECTOR& clickPointFar, const XMMATRIX& projectionMatrix, float& distance)
{
	// NOTE: We do NOT need to pass the parent's model matrix into this function because this class keeps
	//		 track of the accumulated model matrix. Assuming the Update is done correctly, this class will
	//		 already have an up-to-date model matrix

	// Compute the ray origin and ray direction vector
	XMVECTOR rayOrigin, rayDestination, rayDirection;

	D3D11_VIEWPORT viewport = m_deviceResources->GetScreenViewport();
	XMMATRIX viewMatrix = m_moveLookController->ViewMatrix();

	// Here, we use the identity matrix for the World matrix because we don't want to translate
	// the vectors as if they were at the origin. If we did want to do that, we would use XMMatrixTranslation(eye.x, eye.y, eye.z)
	rayOrigin = XMVector3Unproject(
		clickPointNear,
		viewport.TopLeftX, viewport.TopLeftY,
		viewport.Width, viewport.Height,
		0, 1,
		projectionMatrix,
		viewMatrix,
		m_accumulatedModelMatrix);

	rayDestination = XMVector3Unproject(
		clickPointFar,
		viewport.TopLeftX, viewport.TopLeftY,
		viewport.Width, viewport.Height,
		0, 1,
		projectionMatrix,
		viewMatrix,
		m_accumulatedModelMatrix);

	rayDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(rayDestination, rayOrigin));


	// First, run the test for the meshes of this node
	float dist;
	bool found = false;
	distance = FLT_MAX;

	//     ModelNode can have multiple meshes, so loop over each one
	for (std::shared_ptr<Mesh> mesh : m_meshes)
	{
		if (mesh->RayIntersectionTest(rayOrigin, rayDirection, dist))
		{
			distance = std::min(distance, dist);
			found = true;
		}
	}

	// Second, run the test for all child nodes
	for (std::unique_ptr<ModelNode>& node : m_childNodes)
	{
		if (node->IsMouseHovered(clickPointNear, clickPointFar, projectionMatrix, dist))
		{
			distance = std::min(distance, dist);
			found = true;
		}
	}

	return found;	
}

void ModelNode::GetBoundingBoxPositionsWithTransformation(const XMMATRIX& parentModelMatrix, std::vector<XMVECTOR>& positions)
{
	// Get all positions for the meshes this node owns
	for (std::shared_ptr<Mesh> mesh : m_meshes)
		mesh->GetBoundingBoxPositionsWithTransformation(GetModelMatrix() * parentModelMatrix, positions);

	// Get all positions for all children nodes
	for (std::unique_ptr<ModelNode>& node : m_childNodes)
		node->GetBoundingBoxPositionsWithTransformation(GetModelMatrix() * parentModelMatrix, positions);
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
		ImGui::Checkbox(("Draw Bounding Box##" + id).c_str(), &m_drawBoundingBox);
		ImGui::Text("Translation (prior to rotation):");
		ImGui::Text("    X: "); ImGui::SameLine(); ImGui::DragFloat(("##modelNodePositionX" + id).c_str(), &m_translation.x, 0.05f, -FLT_MAX, FLT_MAX, "%.01f", ImGuiSliderFlags_None);
		ImGui::Text("    Y: "); ImGui::SameLine(); ImGui::DragFloat(("##modelNodePositionY" + id).c_str(), &m_translation.y, 0.05f, -FLT_MAX, FLT_MAX, "%.01f", ImGuiSliderFlags_None);
		ImGui::Text("    Z: "); ImGui::SameLine(); ImGui::DragFloat(("##modelNodePositionZ" + id).c_str(), &m_translation.z, 0.05f, -FLT_MAX, FLT_MAX, "%.01f", ImGuiSliderFlags_None);

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

bool ModelNode::NeedDrawBoundingBox()
{
	// Return true if any node needs its boundingbox to be drawn
	if (m_drawBoundingBox)
		return true;

	for (std::unique_ptr<ModelNode>& node : m_childNodes)
		if (node->NeedDrawBoundingBox())
			return true;

	return false;
}

void ModelNode::DrawBoundingBox(const XMMATRIX& projectionMatrix)
{
	// Draw the bounding box for the model if necessary then pass the call to the root node
	if (m_drawBoundingBox)
	{
		for (std::shared_ptr<Mesh> mesh : m_meshes)
			mesh->DrawBoundingBox(m_accumulatedModelMatrix, m_moveLookController->ViewMatrix(), projectionMatrix);
	}

	for (std::unique_ptr<ModelNode>& node : m_childNodes)
		node->DrawBoundingBox(projectionMatrix);
}
#endif