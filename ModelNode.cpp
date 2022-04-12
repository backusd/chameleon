#include "ModelNode.h"

using DirectX::XMFLOAT3;
using DirectX::XMMATRIX;

ModelNode::ModelNode(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController) :
	m_mesh(nullptr),
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

void ModelNode::Draw(XMMATRIX modelMatrix, XMMATRIX projectionMatrix)
{
	INFOMAN(m_deviceResources);

	// Bind the mesh (vertex and index buffers)
	m_mesh->Bind();

	// Update the Model-view-projection constant buffer with the aggregated model matrix
	UpdateModelViewProjectionConstantBuffer(modelMatrix, projectionMatrix);

	// Determine the type of draw call from the mesh
	if (m_mesh->DrawIndexed())
	{
		GFX_THROW_INFO_ONLY(
			m_deviceResources->D3DDeviceContext()->DrawIndexed(m_mesh->IndexCount(), 0u, 0u)
		);
	}
	else
	{
		GFX_THROW_INFO_ONLY(
			m_deviceResources->D3DDeviceContext()->Draw(m_mesh->VertexCount(), 0u)
		);
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
	m_accumulatedModelMatrix = parentModelMatrix * this->GetModelMatrix();
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

#ifndef NDEBUG
void ModelNode::SetMoveLookController(std::shared_ptr<MoveLookController> mlc)
{
	m_moveLookController = mlc;

	// NOTE: Must reference the unique_ptr (cannot be copied)
	for (std::unique_ptr<ModelNode>& node : m_childNodes)
		node->SetMoveLookController(mlc);
}
#endif