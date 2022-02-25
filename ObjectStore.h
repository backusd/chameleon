#pragma once
#include "pch.h"

#include "DeviceResources.h"
#include "DeviceResourcesException.h"
#include "ObjectStoreException.h"
#include "Mesh.h"
#include "Texture.h"
#include "DepthStencilState.h"
#include "TerrainMesh.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "InputLayout.h"
#include "RasterizerState.h"
#include "ConstantBuffer.h"
#include "ConstantBufferArray.h"
#include "SamplerState.h"
#include "TextureArray.h"
#include "Bindable.h"

#include <memory>
#include <string>
#include <map>

#ifndef NDEBUG
#define MAP_LOOKUP(map, key) if (map.find(key) != map.end()) {return map[key];} else {throw ObjectStoreInvalidKeyException(__LINE__, __FILE__, #map, key);} 
#else
#define MAP_LOOKUP(map, key) return map[key];
#endif


// ObjectStore is intended to be a static class

class ObjectStore
{
public:
	static void Initialize(std::shared_ptr<DeviceResources> deviceResources);
	static void DestructObjects();


	static void AddTexture(std::string lookupName, std::shared_ptr<Texture> texture) { m_textureMap.insert(std::pair(lookupName, texture)); }
	static void AddConstantBuffer(std::string lookupName, std::shared_ptr<ConstantBuffer> constantBuffer) { m_constantBufferMap.insert(std::pair(lookupName, constantBuffer)); }
	static void AddMesh(std::string lookupName, std::shared_ptr<Mesh> mesh) { m_meshMap.insert(std::pair(lookupName, mesh)); }
	static void AddTerrainMesh(std::shared_ptr<TerrainMesh> terrainMesh, std::string lookupName) { m_terrainMeshMap.insert(std::pair(lookupName, terrainMesh)); }
	static void AddBindable(std::string lookupName, std::shared_ptr<Bindable> bindable) { m_bindablesMap.insert(std::pair(lookupName, bindable)); }


	static std::shared_ptr<Mesh> GetMesh(std::string lookupName) { MAP_LOOKUP(m_meshMap, lookupName); }
	static std::shared_ptr<ConstantBuffer> GetConstantBuffer(std::string lookupName) { MAP_LOOKUP(m_constantBufferMap, lookupName); }
	static std::shared_ptr<Texture> GetTexture(std::string lookupName) { MAP_LOOKUP(m_textureMap, lookupName); }
	static std::shared_ptr<TerrainMesh> GetTerrainMesh(std::string lookupName) { MAP_LOOKUP(m_terrainMeshMap, lookupName); }
	static std::shared_ptr<Bindable> GetBindable(std::string lookupName) { MAP_LOOKUP(m_bindablesMap, lookupName); }


private:
	ObjectStore() {} // Disallow creation of an ObjectStore object

	static std::shared_ptr<DeviceResources> m_deviceResources;
	
	static std::map<std::string, std::shared_ptr<ConstantBuffer>>	m_constantBufferMap;
	static std::map<std::string, std::shared_ptr<Texture>>			m_textureMap;
	static std::map<std::string, std::shared_ptr<TerrainMesh>>		m_terrainMeshMap;
	static std::map<std::string, std::shared_ptr<Mesh>>				m_meshMap;
	static std::map<std::string, std::shared_ptr<Bindable>>			m_bindablesMap;

};
