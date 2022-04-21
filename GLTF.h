#pragma once
#include "pch.h"
#include "GLTFException.h"
#include "Json.h"
#include "Base64.h"

#include <string>
#include <vector>
#include <map>
#include <algorithm>

struct GLTFBufferView
{
	int m_bufferIndex;
	int m_byteLength;
	int m_byteOffset;
};

struct GLTFAccessor
{
	int m_bufferView;
	int m_componentTypeByteCount;
	int m_count;
	std::vector<float> m_maxValues;
	std::vector<float> m_minValues;
	int m_typeComponentCount;
};

struct GLTFpbrMetallicRoughness
{
	DirectX::XMFLOAT4 m_baseColorFactor;
	float m_metallicFactor;
	float m_roughnessFactor;
};

struct GLTFMaterial
{
	bool m_doubleSided;
	std::string m_name;
	GLTFpbrMetallicRoughness m_pbrMetallicRoughness;
};

struct GLTFMeshPrimitive
{
	std::map<std::string, int> m_attributes;	// key = attribute type, value = index into vector of buffer views
	int m_indicesIndex;		// Index into the vector of buffer views
	int m_materialIndex;	// Index into the vector of materials
};

struct GLTFMesh
{
	std::string m_name;
	std::vector<GLTFMeshPrimitive> m_primitives;
};

struct GLTFNode
{
	int					m_meshIndex;
	std::string			m_name;
	DirectX::XMFLOAT3	m_translation;
	std::vector<int>	m_childNodes;
};

struct GLTFScene
{
	std::string m_name;
	std::vector<int> m_rootNodeIndices;
};

class GLTF
{
public:
	GLTF(std::string filename);

private:
	void GetBuffer(Json& json);
	void GetBufferViews(Json& json);
	void GetAccessors(Json& json);
	void GetMaterials(Json& json);
	void GetMeshes(Json& json);
	void GetNodes(Json& json);
	void GetScenes(Json& json);

	float AnyCastToFloat(std::any val);

	std::vector<uint8_t>		m_buffer;
	std::vector<GLTFBufferView>	m_bufferViews;
	std::vector<GLTFAccessor>	m_accessors;
	std::vector<GLTFMaterial>	m_materials;
	std::vector<GLTFMesh>		m_meshes;
	std::vector<GLTFNode>		m_nodes;
	std::vector<GLTFScene>		m_scenes;
	int							m_sceneIndex;
};