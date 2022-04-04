#include "ModelMesh.h"

using DirectX::XMFLOAT3;
using DirectX::XMFLOAT2;

ModelMesh::ModelMesh(std::shared_ptr<DeviceResources> deviceResources, std::string filename) :
	Mesh(deviceResources)
{
	const std::filesystem::path filePath = filename;
	std::string extension = filePath.extension().string();
	if (extension == ".obj")
	{
		LoadOBJ(filename);
	}
	else
	{
		std::ostringstream oss;
		oss << "Unsupported 3D model format: " << filename;
		throw ModelMeshException(__LINE__, __FILE__, oss.str());
	}
}

void ModelMesh::LoadOBJ(std::string filename)
{
	std::vector<XMFLOAT3> positions;
	std::vector<XMFLOAT2> textures;
	std::vector<XMFLOAT3> normals;
	std::vector<int> indexLookup;

	std::vector<OBJVertex> vertices;
	std::vector<unsigned short> indices;

	std::string line;
	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::ostringstream oss;
		oss << "Could not open file: " << filename;
		throw ModelMeshException(__LINE__, __FILE__, oss.str());
	}

	std::vector<std::string> tokens;
	std::vector<std::string> faceTokens;
	std::istringstream iss;
	std::istringstream faceISS;
	std::string item;
	int lineNumber = 0;
	unsigned short parsedPositionIndex, parsedTextureIndex, parsedNormalIndex;

	while (std::getline(file, line))
	{
		++lineNumber;
		tokens.clear();
		iss = std::istringstream(line);
		while (std::getline(iss, item, ' '))
			tokens.push_back(item);

		if (tokens.size() == 0)
			continue;

		if (tokens[0] == "v")
		{
			if (tokens.size() != 4)
			{
				std::ostringstream oss;
				oss << "File '" << filename << "' has invalid number of vertex parameters at line " << lineNumber;
				throw ModelMeshException(__LINE__, __FILE__, oss.str());
			}

			positions.push_back(XMFLOAT3());
			positions[positions.size() - 1].x = std::stof(tokens[1]);
			positions[positions.size() - 1].y = std::stof(tokens[2]);
			positions[positions.size() - 1].z = std::stof(tokens[3]);

			indexLookup.push_back(-1);
		}
		else if (tokens[0] == "vt")
		{
			if (tokens.size() != 3)
			{
				std::ostringstream oss;
				oss << "File '" << filename << "' has invalid number of vertex texture parameters at line " << lineNumber;
				throw ModelMeshException(__LINE__, __FILE__, oss.str());
			}

			textures.push_back(XMFLOAT2());
			textures[textures.size() - 1].x = std::stof(tokens[1]);
			textures[textures.size() - 1].y = std::stof(tokens[2]);
		}
		else if (tokens[0] == "vn")
		{
			if (tokens.size() != 4)
			{
				std::ostringstream oss;
				oss << "File '" << filename << "' has invalid number of vertex normal parameters at line " << lineNumber;
				throw ModelMeshException(__LINE__, __FILE__, oss.str());
			}

			normals.push_back(XMFLOAT3());
			normals[normals.size() - 1].x = std::stof(tokens[1]);
			normals[normals.size() - 1].y = std::stof(tokens[2]);
			normals[normals.size() - 1].z = std::stof(tokens[3]);
		}
		else if (tokens[0] == "f")
		{
			if (tokens.size() != 4)
			{
				std::ostringstream oss;
				oss << "File '" << filename << "' has invalid number of face parameters at line " << lineNumber;
				throw ModelMeshException(__LINE__, __FILE__, oss.str());
			}

			// Iterate over each of the three v/vt/vn values
			for (int iii = 1; iii <= 3; ++iii)
			{
				faceTokens.clear();
				faceISS = std::istringstream(tokens[iii]);
				while (std::getline(faceISS, item, '/'))
					faceTokens.push_back(item);

				if (faceTokens.size() != 3)
				{
					std::ostringstream oss;
					oss << "File '" << filename << "' has invalid number of face tokens at line " << lineNumber;
					throw ModelMeshException(__LINE__, __FILE__, oss.str());
				}

				parsedPositionIndex = static_cast<unsigned short>(std::stoi(faceTokens[0]) - 1); // OBJ is NOT 0 indexed - index starts at 1 so we need to subtract 1 here
				parsedTextureIndex = static_cast<unsigned short>(std::stoi(faceTokens[1]) - 1);
				parsedNormalIndex = static_cast<unsigned short>(std::stoi(faceTokens[2]) - 1);

				// If the position was not used to create a previous vertex, then create a new one
				if (indexLookup[parsedPositionIndex] == -1)
				{
					// Create and add new vertex
					vertices.push_back(OBJVertex());
					vertices[vertices.size() - 1].position = positions[parsedPositionIndex];
					vertices[vertices.size() - 1].texture = textures[parsedTextureIndex];
					vertices[vertices.size() - 1].normal = normals[parsedNormalIndex];

					// Add new index for the vertex
					indices.push_back(static_cast<unsigned short>(vertices.size() - 1));

					// Update the lookup list so we know not to create a duplicate vertex for this position
					indexLookup[parsedPositionIndex] = static_cast<int>(vertices.size() - 1);
				}
				else
				{
					// Vertex was already added - just add the index
					indices.push_back(static_cast<unsigned short>(indexLookup[parsedPositionIndex]));
				}
			}
		}
	}

	file.close();

	// Create Buffer ======================================================================
	INFOMAN(m_deviceResources);

	m_sizeOfVertex = sizeof(OBJVertex);
	m_vertexCount = static_cast<unsigned int>(vertices.size());

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = static_cast<UINT>(vertices.size()) * m_sizeOfVertex;
	bd.StructureByteStride = m_sizeOfVertex;
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices.data();
	GFX_THROW_INFO(m_deviceResources->D3DDevice()->CreateBuffer(&bd, &sd, &m_vertexBuffer));


	// Index Buffer
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = static_cast<UINT>(indices.size() * sizeof(unsigned short)); //sizeof(indices);
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices.data();
	GFX_THROW_INFO(m_deviceResources->D3DDevice()->CreateBuffer(&ibd, &isd, &m_indexBuffer));

	m_indexCount = static_cast<unsigned int>(indices.size());
}