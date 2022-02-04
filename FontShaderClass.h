#pragma once
#include "pch.h"


#include <fstream>
#include <memory>

#include "DeviceResources.h"

/*

class FontShaderClass
{
public:
	FontShaderClass(std::shared_ptr<DeviceResources> deviceResources, HWND hWnd);
	FontShaderClass(const FontShaderClass&);
	~FontShaderClass();

	bool Initialize(ID3D10Device*, HWND);
	void Shutdown();
	void Render(ID3D10Device*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*, D3DXVECTOR4);

private:
	bool InitializeShader(ID3D10Device*, HWND, CHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, CHAR*);

	void SetShaderParameters(D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*, D3DXVECTOR4);
	void RenderShader(ID3D10Device*, int);

private:
	std::shared_ptr<DeviceResources> m_deviceResources;

	ID3D11Effect* m_effect;
	ID3D10EffectTechnique* m_technique;
	ID3D10InputLayout* m_layout;

	ID3D10EffectMatrixVariable* m_worldMatrixPtr;
	ID3D10EffectMatrixVariable* m_viewMatrixPtr;
	ID3D10EffectMatrixVariable* m_projectionMatrixPtr;
	ID3D10EffectShaderResourceVariable* m_texturePtr;
	ID3D10EffectVectorVariable* m_pixelColorPtr;
};
*/