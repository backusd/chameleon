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
	// For now, we are not creating indices for OBJ objects, so do not use DrawIndexed
	m_drawIndexed = false;

	std::vector<XMFLOAT3> positions;
	std::vector<XMFLOAT2> textures;
	std::vector<XMFLOAT3> normals;

	std::vector<OBJVertex> vertices;

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

				vertices.push_back(OBJVertex());
				vertices[vertices.size() - 1].position = positions[std::stoi(faceTokens[0]) - 1]; // OBJ is NOT 0 indexed - index starts at 1 so we need to subtract 1 here
				vertices[vertices.size() - 1].texture  = textures[std::stoi(faceTokens[1]) - 1];
				vertices[vertices.size() - 1].normal   = normals[std::stoi(faceTokens[2]) - 1];
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

}

void ModelMesh::Bind()
{
	INFOMAN(m_deviceResources);
	ID3D11DeviceContext4* context = m_deviceResources->D3DDeviceContext();

	GFX_THROW_INFO_ONLY(
		context->IASetPrimitiveTopology(m_topology)
	);

	const UINT stride = m_sizeOfVertex;
	const UINT offset = 0u;
	GFX_THROW_INFO_ONLY(
		context->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &stride, &offset)
	);
	//GFX_THROW_INFO_ONLY(
	//	context->IASetIndexBuffer(m_indexBuffer.Get(), m_indexFormat, 0u)
	//);
}