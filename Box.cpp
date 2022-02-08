#include "Box.h"


Box::Box(float sideLength) :
	Renderable(),
	m_sideLength(sideLength)
{

}

DirectX::XMMATRIX Box::GetScaleMatrix()
{
	return DirectX::XMMatrixScaling(m_sideLength, m_sideLength, m_sideLength);
}

void Box::Update(std::shared_ptr<StepTimer> timer)
{
	// m_position.z += 0.005f;
}