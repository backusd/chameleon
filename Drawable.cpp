#include "Drawable.h"

using DirectX::XMFLOAT3;
using DirectX::XMMATRIX;

Drawable::Drawable(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController) :
	m_deviceResources(deviceResources),
	m_moveLookController(moveLookController),
	m_projectionMatrix(DirectX::XMMatrixIdentity()),
	m_position(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_roll(0.0f),
	m_pitch(0.0f),
	m_yaw(0.0f)
{
}

void Drawable::Draw()
{
	INFOMAN(m_deviceResources);

	for (std::shared_ptr<Bindable> bindable : m_bindables)
		bindable->Bind();

	m_mesh->Bind();

	// The PreDrawUpdate function will execute immediately prior to performing the actual Draw call. 
	// This allows updating of any constant buffers or other necessary updates that need to take place
	// before submitting the vertices to be rendered
	PreDrawUpdate();

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
}

void Drawable::UpdateModelViewProjectionConstantBuffer()
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
	XMMATRIX model = this->GetModelMatrix();
	XMMATRIX viewProjection = m_moveLookController->ViewMatrix() * m_projectionMatrix;
	DirectX::XMStoreFloat4x4(&(mappedBuffer->model), model);
	DirectX::XMStoreFloat4x4(&(mappedBuffer->modelViewProjection), model * viewProjection);
	DirectX::XMStoreFloat4x4(&(mappedBuffer->inverseTransposeModel), DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, model)));

	GFX_THROW_INFO_ONLY(
		context->Unmap(vsBuffer.Get(), 0)
	);
}

XMMATRIX Drawable::GetModelMatrix()
{
	return DirectX::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll) *
		GetScaleMatrix() *
		DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
}