#include "GLTF.h"

using DirectX::XMFLOAT3;

GLTF::GLTF(std::string filename)
{
	Json json = JsonReader::ReadFile(filename);
	GetBuffer(json);
	GetBufferViews(json);
	GetAccessors(json);
	GetMaterials(json);
	GetMeshes(json);
	GetNodes(json);
	GetScenes(json);
}

float GLTF::AnyCastToFloat(std::any val)
{
	// Helper method to any_cast to float
	// Necessary because some times values that should be floats are parsed as ints
	try
	{
		return std::any_cast<float>(val);
	}
	catch (const std::bad_any_cast& e)
	{
		return static_cast<float>(std::any_cast<int>(val));
	}
}

void GLTF::GetBuffer(Json& json)
{
	std::ostringstream oss;

	std::vector<std::any> buffers = json.Get<std::vector<std::any>>("buffers");

	if (buffers.size() > 1)
	{
		oss << "Invalid buffer size: " << buffers.size() << std::endl;
		oss << "We are currently only supporting a single buffer in a GLTF file." << std::endl;
		oss << "If you need this, you will have to add support for it";
		throw GLTFException(__LINE__, __FILE__, oss.str());
	}

	Json buffer = std::any_cast<Json>(buffers[0]);

	std::string uri = buffer.Get<std::string>("uri");

	if (uri.substr(0, 5) != "data:")
	{
		oss << "Invalid URI: " << uri.substr(0, 5) << "\nURI must begin with 'data:'" << std::endl;
		oss << "If you are trying to reference and load a binary file, you will need to add support for that.";
		throw GLTFException(__LINE__, __FILE__, oss.str());
	}

	if (uri.substr(5, 32) != "application/octet-stream;base64,")
	{
		oss << "URI data type NOT supported: " << uri.substr(5, 32) << "\nIf you need this data type, you will need to add support for it.";
		throw GLTFException(__LINE__, __FILE__, oss.str());
	}

	uri = uri.substr(37, uri.size());

	m_buffer = Base64::Decode(uri);

	// Make sure the that amount of data parsed matches the amount of data specified in the file
	int byteLength = buffer.Get<int>("byteLength");

	if (m_buffer.size() != byteLength)
	{
		oss << "Failed to read the correct number bytes from the buffer URI" << std::endl;
		oss << "Bytes read:     " << m_buffer.size() << std::endl;
		oss << "Bytes expected: " << byteLength << std::endl;
		throw GLTFException(__LINE__, __FILE__, oss.str());
	}
}

void GLTF::GetBufferViews(Json& json)
{
	std::ostringstream oss;

	std::vector<std::any> bufferViews = json.Get<std::vector<std::any>>("bufferViews");

	Json bufferViewObject;
	GLTFBufferView bufferView;

	for (unsigned int iii = 0; iii < bufferViews.size(); ++iii)
	{
		bufferViewObject = std::any_cast<Json>(bufferViews[iii]);

		// reset the object to be populated
		bufferView = GLTFBufferView();

		bufferView.m_bufferIndex = bufferViewObject.Get<int>("buffer");
		bufferView.m_byteLength  = bufferViewObject.Get<int>("byteLength");
		bufferView.m_byteOffset  = bufferViewObject.Get<int>("byteOffset");

		if (bufferView.m_bufferIndex > 0)
		{
			oss << "Invalid buffer index: " << bufferView.m_bufferIndex << std::endl;
			oss << "We are currently only supporting a single buffer in a GLTF file." << std::endl;
			oss << "If you need this, you will have to add support for it";
			throw GLTFException(__LINE__, __FILE__, oss.str());
		}

		m_bufferViews.push_back(bufferView);
	}
}

