#include "Box.h"

using DirectX::XMFLOAT3;

Box::Box(float sideLength) :
	Renderable(),
	m_sideLength(sideLength)
{

}

Box::Box(DirectX::XMFLOAT3 position, float sideLength) :
	Renderable(),
	m_sideLength(sideLength)
{
	m_position = XMFLOAT3(position.x, position.y, position.z);
}

DirectX::XMMATRIX Box::GetScaleMatrix()
{
	return DirectX::XMMatrixScaling(m_sideLength, m_sideLength, m_sideLength);
}

void Box::Update(std::shared_ptr<StepTimer> timer)
{
	// m_position.z += 0.005f;
}