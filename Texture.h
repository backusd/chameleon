#pragma once
#include "pch.h"
#include "DeviceResources.h"
#include "DeviceResourcesException.h"
#include "TextureException.h"

#include <memory>
#include <string>
#include <stdio.h>

class Texture
{
private:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

public:
	Texture(std::shared_ptr<DeviceResources> deviceResources);

	void TextureMipLevels(unsigned int levels) { m_textureDesc.MipLevels = levels; }
	void TextureArraySize(unsigned int size) { m_textureDesc.ArraySize = size; }
	void TextureFormat(DXGI_FORMAT format) { m_textureDesc.Format = format; m_srvDesc.Format = format; } // Also update the SRV format
	void TextureSampleDescCount(unsigned int count) { m_textureDesc.SampleDesc.Count = count; }
	void TextureSampleDescQuality(unsigned int quality) { m_textureDesc.SampleDesc.Quality = quality; }
	void TextureUsage(D3D11_USAGE usage) { m_textureDesc.Usage = usage; }
	void TextureBindFlags(unsigned int flags) { m_textureDesc.BindFlags = flags; }
	void TextureCPUAccessFlags(unsigned int flags) { m_textureDesc.CPUAccessFlags = flags; }
	void TextureMiscFlags(unsigned int flags) { m_textureDesc.MiscFlags = flags; }

	void SRVMostDetailedMip(unsigned int mip) { m_srvDesc.Texture2D.MostDetailedMip = 0; }	
	void SRVMipLevels(unsigned int levels) { m_srvDesc.Texture2D.MipLevels = -1; }

	void Reset();

	void LoadTarga(std::string filename);

	ID3D11ShaderResourceView* GetRawTextureViewPointer() { return m_textureView.Get(); }

private:
	std::shared_ptr<DeviceResources> m_deviceResources;

	D3D11_TEXTURE2D_DESC			m_textureDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC m_srvDesc;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureView;
};