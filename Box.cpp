#include "Box.h"

using DirectX::XMFLOAT3;

Box::Box(float sideLength) :
	Renderable(),
	m_xSideLength(sideLength),
	m_ySideLength(sideLength),
	m_zSideLength(sideLength)
{

}

Box::Box(DirectX::XMFLOAT3 position, float sideLength) :
	Renderable(),
	m_xSideLength(sideLength),
	m_ySideLength(sideLength),
	m_zSideLength(sideLength)
{
	m_position = XMFLOAT3(position.x, position.y, position.z);
}

Box::Box(DirectX::XMFLOAT3 position, float xSideLength, float ySideLength, float zSideLength) :
	Renderable(),
	m_xSideLength(xSideLength),
	m_ySideLength(ySideLength),
	m_zSideLength(zSideLength)
{
	m_position = XMFLOAT3(position.x, position.y, position.z);
}

DirectX::XMMATRIX Box::GetScaleMatrix()
{
	return DirectX::XMMatrixScaling(m_xSideLength, m_ySideLength, m_zSideLength);
}

void Box::Update(std::shared_ptr<StepTimer> timer)
{
	// m_position.z += 0.005f;
}