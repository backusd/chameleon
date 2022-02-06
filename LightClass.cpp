#include "LightClass.h"

using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;


LightClass::LightClass() : 
	m_ambientColor(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)),
	m_diffuseColor(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)),
	m_direction(XMFLOAT3(0.0f, 0.0f, 0.0f))
{
}


LightClass::~LightClass()
{
}


void LightClass::SetAmbientColor(float red, float green, float blue, float alpha)
{
	m_ambientColor = XMFLOAT4(red, green, blue, alpha);
	return;
}


void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = XMFLOAT4(red, green, blue, alpha);
	return;
}


void LightClass::SetDirection(float x, float y, float z)
{
	m_direction = XMFLOAT3(x, y, z);
	return;
}
