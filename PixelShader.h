#pragma once
#include "pch.h"
#include "Bindable.h"

class PixelShader : public Bindable
{
public:
	PixelShader(std::shared_ptr<DeviceResources> deviceResources, std::wstring pixelShaderFile);

	void Bind() override;

private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
};