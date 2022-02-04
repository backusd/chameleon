#pragma once
#include "pch.h"

#include "DeviceResources.h"
#include "TextureClass.h"

#include <memory>

class BitmapClass
{
private:
	struct VertexType
	{
		//D3DXVECTOR3 position;
		//D3DXVECTOR2 texture;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
	};

public:
	BitmapClass(std::shared_ptr<DeviceResources> deviceResources, int screenWidth, int screenHeight, CHAR* textureFilename, int bitmapWidth, int bitmapHeight, int locationX, int locationY);
	~BitmapClass();

	void Shutdown();
	bool Render(int, int);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();
	int GetLocationX();
	int GetLocationY();

private:
	bool InitializeBuffers();
	void ShutdownBuffers();
	bool UpdateBuffers(int, int);
	void RenderBuffers();

	bool LoadTexture(CHAR*);
	void ReleaseTexture();

private:
	std::shared_ptr<DeviceResources> m_deviceResources;


	// ID3D10Buffer* m_vertexBuffer, * m_indexBuffer;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;

	int m_vertexCount, m_indexCount;
	TextureClass* m_Texture;
	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;
	int m_locX, m_locY;
};