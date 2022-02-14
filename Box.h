#pragma once
#include "pch.h"
#include "Renderable.h"
#include "StepTimer.h"

#include <memory>



class Box : public Renderable
{
public:
	Box(float m_sideLength);
	Box(DirectX::XMFLOAT3 position, float sideLength);
	Box(DirectX::XMFLOAT3 position, float xSideLength, float ySideLength, float zSideLength);

	DirectX::XMMATRIX GetScaleMatrix() override;
	
	void Update(std::shared_ptr<StepTimer> timer);

private:
	float m_xSideLength;
	float m_ySideLength;
	float m_zSideLength;
};