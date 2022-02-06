#pragma once
#include "pch.h"


class CameraClass
{
public:
	CameraClass();
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetRotation();

	void Render();
	DirectX::XMMATRIX GetViewMatrix() { return m_viewMatrix; }

	void GenerateBaseViewMatrix();
	DirectX::XMMATRIX GetBaseViewMatrix() { return m_baseViewMatrix; }

	void RenderReflection(float);
	DirectX::XMMATRIX GetReflectionViewMatrix() { return m_reflectionViewMatrix; }

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	
	DirectX::XMMATRIX m_viewMatrix;
	DirectX::XMMATRIX m_baseViewMatrix;
	DirectX::XMMATRIX m_reflectionViewMatrix;
};
