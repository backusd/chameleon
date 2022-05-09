#include "ObjectStore.h"

std::shared_ptr<DeviceResources> ObjectStore::m_deviceResources = nullptr;

std::map<std::string, std::shared_ptr<ConstantBuffer>>		ObjectStore::m_constantBufferMap;
std::map<std::string, std::shared_ptr<SamplerState>>		ObjectStore::m_sampleStateMap;
std::map<std::string, std::shared_ptr<TerrainMesh>>			ObjectStore::m_terrainMeshMap;
std::map<std::string, std::shared_ptr<Mesh>>				ObjectStore::m_meshMap;
std::map<std::string, std::shared_ptr<Texture>>				ObjectStore::m_textureMap;
std::map<std::string, std::shared_ptr<Bindable>>			ObjectStore::m_bindablesMap;



void ObjectStore::Initialize(std::shared_ptr<DeviceResources> deviceResources)
{
	m_deviceResources = deviceResources;
}

void ObjectStore::DestructObjects()
{
	m_deviceResources = nullptr;

	m_terrainMeshMap.clear();
	m_meshMap.clear();
	m_textureMap.clear();
	m_constantBufferMap.clear();
	m_bindablesMap.clear();
	m_sampleStateMap.clear();
}
