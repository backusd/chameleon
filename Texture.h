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
	Texture(std::shared_ptr<DeviceResources> deviceResources, std::string filename);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture() { return m_textureView; }

private:
	void LoadTarga(std::string filename, int& height, int& width);


	std::shared_ptr<DeviceResources> m_deviceResources;

	unsigned char* m_targaData;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureView;
};