#include "ObjectStore.h"

std::shared_ptr<DeviceResources> ObjectStore::m_deviceResources = nullptr;

//std::map<std::string, Microsoft::WRL::ComPtr<ID3D11VertexShader>>	 ObjectStore::m_vertexShaderMap;
//std::map<std::string, Microsoft::WRL::ComPtr<ID3D11InputLayout>>	 ObjectStore::m_inputLayoutMap;
//std::map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>>	 ObjectStore::m_pixelShaderMap;
std::map<std::string, std::shared_ptr<VertexShader>>	ObjectStore::m_vertexShaderMap;
std::map<std::string, std::shared_ptr<InputLayout>>		ObjectStore::m_inputLayoutMap;
std::map<std::string, std::shared_ptr<PixelShader>>		ObjectStore::m_pixelShaderMap;


std::map<std::string, Microsoft::WRL::ComPtr<ID3D11Buffer>>			 ObjectStore::m_constantBufferMap;
std::map<std::string, Microsoft::WRL::ComPtr<ID3D11RasterizerState>> ObjectStore::m_rasterStateMap;
std::map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>>	 ObjectStore::m_samplerStateMap;

std::map<std::string, std::shared_ptr<Mesh>>    ObjectStore::m_meshMap;
std::map<std::string, std::shared_ptr<Texture>> ObjectStore::m_textureMap;
std::map<std::string, std::shared_ptr<DepthStencilState>> ObjectStore::m_depthStencilStateMap;
std::map<std::string, std::shared_ptr<TerrainMesh>> ObjectStore::m_terrainMeshMap;





void ObjectStore::Initialize(std::shared_ptr<DeviceResources> deviceResources)
{
	m_deviceResources = deviceResources;
}

void ObjectStore::DestructObjects()
{
	m_deviceResources = nullptr;
	m_vertexShaderMap.clear();
	m_inputLayoutMap.clear();
	m_pixelShaderMap.clear();
}

/*
void ObjectStore::AddVertexShaderAndInputLayout(std::wstring vertexShaderFile, const D3D11_INPUT_ELEMENT_DESC* inputDescription, UINT numElements, std::string lookupName)
{
	INFOMAN(m_deviceResources);

	// Vertex Shader
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(vertexShaderFile.c_str(), &pBlob));
	GFX_THROW_INFO(m_deviceResources->D3DDevice()->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));

	m_vertexShaderMap.insert(std::pair(lookupName, pVertexShader));

	// Input Layout
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	GFX_THROW_INFO(m_deviceResources->D3DDevice()->CreateInputLayout(
		inputDescription, numElements,
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		&pInputLayout
	));

	m_inputLayoutMap.insert(std::pair(lookupName, pInputLayout));
}

void ObjectStore::AddPixelShader(std::wstring fileName, std::string lookupName)
{
	INFOMAN(m_deviceResources);

	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(fileName.c_str(), &pBlob));
	GFX_THROW_INFO(m_deviceResources->D3DDevice()->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

	m_pixelShaderMap.insert(std::pair(lookupName, pPixelShader));
}
*/

void ObjectStore::AddRasterState(D3D11_RASTERIZER_DESC desc, std::string lookupName)
{
	INFOMAN(m_deviceResources);

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterState;
	GFX_THROW_INFO(
		m_deviceResources->D3DDevice()->CreateRasterizerState(&desc, rasterState.ReleaseAndGetAddressOf())
	);

	m_rasterStateMap.insert(std::pair(lookupName, rasterState));
}

void ObjectStore::AddSamplerState(D3D11_SAMPLER_DESC desc, std::string lookupName)
{
	INFOMAN(m_deviceResources);

	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampleState;
	GFX_THROW_INFO(
		m_deviceResources->D3DDevice()->CreateSamplerState(&desc, sampleState.ReleaseAndGetAddressOf())
	);

	m_samplerStateMap.insert(std::pair(lookupName, sampleState));
}