void GLTF::GetAccessors(Json& json)
{
	std::ostringstream oss;

	std::vector<std::any> accessors = json.Get<std::vector<std::any>>("accessors");

	Json accessorJson;
	GLTFAccessor accessor;

	int componentType;
	std::string type;
	std::vector<std::any> minMaxValues;

	for (unsigned int iii = 0; iii < accessors.size(); ++iii)
	{
		accessorJson = std::any_cast<Json>(accessors[iii]);

		// reset the object to be populated
		accessor = GLTFAccessor();

		// Buffer Index
		accessor.m_bufferView = accessorJson.Get<int>("bufferView");

		if (accessor.m_bufferView >= m_bufferViews.size())
		{
			oss << "Invalid buffer view index: " << accessor.m_bufferView << std::endl;
			oss << "There are only a total of " << m_bufferViews.size() << " buffer views." << std::endl;
			throw GLTFException(__LINE__, __FILE__, oss.str());
		}

		// Component Type
		componentType = accessorJson.Get<int>("componentType");

		switch (componentType)
		{
		case 5120: accessor.m_componentTypeByteCount = 1; break; // signed byte
		case 5121: accessor.m_componentTypeByteCount = 1; break; // unsigned byte
		case 5122: accessor.m_componentTypeByteCount = 2; break; // signed short
		case 5123: accessor.m_componentTypeByteCount = 2; break; // unsigned short
		case 5125: accessor.m_componentTypeByteCount = 4; break; // unsigned int
		case 5126: accessor.m_componentTypeByteCount = 4; break; // signed float
		default:
			oss << "Invalid component type: " << componentType << std::endl;
			throw GLTFException(__LINE__, __FILE__, oss.str());
		}

		// Count
		accessor.m_count = accessorJson.Get<int>("count");

		// Type
		type = accessorJson.Get<std::string>("type");
		if (type == "SCALAR")
			accessor.m_typeComponentCount = 1;
		else if (type == "VEC2")
			accessor.m_typeComponentCount = 2;
		else if (type == "VEC3")
			accessor.m_typeComponentCount = 3;
		else if (type == "VEC4")
			accessor.m_typeComponentCount = 4;
		else if (type == "MAT2")
			accessor.m_typeComponentCount = 4;
		else if (type == "MAT3")
			accessor.m_typeComponentCount = 9;
		else if (type == "MAT4")
			accessor.m_typeComponentCount = 16;

		// Min / Max
		accessor.m_maxValues.resize(accessor.m_typeComponentCount, FLT_MAX);
		accessor.m_minValues.resize(accessor.m_typeComponentCount, -FLT_MAX);

		if (accessorJson.HasKey("max"))
		{
			minMaxValues = accessorJson.Get<std::vector<std::any>>("max");

			if (minMaxValues.size() != accessor.m_typeComponentCount)
			{
				oss << "Invalid number of max values for accessor #" << iii << std::endl;
				oss << "Expected: " << accessor.m_typeComponentCount << std::endl;
				oss << "Actual:   " << minMaxValues.size();
				throw GLTFException(__LINE__, __FILE__, oss.str());
			}

			for (unsigned int jjj = 0; jjj < minMaxValues.size(); ++jjj)
				accessor.m_maxValues[jjj] = AnyCastToFloat(minMaxValues[jjj]);
		}

		if (accessorJson.HasKey("min"))
		{
			minMaxValues = accessorJson.Get<std::vector<std::any>>("min");

			if (minMaxValues.size() != accessor.m_typeComponentCount)
			{
				oss << "Invalid number of min values for accessor #" << iii << std::endl;
				oss << "Expected: " << accessor.m_typeComponentCount << std::endl;
				oss << "Actual:   " << minMaxValues.size();
				throw GLTFException(__LINE__, __FILE__, oss.str());
			}

			for (unsigned int jjj = 0; jjj < minMaxValues.size(); ++jjj)
				accessor.m_minValues[jjj] = std::any_cast<float>(minMaxValues[jjj]);
		}


		m_accessors.push_back(accessor);
	}
}

