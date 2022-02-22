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

	static void AddInputLayout(std::string lookupName, std::shared_ptr<InputLayout> inputLayout) { m_inputLayoutMap.insert(std::pair(lookupName, inputLayout)); }
	static void AddVertexShader(std::string lookupName, std::shared_ptr<VertexShader> vertexShader) { m_vertexShaderMap.insert(std::pair(lookupName, vertexShader)); }
	static void AddPixelShader(std::string lookupName, std::shared_ptr<PixelShader> pixelShader) { m_pixelShaderMap.insert(std::pair(lookupName, pixelShader)); }
 	static void AddRasterizerState(std::string lookupName, std::shared_ptr<RasterizerState> rasterizerState) { m_rasterizerStateMap.insert(std::pair(lookupName, rasterizerState)); }
	static void AddConstantBuffer(std::string lookupName, std::shared_ptr<ConstantBuffer> constantBuffer) { m_constantBufferMap.insert(std::pair(lookupName, constantBuffer)); }
	static void AddConstantBufferArray(std::string lookupName, std::shared_ptr<ConstantBufferArray> constantBufferArray) { m_constantBufferArrayMap.insert(std::pair(lookupName, constantBufferArray)); }



	static void AddSamplerState(D3D11_SAMPLER_DESC desc, std::string lookupName);
	static void AddMesh(std::shared_ptr<Mesh> mesh, std::string lookupName) { m_meshMap.insert(std::pair(lookupName, mesh)); }
	static void AddTexture(std::shared_ptr<Texture> texture, std::string lookupName) { m_textureMap.insert(std::pair(lookupName, texture)); }
	static void AddDepthStencilState(std::shared_ptr<DepthStencilState> depthStencilState, std::string lookupName) { m_depthStencilStateMap.insert(std::pair(lookupName, depthStencilState)); }
	static void AddTerrain(std::shared_ptr<TerrainMesh> terrainMesh, std::string lookupName) { m_terrainMeshMap.insert(std::pair(lookupName, terrainMesh)); }


	static std::shared_ptr<VertexShader> GetVertexShader(std::string lookupName) { MAP_LOOKUP(m_vertexShaderMap, lookupName); }
	static std::shared_ptr<InputLayout> GetInputLayout(std::string lookupName) { MAP_LOOKUP(m_inputLayoutMap, lookupName); }
	static std::shared_ptr<PixelShader> GetPixelShader(std::string lookupName) { MAP_LOOKUP(m_pixelShaderMap, lookupName); }
	static std::shared_ptr<RasterizerState> GetRasterizerState(std::string lookupName) { MAP_LOOKUP(m_rasterizerStateMap, lookupName); }
	static std::shared_ptr<ConstantBuffer> GetConstantBuffer(std::string lookupName) { MAP_LOOKUP(m_constantBufferMap, lookupName); }
	static std::shared_ptr<ConstantBufferArray> GetConstantBufferArray(std::string lookupName) { MAP_LOOKUP(m_constantBufferArrayMap, lookupName); }


	static Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSamplerState(std::string lookupName) { MAP_LOOKUP(m_samplerStateMap, lookupName); }
	static std::shared_ptr<Mesh> GetMesh(std::string lookupName) { MAP_LOOKUP(m_meshMap, lookupName); }
	static std::shared_ptr<Texture> GetTexture(std::string lookupName) { MAP_LOOKUP(m_textureMap, lookupName); }
	static std::shared_ptr<DepthStencilState> GetDepthStencilState(std::string lookupName) { MAP_LOOKUP(m_depthStencilStateMap, lookupName); }
	static std::shared_ptr<TerrainMesh> GetTerrain(std::string lookupName) { MAP_LOOKUP(m_terrainMeshMap, lookupName); }

private:
	ObjectStore() {} // Disallow creation of an ObjectStore object

	static std::shared_ptr<DeviceResources> m_deviceResources;

	static std::map<std::string, std::shared_ptr<VertexShader>> m_vertexShaderMap;
	static std::map<std::string, std::shared_ptr<InputLayout>> m_inputLayoutMap;
	static std::map<std::string, std::shared_ptr<PixelShader>> m_pixelShaderMap;
	static std::map<std::string, std::shared_ptr<RasterizerState>> m_rasterizerStateMap;
	static std::map<std::string, std::shared_ptr<ConstantBuffer>> m_constantBufferMap;
	static std::map<std::string, std::shared_ptr<ConstantBufferArray>> m_constantBufferArrayMap;

	
	
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>>	m_samplerStateMap;

	static std::map<std::string, std::shared_ptr<Mesh>> m_meshMap;
	static std::map<std::string, std::shared_ptr<Texture>> m_textureMap;
	static std::map<std::string, std::shared_ptr<DepthStencilState>> m_depthStencilStateMap;
	static std::map<std::string, std::shared_ptr<TerrainMesh>> m_terrainMeshMap;

	
};
