#include "ObjectStore.h"

std::shared_ptr<DeviceResources> ObjectStore::m_deviceResources = nullptr;

std::map<std::string, std::shared_ptr<VertexShader>>		ObjectStore::m_vertexShaderMap;
std::map<std::string, std::shared_ptr<InputLayout>>			ObjectStore::m_inputLayoutMap;
std::map<std::string, std::shared_ptr<PixelShader>>			ObjectStore::m_pixelShaderMap;
std::map<std::string, std::shared_ptr<RasterizerState>>		ObjectStore::m_rasterizerStateMap;
std::map<std::string, std::shared_ptr<ConstantBuffer>>		ObjectStore::m_constantBufferMap;
std::map<std::string, std::shared_ptr<ConstantBufferArray>> ObjectStore::m_constantBufferArrayMap;


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

void ObjectStore::AddSamplerState(D3D11_SAMPLER_DESC desc, std::string lookupName)
{
	INFOMAN(m_deviceResources);

	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampleState;
	GFX_THROW_INFO(
		m_deviceResources->D3DDevice()->CreateSamplerState(&desc, sampleState.ReleaseAndGetAddressOf())
	);

	m_samplerStateMap.insert(std::pair(lookupName, sampleState));
}