#pragma once
#include "pch.h"

#include "DeviceResources.h"
#include "DeviceResourcesException.h"
#include "ObjectStoreException.h"

#include <memory>
#include <string>
#include <map>

#ifndef NDEBUG
#define MAP_LOOKUP(map, key) if (map.find(key) != map.end()) {return map[key];} else {throw ObjectStoreException(__LINE__, __FILE__, #map, key);} 
#else
#define MAP_LOOKUP(map, key) return map[key];
#endif


// ObjectStore is intended to be a static class

class ObjectStore
{
public:
	static void Initialize(std::shared_ptr<DeviceResources> deviceResources);
	static void DestructObjects();

	static void AddVertexShader(std::wstring fileName, std::string lookupName);
	static void AddPixelShader(std::wstring fileName, std::string lookupName);

	static Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader(std::string lookupName) { MAP_LOOKUP(m_vertexShaderMap, lookupName); }
	static Microsoft::WRL::ComPtr<ID3DBlob> GetVertexShaderBlob(std::string lookupName) { MAP_LOOKUP(m_vertexShaderBlobMap, lookupName); }
	static Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader(std::string lookupName) { MAP_LOOKUP(m_pixelShaderMap, lookupName); }



private:
	ObjectStore() {} // Disallow creation of an ObjectStore object

	static std::shared_ptr<DeviceResources> m_deviceResources;

	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11VertexShader>> m_vertexShaderMap;
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> m_vertexShaderBlobMap;
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>> m_pixelShaderMap;
};