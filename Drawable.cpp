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

	UINT indexCount = m_mesh->IndexCount();
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->DrawIndexed(indexCount, 0u, 0u)
	);
}

XMMATRIX Drawable::GetModelMatrix()
{
	return DirectX::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll) *
		GetScaleMatrix() *
		DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
}