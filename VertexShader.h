#pragma once
#include "pch.h"
#include "Bindable.h"

class VertexShader : public Bindable
{
public:
	VertexShader(std::shared_ptr<DeviceResources> deviceResources, Microsoft::WRL::ComPtr<ID3DBlob> blob);

	void Bind() override;

private:

	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
};