#include "Renderable.h"

using DirectX::XMFLOAT3;
using DirectX::XMMATRIX;

Renderable::Renderable() :
	m_position(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_roll(0.0f),
	m_pitch(0.0f),
	m_yaw(0.0f)
{

}

XMMATRIX Renderable::GetModelMatrix()
{
	return DirectX::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll) *
			GetScaleMatrix() *
			DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
}