void GLTF::GetMaterials(Json& json)
{
	std::ostringstream oss;

	std::vector<std::any> materials = json.Get<std::vector<std::any>>("materials");

	Json materialJson;
	GLTFMaterial material;

	Json metallicRoughnessJson;
	std::vector<std::any> baseColorFactor;

	for (unsigned int iii = 0; iii < materials.size(); ++iii)
	{
		materialJson = std::any_cast<Json>(materials[iii]);

		// reset the object to be populated
		material = GLTFMaterial();

		// Double Sided
		material.m_doubleSided = materialJson.Get<bool>("doubleSided");

		// Name
		material.m_name = materialJson.Get<std::string>("name");

		// Metallic Roughness
		metallicRoughnessJson = materialJson.Get<Json>("pbrMetallicRoughness");
		baseColorFactor = metallicRoughnessJson.Get<std::vector<std::any>>("baseColorFactor");
		if (baseColorFactor.size() != 4)
		{
			oss << "Invalid number of base color factors for material #" << iii << std::endl;
			oss << "Expected: " << 4 << std::endl;
			oss << "Actual:   " << baseColorFactor.size();
			throw GLTFException(__LINE__, __FILE__, oss.str());
		}
		material.m_pbrMetallicRoughness.m_baseColorFactor.x = AnyCastToFloat(baseColorFactor[0]);
		material.m_pbrMetallicRoughness.m_baseColorFactor.y = AnyCastToFloat(baseColorFactor[1]);
		material.m_pbrMetallicRoughness.m_baseColorFactor.z = AnyCastToFloat(baseColorFactor[2]);
		material.m_pbrMetallicRoughness.m_baseColorFactor.w = AnyCastToFloat(baseColorFactor[3]);

		material.m_pbrMetallicRoughness.m_metallicFactor = metallicRoughnessJson.Get<float>("metallicFactor");
		material.m_pbrMetallicRoughness.m_roughnessFactor = metallicRoughnessJson.Get<float>("roughnessFactor");

		m_materials.push_back(material);
	}
}

void GLTF::GetMeshes(Json& json)
{
	std::ostringstream oss;

	std::vector<std::any> meshes = json.Get<std::vector<std::any>>("meshes");

	Json meshJson;
	GLTFMesh mesh;

	Json meshPrimitiveJson;
	Json attributesJson;
	std::vector<std::any> primitives;
	std::vector<std::string> keys;

	for (unsigned int iii = 0; iii < meshes.size(); ++iii)
	{
		meshJson = std::any_cast<Json>(meshes[iii]);

		// reset the object to be populated
		mesh = GLTFMesh();

		// Name
		mesh.m_name = meshJson.Get<std::string>("name");

		// Primitives
		primitives = meshJson.Get<std::vector<std::any>>("primitives");
		for (unsigned int jjj = 0; jjj < primitives.size(); ++jjj)
		{
			meshPrimitiveJson = std::any_cast<Json>(primitives[jjj]);

			mesh.m_primitives.push_back(GLTFMeshPrimitive());

			// Indices
			mesh.m_primitives.back().m_indicesIndex = meshPrimitiveJson.Get<int>("indices");

			if (mesh.m_primitives.back().m_indicesIndex >= m_bufferViews.size())
			{
				oss << "Invalid primitive index value for mesh #" << iii << std::endl;
				oss << "Cannot be greater than the number of buffer views: " << m_bufferViews.size() << std::endl;
				oss << "Bad Value: " << mesh.m_primitives.back().m_indicesIndex << std::endl;
				throw GLTFException(__LINE__, __FILE__, oss.str());
			}

			// Material
			mesh.m_primitives.back().m_materialIndex = meshPrimitiveJson.Get<int>("material");

			if (mesh.m_primitives.back().m_materialIndex >= m_materials.size())
			{
				oss << "Invalid material index value for mesh #" << iii << std::endl;
				oss << "Cannot be greater than the number of materials: " << m_materials.size() << std::endl;
				oss << "Bad Value: " << mesh.m_primitives.back().m_materialIndex << std::endl;
				throw GLTFException(__LINE__, __FILE__, oss.str());
			}

			// Attributes
			attributesJson = meshPrimitiveJson.Get<Json>("attributes");
			keys = attributesJson.GetKeys();
			for (std::string key : keys)
			{
				mesh.m_primitives.back().m_attributes.insert(std::pair(key, attributesJson.Get<int>(key)));

				if (attributesJson.Get<int>(key) >= m_bufferViews.size())
				{
					oss << "Invalid primitive attribute index value for mesh #" << iii << std::endl;
					oss << "Cannot be greater than the number of buffer views: " << m_bufferViews.size() << std::endl;
					oss << "Bad Value: " << attributesJson.Get<int>(key) << std::endl;
					throw GLTFException(__LINE__, __FILE__, oss.str());
				}
			}
		}

		m_meshes.push_back(mesh);
	}
}

