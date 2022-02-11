#pragma once
#include "pch.h"

#include "DeviceResources.h"
#include "DeviceResourcesException.h"
#include "ObjectStoreException.h"
#include "Mesh.h"
#include "Texture.h"
#include "DepthStencilState.h"

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

	static void AddVertexShaderAndInputLayout(std::wstring vertexShaderFile, const D3D11_INPUT_ELEMENT_DESC* inputDescription, UINT numElements, std::string lookupName);
	static void AddPixelShader(std::wstring fileName, std::string lookupName);	
	static void AddRasterState(D3D11_RASTERIZER_DESC desc, std::string lookupName);
	static void AddSamplerState(D3D11_SAMPLER_DESC desc, std::string lookupName);
	static void AddMesh(std::shared_ptr<Mesh> mesh, std::string lookupName) { m_meshMap.insert(std::pair(lookupName, mesh)); }
	static void AddTexture(std::shared_ptr<Texture> texture, std::string lookupName) { m_textureMap.insert(std::pair(lookupName, texture)); }
	static void AddDepthStencilState(std::shared_ptr<DepthStencilState> depthStencilState, std::string lookupName) { m_depthStencilStateMap.insert(std::pair(lookupName, depthStencilState)); }


	template <typename T>
	static void AddConstantBuffer(std::string lookupName, D3D11_USAGE usage, UINT CPUAccessFlags);

	static Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader(std::string lookupName) { MAP_LOOKUP(m_vertexShaderMap, lookupName); }
	static Microsoft::WRL::ComPtr<ID3D11InputLayout> GetInputLayout(std::string lookupName) { MAP_LOOKUP(m_inputLayoutMap, lookupName); }
	static Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader(std::string lookupName) { MAP_LOOKUP(m_pixelShaderMap, lookupName); }
	static Microsoft::WRL::ComPtr<ID3D11Buffer> GetConstantBuffer(std::string lookupName) { MAP_LOOKUP(m_constantBufferMap, lookupName); }
	static Microsoft::WRL::ComPtr<ID3D11RasterizerState> GetRasterState(std::string lookupName) { MAP_LOOKUP(m_rasterStateMap, lookupName); }
	static Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSamplerState(std::string lookupName) { MAP_LOOKUP(m_samplerStateMap, lookupName); }
	static std::shared_ptr<Mesh> GetMesh(std::string lookupName) { MAP_LOOKUP(m_meshMap, lookupName); }
	static std::shared_ptr<Texture> GetTexture(std::string lookupName) { MAP_LOOKUP(m_textureMap, lookupName); }
	static std::shared_ptr<DepthStencilState> GetDepthStencilState(std::string lookupName) { MAP_LOOKUP(m_depthStencilStateMap, lookupName); }

private:
	ObjectStore() {} // Disallow creation of an ObjectStore object

	static std::shared_ptr<DeviceResources> m_deviceResources;

	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11VertexShader>>	m_vertexShaderMap;
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11InputLayout>>		m_inputLayoutMap;
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>>		m_pixelShaderMap;
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11Buffer>>			m_constantBufferMap;
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11RasterizerState>> m_rasterStateMap;
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>>	m_samplerStateMap;

	static std::map<std::string, std::shared_ptr<Mesh>> m_meshMap;
	static std::map<std::string, std::shared_ptr<Texture>> m_textureMap;
	static std::map<std::string, std::shared_ptr<DepthStencilState>> m_depthStencilStateMap;

	
};

template <typename T>
void ObjectStore::AddConstantBuffer(std::string lookupName, D3D11_USAGE usage, UINT CPUAccessFlags)
{
	INFOMAN(m_deviceResources);

	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	CD3D11_BUFFER_DESC constantBufferDesc(sizeof(T), D3D11_BIND_CONSTANT_BUFFER, usage, CPUAccessFlags);
	GFX_THROW_INFO(
		m_deviceResources->D3DDevice()->CreateBuffer(
			&constantBufferDesc, // Use the description we just created
			nullptr,			 // Don't fill it with any data
			&buffer			     // Assign result to buffer
		)
	);

	m_constantBufferMap.insert(std::pair(lookupName, buffer));
}