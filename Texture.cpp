#include "Texture.h"

using DirectX::ScratchImage;
using DirectX::TexMetadata;

Texture::Texture(std::shared_ptr<DeviceResources> deviceResources) :
	m_deviceResources(deviceResources)
{
	Reset();
}

void Texture::Reset()
{
	// Load default texture and SRV values
	// (don't load height/width because the Load* function that loads the image file will take care of that)
	m_textureDesc.MipLevels = 0;
	m_textureDesc.ArraySize = 1;
	m_textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_textureDesc.SampleDesc.Count = 1;
	m_textureDesc.SampleDesc.Quality = 0;
	m_textureDesc.Usage = D3D11_USAGE_DEFAULT;
	m_textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	m_textureDesc.CPUAccessFlags = 0;
	m_textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	m_srvDesc.Format = m_textureDesc.Format;
	m_srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	m_srvDesc.Texture2D.MostDetailedMip = 0;
	m_srvDesc.Texture2D.MipLevels = -1;
}

void Texture::Create(std::string filename)
{
	INFOMAN(m_deviceResources);

	std::unique_ptr<DirectX::ScratchImage> scratchImage;

	const std::filesystem::path filePath = filename;
	std::string extension = filePath.extension().string();
	if (extension == ".tga")
	{
		scratchImage = LoadTGAImage(filename);
	}
	else if (extension == ".bmp" || extension == ".jpeg" || extension == ".png")
	{
		scratchImage = LoadWICImage(filename);
	}


	const DirectX::Image* img = scratchImage->GetImage(0, 0, 0);
	assert(img);


	m_textureDesc.Height = img->height;
	m_textureDesc.Width = img->width;

	// Create the empty texture.
	GFX_THROW_INFO(m_deviceResources->D3DDevice()->CreateTexture2D(&m_textureDesc, nullptr, &m_texture));

	// Set the row pitch of the targa image data.
	unsigned int _rowPitch = (img->width * 4) * sizeof(unsigned char);

	// Copy the targa image data into the texture.
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->UpdateSubresource(m_texture.Get(), 0, NULL, img->pixels, _rowPitch, 0)
	);

	// Create the shader resource view for the texture.
	GFX_THROW_INFO(
		m_deviceResources->D3DDevice()->CreateShaderResourceView(m_texture.Get(), &m_srvDesc, m_textureView.ReleaseAndGetAddressOf())
	);

	// Generate mipmaps for this texture.
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->GenerateMips(m_textureView.Get())
	);
}

std::unique_ptr<ScratchImage> Texture::LoadWICImage(std::string filename)
{
	INFOMAN(m_deviceResources);

	std::unique_ptr<ScratchImage> image = std::make_unique<DirectX::ScratchImage>();
	DirectX::TexMetadata metadata;
	std::wstring wideFilename = std::wstring(filename.begin(), filename.end());
	GFX_THROW_INFO(
		DirectX::LoadFromWICFile(wideFilename.c_str(), DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &metadata, *image)
	);

	return std::move(image);
}

std::unique_ptr<ScratchImage> Texture::LoadTGAImage(std::string filename)
{
	INFOMAN(m_deviceResources);

	std::unique_ptr<ScratchImage> image = std::make_unique<DirectX::ScratchImage>();
	DirectX::TexMetadata metadata;
	std::wstring wideFilename = std::wstring(filename.begin(), filename.end());
	GFX_THROW_INFO(
		DirectX::LoadFromTGAFile(wideFilename.c_str(), DirectX::TGA_FLAGS::TGA_FLAGS_NONE, &metadata, *image)
	);

	return std::move(image);
}