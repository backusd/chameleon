#pragma once
#include "pch.h"


class LightClass
{
public:
	LightClass();
	~LightClass();

	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);

	DirectX::XMFLOAT4 GetAmbientColor() { return m_ambientColor; }
	DirectX::XMFLOAT4 GetDiffuseColor() { return m_diffuseColor; }
	DirectX::XMFLOAT3 GetDirection() { return m_direction; }

private:
	DirectX::XMFLOAT4 m_ambientColor;
	DirectX::XMFLOAT4 m_diffuseColor;
	DirectX::XMFLOAT3 m_direction;
};