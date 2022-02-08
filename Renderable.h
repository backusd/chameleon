#pragma once
#include "pch.h"
#include "StepTimer.h"

#include <memory>

class Renderable
{
public:
	Renderable();


	// Every object should provide how to scale itself
	virtual DirectX::XMMATRIX GetScaleMatrix() { return DirectX::XMMatrixIdentity(); }

	DirectX::XMMATRIX GetModelMatrix();

	virtual void Update(std::shared_ptr<StepTimer> timer) = 0;

protected:
	DirectX::XMFLOAT3 m_position; // Every object will have a "center point" location
	
	// Rotation about the internal center point
	float m_roll;
	float m_pitch;
	float m_yaw;
};

