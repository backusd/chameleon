#include "TextureArray.h"
#include "ObjectStore.h"

TextureArray::TextureArray(std::shared_ptr<DeviceResources> deviceResources, TextureBindingLocation bindToStage) :
	Bindable(deviceResources)
{
	switch (bindToStage)
	{
	case TextureBindingLocation::COMPUTE_SHADER:	BindFunc = std::bind(&TextureArray::BindCS, this); break;
	case TextureBindingLocation::VERTEX_SHADER:		BindFunc = std::bind(&TextureArray::BindVS, this); break;
	case TextureBindingLocation::HULL_SHADER:		BindFunc = std::bind(&TextureArray::BindHS, this); break;
	case TextureBindingLocation::DOMAIN_SHADER:		BindFunc = std::bind(&TextureArray::BindDS, this); break;
	case TextureBindingLocation::GEOMETRY_SHADER:	BindFunc = std::bind(&TextureArray::BindGS, this); break;
	case TextureBindingLocation::PIXEL_SHADER:		BindFunc = std::bind(&TextureArray::BindPS, this); break;
	}
}

void TextureArray::AddTexture(std::string lookupName)
{
	m_textures.push_back(ObjectStore::GetTexture(lookupName));
}

void TextureArray::Bind()
{
	m_rawTextureViewPointers.clear();
	for (std::shared_ptr<Texture> texture : m_textures)
		m_rawTextureViewPointers.push_back(texture->GetRawTextureViewPointer());

	BindFunc();
}

void TextureArray::BindCS()
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->CSSetShaderResources(0u, static_cast<unsigned int>(m_rawTextureViewPointers.size()), m_rawTextureViewPointers.data())
	);
}

void TextureArray::BindVS()
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->VSSetShaderResources(0u, static_cast<unsigned int>(m_rawTextureViewPointers.size()), m_rawTextureViewPointers.data())
	);
}

void TextureArray::BindHS()
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->HSSetShaderResources(0u, static_cast<unsigned int>(m_rawTextureViewPointers.size()), m_rawTextureViewPointers.data())
	);
}

void TextureArray::BindDS()
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->DSSetShaderResources(0u, static_cast<unsigned int>(m_rawTextureViewPointers.size()), m_rawTextureViewPointers.data())
	);
}

void TextureArray::BindGS()
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->GSSetShaderResources(0u, static_cast<unsigned int>(m_rawTextureViewPointers.size()), m_rawTextureViewPointers.data())
	);
}

void TextureArray::BindPS()
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->PSSetShaderResources(0u, static_cast<unsigned int>(m_rawTextureViewPointers.size()), m_rawTextureViewPointers.data())
	);
}