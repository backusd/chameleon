#include "TextureClass.h"


TextureClass::TextureClass(std::shared_ptr<DeviceResources> deviceResources, CHAR* filename) :
	m_deviceResources(deviceResources),
	m_texture(nullptr)
{
	HRESULT result;

	D3DX11CreateShaderResourceViewFromFile(m_deviceResources->D3DDevice(), filename, NULL, NULL, &m_texture, NULL);
}


TextureClass::~TextureClass()
{
	this->Shutdown();
}



void TextureClass::Shutdown()
{
	// Release the texture resource.
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	return;
}


ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_texture;
}