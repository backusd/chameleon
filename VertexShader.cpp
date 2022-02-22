#include "VertexShader.h"

VertexShader::VertexShader(std::shared_ptr<DeviceResources> deviceResources, Microsoft::WRL::ComPtr<ID3DBlob> blob) : 
	Bindable(deviceResources)
{
	INFOMAN(m_deviceResources);

	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	// GFX_THROW_INFO(D3DReadFileToBlob(vertexShaderFile.c_str(), &pBlob));
	GFX_THROW_INFO(
		m_deviceResources->D3DDevice()->CreateVertexShader(
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			nullptr,
			m_vertexShader.ReleaseAndGetAddressOf()
		)
	);
}

void VertexShader::Bind()
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->VSSetShader(m_vertexShader.Get(), nullptr, 0u)
	);
}