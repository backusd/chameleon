#include "PixelShader.h"

PixelShader::PixelShader(std::shared_ptr<DeviceResources> deviceResources, std::wstring pixelShaderFile) :
	Bindable(deviceResources)
{
	INFOMAN(m_deviceResources);

	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(pixelShaderFile.c_str(), &pBlob));
	GFX_THROW_INFO(
		m_deviceResources->D3DDevice()->CreatePixelShader(
			pBlob->GetBufferPointer(), 
			pBlob->GetBufferSize(), 
			nullptr, 
			m_pixelShader.ReleaseAndGetAddressOf()
		)
	);

}

void PixelShader::Bind()
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->PSSetShader(m_pixelShader.Get(), nullptr, 0u)
	);
}