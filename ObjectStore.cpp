#include "ObjectStore.h"

std::shared_ptr<DeviceResources> ObjectStore::m_deviceResources = nullptr;

std::map<std::string, Microsoft::WRL::ComPtr<ID3D11VertexShader>> ObjectStore::m_vertexShaderMap;
std::map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> ObjectStore::m_vertexShaderBlobMap;
std::map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>> ObjectStore::m_pixelShaderMap;


void ObjectStore::Initialize(std::shared_ptr<DeviceResources> deviceResources)
{
	m_deviceResources = deviceResources;
}

void ObjectStore::DestructObjects()
{
	m_deviceResources = nullptr;
	m_vertexShaderMap.clear();
	m_vertexShaderBlobMap.clear();
	m_pixelShaderMap.clear();
}

void ObjectStore::AddVertexShader(std::wstring fileName, std::string lookupName)
{
	INFOMAN(m_deviceResources);

	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(fileName.c_str(), &pBlob));
	GFX_THROW_INFO(m_deviceResources->D3DDevice()->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));

	m_vertexShaderMap.insert(std::pair(lookupName, pVertexShader));
	m_vertexShaderBlobMap.insert(std::pair(lookupName, pBlob));
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