void GLTF::GetNodes(Json& json)
{
	std::ostringstream oss;

	std::vector<std::any> nodes = json.Get<std::vector<std::any>>("nodes");

	Json nodeJson;
	GLTFNode node;

	std::vector<std::any> translationValues;
	std::vector<std::any> childValues;

	for (unsigned int iii = 0; iii < nodes.size(); ++iii)
	{
		nodeJson = std::any_cast<Json>(nodes[iii]);

		// reset the object to be populated
		node = GLTFNode();

		// Name
		node.m_name = nodeJson.Get<std::string>("name");

		// Mesh
		node.m_meshIndex = nodeJson.Get<int>("mesh");

		if (node.m_meshIndex >= m_meshes.size())
		{
			oss << "Invalid mesh index for node #" << iii << std::endl;
			oss << "Cannot be greater than the number of meshes: " << m_meshes.size() << std::endl;
			oss << "Bad Value: " << node.m_meshIndex << std::endl;
			throw GLTFException(__LINE__, __FILE__, oss.str());
		}

		// Translation
		if (nodeJson.HasKey("translation"))
		{
			translationValues = nodeJson.Get<std::vector<std::any>>("translation");
			if (translationValues.size() != 3)
			{
				oss << "Invalid number of translation values for node #" << iii << std::endl;
				oss << "Expected Number of Values: 3" << std::endl;
				oss << "Actual Number of Values:   " << translationValues.size() << std::endl;
				throw GLTFException(__LINE__, __FILE__, oss.str());
			}

			node.m_translation.x = AnyCastToFloat(translationValues[0]);
			node.m_translation.y = AnyCastToFloat(translationValues[1]);
			node.m_translation.z = AnyCastToFloat(translationValues[2]);
		}
		else
			node.m_translation = XMFLOAT3(0.0f, 0.0f, 0.0f);

		// Children
		if (nodeJson.HasKey("children"))
		{
			childValues = nodeJson.Get<std::vector<std::any>>("children");

			for (std::any value : childValues)
				node.m_childNodes.push_back(std::any_cast<int>(value));
		}

		m_nodes.push_back(node);
	}

	// Do a final check to make sure that no node references a child index that is too large
	for (GLTFNode node : m_nodes)
	{
		if (node.m_childNodes.size() > 0)
		{
			if (*std::max_element(node.m_childNodes.begin(), node.m_childNodes.end()) >= m_nodes.size())
			{
				oss << "There is a node that has a child, but the child index is larger that the total number of nodes";
				throw GLTFException(__LINE__, __FILE__, oss.str());
			}
		}
	}
}

void GLTF::GetScenes(Json& json)
{
	std::ostringstream oss;

	std::vector<std::any> scenes = json.Get<std::vector<std::any>>("scenes");

	if (scenes.size() > 1)
	{
		oss << "Too many scenes specified: " << scenes.size() << std::endl;
		oss << "Currently only supporting a single scene object";
		throw GLTFException(__LINE__, __FILE__, oss.str());
	}

	Json sceneJson = std::any_cast<Json>(scenes[0]);

	GLTFScene scene;
	scene.m_name = sceneJson.Get<std::string>("name");

	std::vector<std::any> nodes = sceneJson.Get<std::vector<std::any>>("nodes");
	if (nodes.size() > 1)
	{
		oss << "Too many scene nodes specified: " << nodes.size() << std::endl;
		oss << "Currently only supporting a single scene root node";
		throw GLTFException(__LINE__, __FILE__, oss.str());
	}

	scene.m_rootNodeIndices.push_back(std::any_cast<int>(nodes[0]));

	m_scenes.push_back(scene);

	// Also get the scene index
	m_sceneIndex = json.Get<int>("scene");
	if (m_sceneIndex != 0)
	{
		oss << "Bad scene index value: " << m_sceneIndex << std::endl;
		oss << "Currently only supporting a single scene so the index must be 0";
		throw GLTFException(__LINE__, __FILE__, oss.str());
	}
}
