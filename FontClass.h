#pragma once
#include "pch.h"

#include <fstream>
#include <memory>

#include "TextureClass.h"
#include "DeviceResources.h"



class FontClass
{
private:
	struct FontType
	{
		float left, right;
		int size;
	};

	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
	};

public:
	FontClass(std::shared_ptr<DeviceResources> deviceResources, char* fontFilename, CHAR* textureFilename);
	~FontClass();

	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

	void BuildVertexArray(void*, char*, float, float);

private:
	bool LoadFontData(char*);
	void ReleaseFontData();
	bool LoadTexture(CHAR*);
	void ReleaseTexture();

private:
	std::shared_ptr<DeviceResources> m_deviceResources;


	FontType* m_Font;
	TextureClass* m_Texture;
};