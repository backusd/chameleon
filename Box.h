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

	DirectX::XMMATRIX GetScaleMatrix() override;
	
	void Update(std::shared_ptr<StepTimer> timer);

private:
	float m_sideLength;
};