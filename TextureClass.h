#pragma once
#include "pch.h"

#include "DeviceResources.h"

#include <memory>

#include <D3DX11tex.h>



class TextureClass
{
public:
	TextureClass(std::shared_ptr<DeviceResources> deviceResources, CHAR* filename);
	~TextureClass();

	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	std::shared_ptr<DeviceResources> m_deviceResources;

	ID3D11ShaderResourceView* m_texture;

};